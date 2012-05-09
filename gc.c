#include "co.h"

static int collecting = 0;

struct gc_generation {
    gc_head head;
    int threshold;
    int count;      /* count of allocations or collections of younger generations */
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
gc_list_init(gc_head *list)
{
    list->gc.gc_prev = list;
    list->gc.gc_next = list;
}

static int
gc_list_is_empty(gc_head *list)
{
    return (list->gc.gc_next == list);
}

/* Remove `node` from the gc list it's currently in. */
static void
gc_list_remove(gc_head *node)
{
    node->gc.gc_prev->gc.gc_next = node->gc.gc_next;
    node->gc.gc_next->gc.gc_prev = node->gc.gc_prev;
    node->gc.gc_next = NULL;
}

COObject *
COObject_GC_Malloc(size_t basicsize)
{
    COObject *o;
    gc_head *g;
    if (basicsize > SSIZE_MAX - sizeof(gc_head))
        return COErr_NoMemory();
    g = (gc_head *)COMem_MALLOC(sizeof(gc_head) + basicsize);
    if (!g)
        return COErr_NoMemory();

    g->gc.gc_refs = GC_UNTRACKED;

    generations[0].count++;

    if (generations[0].count > generations[0].threshold
            && !collecting && !COErr_Occurred()) {
        collecting = 1;
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
update_refs(gc_head *container)
{
    gc_head *gc = container->gc.gc_next;
    for (; gc != container; gc = gc->gc.gc_next) {
        assert(gc->gc.gc_refs == GC_REACHABLE);
        gc->gc.gc_refs = CO_REFCNT(FROM_GC(gc));
        /*
         * Cyclic gc should never see an incoming refcnt of 0.
         */
        assert(gc->gc.gc_refs != 0);
    }
}

/* Subtract internal references from gc_refs. After this, gc_refs is >= 0 for
 * all objects in containers, and is GC_REACHABLE for all tracked gc objects not
 * in containers. The ones with gc_refs > 0 are directly reachable from outside
 * containers, and so cannot be collected.
 */
static void
subtract_refs(gc_head *container)
{

}

/* Main Garbage Collecting Routine. */
static ssize_t
collect(int generation)
{
    int i;
    gc_head *young;

    young = gc_generation0;

    /* Using co_refcnt & gc_refs, calculate which objects in the container set
     * are reachable from outside. */
    update_refs(young);
    subtract_refs(young);
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
