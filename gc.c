#include "co.h"

static int enabled = 1;             /* GC enabled? */
static int collecting = 0;          /* GC collecting? */
static COObject *garbage = NULL;    /* list of uncollectable objects */

struct gc_generation {
    gc_head head;
    int threshold;
    int count;                  /* count of allocations or collections of younger generations */
};

#define NUM_GENERATIONS 3
#define GEN_HEAD(n) (&generations[n].head)

/* linked list of container objects */
static struct gc_generation generations[NUM_GENERATIONS] = {
    {
     {
      {
       GEN_HEAD(0),
       GEN_HEAD(0),
       0,
       }
      },
     700,
     0,
     },
    {
     {
      {
       GEN_HEAD(1),
       GEN_HEAD(1),
       0,
       }
      },
     10,
     0,
     },
    {
     {
      {
       GEN_HEAD(2),
       GEN_HEAD(2),
       0,
       }
      },
     10,
     0,
     },
};

gc_head *gc_generation0 = GEN_HEAD(0);

/*** GC doubly linked list functions ***/
static void
gc_list_init(gc_head * list)
{
    list->gc.gc_prev = list;
    list->gc.gc_next = list;
}

static int
gc_list_is_empty(gc_head * list)
{
    return (list->gc.gc_next == list);
}

/* Remove `node` from the gc list it's currently in. */
static void
gc_list_remove(gc_head * node)
{
    node->gc.gc_prev->gc.gc_next = node->gc.gc_next;
    node->gc.gc_next->gc.gc_prev = node->gc.gc_prev;
    node->gc.gc_next = NULL;
}

/* Move `node` from the gc list it's currently in to the end of `list`. */
static void
gc_list_move(gc_head * node, gc_head * list)
{
    gc_head *current_prev = node->gc.gc_prev;
    gc_head *current_next = node->gc.gc_next;
    /* Unlink from current list. */
    current_prev->gc.gc_next = current_next;
    current_next->gc.gc_prev = current_prev;
    /* Relink at end of new list. */
    node->gc.gc_prev = list->gc.gc_prev;
    node->gc.gc_prev->gc.gc_next = node;
    list->gc.gc_prev = node;
    node->gc.gc_next = list;
}

/*** ! GC doubly linked list functions ***/

static ssize_t
collect_generations(void)
{
    // TODO
    return 0;
}

COObject *
COObject_GC_Malloc(size_t basicsize)
{
    COObject *o;
    gc_head *g;
    if (basicsize > SSIZE_MAX - sizeof(gc_head))
        return COErr_NoMemory();
    g = (gc_head *) COMem_MALLOC(sizeof(gc_head) + basicsize);
    if (!g)
        return COErr_NoMemory();

    g->gc.gc_refs = GC_UNTRACKED;

    generations[0].count++;

    if (generations[0].count > generations[0].threshold
        && enabled
        && !collecting
        && !COErr_Occurred()) {
        collecting = 1;
        collect_generations();
        collecting = 0;
    }

    o = FROM_GC(g);
    return o;
}

COObject *
COObject_GC_New(COTypeObject *tp)
{
    COObject *o = COObject_GC_Malloc(tp->tp_basicsize);
    if (o)
        o = COObject_INIT(o, tp);
    return o;
}

COVarObject *
COVarObject_GC_New(COTypeObject *tp, ssize_t nitems)
{
    const size_t size = COObject_VAR_SIZE(tp, nitems);
    COVarObject *o = (COVarObject *)COObject_GC_Malloc(size);
    if (o)
        o = COVarObject_INIT(o, tp, nitems);
    return o;
}

void
COObject_GC_Free(void *o)
{
    gc_head *g = AS_GC(o);

    if (IS_TRACKED(o)) {
        gc_list_remove(g);
    }

    if (generations[0].count > 0) {
        generations[0].count--;
    }

    COMem_FREE(g);
}

/*
 * Set all gc_refs = co_refcnt. 
 */
static void
update_refs(gc_head * container)
{
    gc_head *gc = container->gc.gc_next;
    for (; gc !=container; gc = gc->gc.gc_next) {
        assert(gc->gc.gc_refs == GC_REACHABLE);
        gc->gc.gc_refs = CO_REFCNT(FROM_GC(gc));
        /*
         * Cyclic gc should never see an incoming refcnt of 0.
         */
        assert(gc->gc.gc_refs != 0);
    }
}

