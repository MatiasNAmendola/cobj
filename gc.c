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

COObject *
_COObject_GC_Malloc(size_t basicsize)
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
_COObject_GC_New(COTypeObject *tp)
{
    COObject *o = _COObject_GC_Malloc(tp->tp_basicsize);
}
