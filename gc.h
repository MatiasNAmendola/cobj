#ifndef GC_H
#define GC_H
/**
 * Reference Cycle Garbage Collection
 *
 * In COObject, each object has a reference count which indicates how many objects are
 * referring to it. When this reference count reaches zero the object is freed.
 *
 * If where is a reference cycle, this module is used to detct it and free it
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
 *      1. For each container object, set gc_refs equal to the object's
 *      reference count.
 *      2. For each container object, find which container objects it references
 *      and decrement the referenced container's gc_refs field.
 *      3. All container objects that now have a gc_refs field greater than one
 *      are referenced from outside the set of container objects. We cannot free
 *      these objects so we move them to a different set.
 *      4. Any objects referenced from the objects moved also cannot be freed.
 *      We move them and all the objects reachable from them too.
 *      5. Objects left in our original set are referenced only by objects
 *      within that set (ie. they are inaccessible and are garbage). We can now
 *      go about freeing these objects.
 *
 *  What are the costs?
 *  ===================
 *  This form of garbage collection is fairly cheap. One of the biggest costs is
 *  the three extra words of memory required for each container object. There is
 *  also the overhead of maintaining the set of containers.
 *
 * !!! This module is based on Python GC module.
 */

#include "object.h"

// GC information is stored before the object structure.
typedef union _gc_head {
    struct {
        union _gc_head *gc_next;
        union _gc_head *gc_prev;
        ssize_t gc_refs;
    } gc;
    long double dummy;
} gc_head;

#define AS_GC(o)    ((gc_head *)(o) - 1)
#define FROM_GC(o)  ((COObject *)((gc_head *)o + 1))

/* gc_refs values.
 * 
 * Between collections, every object has one of two gc_refs values:
 *
 * GC_UNTRACKED
 *  The initial state. The object doesn't live in any generation list.
 *
 * GC_REACHABLE
 *  The object lives in some generation list, and its tp_traverse is safe to
 *  call. 
 *
 * During a collection, gc_refs can temporarily take on other states:
 *
 * >= 0
 * GC_TENTETIVELY_UNREACHABLE
 */
#define GC_UNTRACKED                    -1
#define GC_REACHABLE                    -2
#define GC_TENTETIVELY_UNREACHABLE      -3

#define IS_TRACKED(o)                   ((AS_GC(o))->gc.gc_refs != GC_UNTRACKED)
#define IS_REACHABLE(o)                 ((AS_GC(o))->gc.gc_refs == GC_REACHABLE)
#define IS_TENTETIVELY_UNREACHABLE(o)   ((AS_GC(o))->gc.gc_refs == GC_TENTETIVELY_UNREACHABLE)

void COObject_GC_Init(void);
COObject *COObject_GC_New(COTypeObject *tp);
COVarObject *COVarObject_GC_New(COTypeObject *tp, ssize_t nitems);
void COObject_GC_Free(void *o);
ssize_t COObject_GC_Collect(void);

gc_head *gc_generation0;

#define COObject_GC_TRACK(o)    do {            \
    gc_head *g = AS_GC(o);                      \
    if (g->gc.gc_refs != GC_UNTRACKED)          \
        error("GC object alread tracked");      \
    g->gc.gc_refs = GC_REACHABLE;               \
    g->gc.gc_next = gc_generation0;             \
    g->gc.gc_prev = gc_generation0->gc.gc_prev; \
    g->gc.gc_prev->gc.gc_next = g;              \
    gc_generation0->gc.gc_prev = g;             \
    } while(0);

#endif
