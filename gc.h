#ifndef GC_H
#define GC_H
/**
 * Reference Cycle Garbage Collection
 *
 * !!! Based on Python GC module.
 *
 * In COObject, each object has a reference count which indicates how many objects are
 * referring to it. When this reference count reaches zero the object is freed.
 * However, there is one fundamental flaw with reference couting and it's due to
 * something called reference cycle.
 *
 * If there is a reference cycle, this module is used to detct it and free it
 * together.
 *
 * How does this work?
 * ===================
 *  First, we observe that reference cycles can only be created by container
 *  objects (e.g. tuple/list/dict, etc). With this observation we
 *  realize that non-container objects can be ignored for the purpose of garbage
 *  collection.
 *
 *  Our idea now is to keep track of all container objects. There are several
 *  ways that this can be done but one of the best is using doubly linked list
 *  with the link fields inside the objects structure. This allows objects to be
 *  quickly inserted and removed from the set as well as not requiring extra
 *  memory allocations.
 *
 *  Now that we have access to all the container objects, how to we find
 *  reference cycles? First we to add another field to containers objects in
 *  addition to the two link pointers. We will call this field `gc_refs`. Here
 *  are the steps to find reference cycles:
 *      1. For each container object, set `gc_refs` equal to the object's
 *      reference count.
 *      2. For each container object, find which container objects it references
 *      and decrement the referenced container's `gc_refs` field.
 *      3. All container objects that now have a gc_refs field greater than one
 *      are referenced from outside the set of container objects. We cannot free
 *      these objects so we move them to a different set.
 *      4. Any objects referenced from the objects moved also cannot be freed.
 *      We move them and all the objects reachable from them too.
 *      5. Objects left in our original set are referenced only by objects
 *      within that set (ie. they are inaccessible and are garbage). We can now
 *      go about freeing these objects.
 *
 *  GC Generation & Threshold
 *  =========================
 *  The GC classifies objects into three generations depending on how many
 *  collection sweeps they have survivied. New objects are placed in the
 *  youngest generation (generation 0). If an object survives a collection it is
 *  moved into the next older generation. Since generation 2 is the oldest
 *  generation, objects in that generation remain there after a collection. In
 *  order to decide when to run, the collector keeps track of the number object
 *  allocations and deallocations since the last collection. When the number of
 *  allocations minus the number of deallocations exceed threshold0, collection
 *  starts. Initially only generation 0 is examined. If generation 0 has been
 *  examied more than threshold1 times since generation 1 has been examined,
 *  then generation 1 is examined as well. Similarly, threshold2 controls the
 *  number of collections of generation 1 before collecting generation 2.
 */

#include "object.h"

// GC information is stored before the object structure.
typedef union _gc_head {
    struct {
        union _gc_head *gc_next;
        union _gc_head *gc_prev;
        ssize_t gc_refs;
    } gc;
    long double dummy;          /* force worst-case alignment */
} gc_head;

#define AS_GC(o)    ((gc_head *)(o) - 1)
#define FROM_GC(o)  ((COObject *)((gc_head *)o + 1))

/* gc_refs values.
 * 
 * Between collections, every object has one of two gc_refs values:
 *
 * - GC_UNTRACKED
 *  The initial state, objects returned by COObject_GC_Malloc are in this state.
 *  The object doesn't live in any generation list, and its tp_traverse slot
 *  must not be called.
 *
 * - GC_REACHABLE
 *  The object lives in some generation list, and its tp_traverse is safe to
 *  call. An object transitions to GC_REACHABLE when COObject_GC_TRACK is called.
 *
 * During a collection, gc_refs can temporarily take on other states:
 *
 *  - >= 0
 *      At the start of a collection, `update_refs()` copies the true refcount
 *      to gc_refs, for each object in the generation being collected.
 *      `subtract_refs()` then adjusts gc_refs so that it equals the number of
 *      times an object is referenceed directly from outside the generation
 *      being collected.
 *      gc_refs remains >=0 throughout these steps.
 *
 *  - GC_TENTATIVELY_UNREACHABLE
 *      `move_unreachable()` then moves objects which maybe unreachable from
 *      outside the generation into an "unreachable" set. It's "tentatively"
 *      because the pass doing this cannot be sure until it ends, and
 *      GC_TENTATIVELY_UNREACHABLE may transition back to GC_REACHABLE.
 */
#define GC_UNTRACKED                    -1
#define GC_REACHABLE                    -2
#define GC_TENTATIVELY_UNREACHABLE      -3

#define IS_TRACKED(o)                   ((AS_GC(o))->gc.gc_refs != GC_UNTRACKED)
#define IS_REACHABLE(o)                 ((AS_GC(o))->gc.gc_refs == GC_REACHABLE)
#define IS_TENTATIVELY_UNREACHABLE(o)   ((AS_GC(o))->gc.gc_refs == GC_TENTATIVELY_UNREACHABLE)

void COObject_GC_Init(void);
COObject *COObject_GC_New(COTypeObject *tp);
COVarObject *COVarObject_GC_New(COTypeObject *tp, ssize_t nitems);
void COObject_GC_Free(void *o);
ssize_t COObject_GC_Collect(void);

gc_head *gc_generation0;

/* Tell the GC to track this object.
 */
#define COObject_GC_TRACK(o)                        \
    do {                                            \
        gc_head *g = AS_GC(o);                      \
        if (g->gc.gc_refs != GC_UNTRACKED)          \
            error("GC object alread tracked");      \
        g->gc.gc_refs = GC_REACHABLE;               \
        g->gc.gc_next = gc_generation0;             \
        g->gc.gc_prev = gc_generation0->gc.gc_prev; \
        g->gc.gc_prev->gc.gc_next = g;              \
        gc_generation0->gc.gc_prev = g;             \
    } while(0)

/* Tell the GC to stop tracking this object.
 * gc_next doesn't need to be set to NULL, but doing so is a good
 * way to provoke memory errors if calling code is confused.
 */
#define COObject_GC_UNTRACK(o)                  \
    do {                                        \
        gc_head *g = AS_GC(o);                      \
        assert(g->gc.gc_refs != GC_UNTRACKED);      \
        g->gc.gc_refs = GC_UNTRACKED;               \
        g->gc.gc_prev->gc.gc_next = g->gc.gc_next;  \
        g->gc.gc_next->gc.gc_prev = g->gc.gc_prev;  \
        g->gc.gc_next = NULL;                       \
    } while (0)

/* True if the object may be tracked by the GC in the future, or already is.
 * This can be useful to implement some optimizations.
 */
#define COObject_GC_MAYBE_TRACKED(o)    \
    (COObject_IS_GC(o) && (!COTuple_Check(o) || IS_TRACKED(o)))

#endif