static int
visit_decref(COObject *o, void *data)
{
    gc_head *gc = AS_GC(o);
    if (gc->gc.gc_refs > 0)
        gc->gc.gc_refs--;
    return 0;
}

/* Subtract internal references from gc_refs. After this, gc_refs is >= 0 for
 * all objects in containers, and is GC_REACHABLE for all tracked gc objects not
 * in containers. The ones with gc_refs > 0 are directly reachable from outside
 * containers, and so cannot be collected.
 */
static void
subtract_refs(gc_head * container)
{
    traversefunc traverser;
    gc_head *gc = container->gc.gc_next;
    for (; gc !=container; gc = gc->gc.gc_next) {
        traverser = CO_TYPE(FROM_GC(gc))->tp_traverse;
        (void)traverser(FROM_GC(gc), (visitfunc)visit_decref, NULL);
    }
}

/* 
 * Move the unreachable objects from young to unreachable. After this, all
 * objects in young have gc_refs = GC_REACHABLE, and all objects in unreachable
 * have gc_refs = GC_TENTETIVELY_UNREACHABLE. All tracked gc objects not in
 * young or unreachable still have gc_refs = GC_REACHABLE.
 *
 * All objects in young after this are directly or indirectly reachable from
 * outside the original young; and all objects in unreachable are not.
 */
static void
move_unreachable(gc_head * young, gc_head * unreachable)
{
    gc_head *gc = young->gc.gc_next;

    /* Invariants: all objects "to the left" of us in young have gc_refs =
     * GC_REACHABLE, and are indeed reachable (directly or indirectly) from
     * outside the young list as it was at entry. All other objects from the
     * original young "to the left" of us are in unreachable now, and have
     * gc_refs = GC_TENTETIVELY_UNREACHABLE. All objects to the left of us in
     * 'young' now have been scanned, and no objects here or to the right have
     * been scanned yet.
     */

    while (gc != young) {
        gc_head *next;

        if (gc->gc.gc_refs) {
            /* gc is definitely reachable from outside the original 'young'.
             * Mark it as such, and 
             */
            next = gc->gc.gc_next;
        } else {
            /* This *may* be unreachable. To make progress, assume it is.
             */
            next = gc->gc.gc_next;
            gc_list_move(gc, unreachable);
            gc->gc.gc_refs = GC_TENTETIVELY_UNREACHABLE;
        }
        gc = next;
    }
}

void
COObject_GC_Init(void)
{
    if (garbage == NULL) {
        garbage = COList_New(0);
        if (!garbage)
            return;
    }
}

/* 
 * Break reference cycles by clearing the containers involved.
 */
static void
delete_garbage(gc_head * collectable)
{
    inquiryfunc clear;

    while (!gc_list_is_empty(collectable)) {
        gc_head *gc = collectable->gc.gc_next;
        COObject *o = FROM_GC(gc);
        assert(IS_TENTETIVELY_UNREACHABLE(o));

        if ((clear = CO_TYPE(o)->tp_clear) != NULL) {
            CO_INCREF(o);
            clear(o);
            CO_DECREF(o);
        }

        if (collectable->gc.gc_next == gc) {
            // object is still alive, move it, it may die later
            /*gc_list_move(gc, old); */
            /*gc->gc.gc_refs = GC_REACHABLE; */
        }
    }
}

/* Main Garbage Collecting Routine. */
static ssize_t
collect(int generation)
{
    ssize_t m = 0;              /* objects collected */
    ssize_t n = 0;              /* uureachable objects that couldn't be collected */
    gc_head *young;
    gc_head unreachable;        /* non-problematic unreachable trash */
    gc_head *gc;

    young = gc_generation0;

    /* Using co_refcnt & gc_refs, calculate which objects in the container set
     * are reachable from outside. */
    update_refs(young);
    subtract_refs(young);

    /* Leave everything reachable from outside young in young, and more
     * everything else (in young) to unreachable.
     */
    gc_list_init(&unreachable);
    move_unreachable(young, &unreachable);

    for (gc = unreachable.gc.gc_next; gc !=&unreachable; gc = gc->gc.gc_next) {
        m++;
    }

    delete_garbage(&unreachable);

    return m + n;
}

ssize_t
COObject_GC_Collect(void)
{
    ssize_t n;
    if (collecting)
        n = 0;
    else {
        collecting = 1;
        n = collect(NUM_GENERATIONS - 1);
        collecting = 0;
    }

    return n;
}
