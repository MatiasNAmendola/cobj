#include "../cobj.h"

/* Speed optimization to avoid frequent malloc/free of small tuples. */
#define COTuple_MAXSAVESIZE 0   // Largest tuple to save on free list.
#define COTuple_MAXFREELIST 2000        // Maximum number of tuples of each size to save.

#if COTuple_MAXSAVESIZE > 0
/* Slot 0 is empty tuple.
 * Slots 1 to COTuple_MAXSAVESIZE is free lists.
 */
static COTupleObject *free_list[COTuple_MAXSAVESIZE];
static int numfree[COTuple_MAXSAVESIZE];
#endif

static COObject *
tuple_repr(COTupleObject *this)
{
    ssize_t i = CO_SIZE(this);
    if (i == 0) {
        return COStr_FromString("()");
    }
    COObject *s;
    s = COStr_FromString("(");
    for (i = 0; i < CO_SIZE(this); i++) {
        COObject *co = COTuple_GET_ITEM(this, i);
        if (i != 0)
            COStr_ConcatAndDel(&s, COStr_FromString(", "));
        COStr_ConcatAndDel(&s, COObject_Repr(co));
    }
    COStr_ConcatAndDel(&s, COStr_FromString(")"));
    return s;
}

static long
tuple_hash(COTupleObject *this)
{
    long x = 0L, y = 0L;
    ssize_t len;
    len = CO_SIZE(this);
    COObject **p = this->co_item;
    while (--len >= 0) {
        y = COObject_Hash(*p++);
        if (y == -1)
            return -1;
        x = (x ^ y) * (len + 1);
    }
    return x;
}

static void
tuple_dealloc(COTupleObject *this)
{
    ssize_t len = CO_SIZE(this);
    ssize_t i;
    if (len > 0) {
        i = len;
        while (--i >= 0) {
            CO_XDECREF(this->co_item[i]);
        }
#if COTuple_MAXSAVESIZE > 0
        if (len < COTuple_MAXSAVESIZE && numfree[len] < COTuple_MAXFREELIST) {
            this->co_item[0] = (COObject *)free_list[len];
            numfree[len]++;
            free_list[len] = this;
            if (IS_TRACKED(this)) {
                COObject_GC_UNTRACK(this);
            }
            return;
        }
#endif
    }
    COObject_GC_Free(this);
    return;
}

static int
tuple_traverse(COTupleObject *this, visitfunc visit, void *arg)
{
    ssize_t i;
    for (i = CO_SIZE(this); --i >= 0;)
        CO_VISIT(this->co_item[i]);
    return 0;
}

static COObject *
tuple_iter(COObject *seq)
{
    COTupleIterObject *it;
    if (!COTuple_Check(seq)) {
        COErr_BadInternalCall();
        return NULL;
    }
    it = COObject_GC_NEW(COTupleIterObject, &COTupleIter_Type);
    if (!it)
        return NULL;
    it->it_index = 0;
    CO_INCREF(seq);
    it->it_seq = (COTupleObject *)seq;
    COObject_GC_TRACK(it);
    return (COObject *)it;
}

static int
tuple_contains(COTupleObject *this, COObject *a)
{
    ssize_t i;
    int cmp;
    for (i = 0, cmp = 0; cmp == 0 && i < CO_SIZE(this); i++)
        cmp = COObject_CompareBool(a, COTuple_GET_ITEM(this, i), Cmp_EQ);
    return cmp;
}

static COSequenceInterface sequence_interface = {
    (binaryintfunc)tuple_contains,
};

COTypeObject COTuple_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "tuple",
    sizeof(COTupleObject) - sizeof(COObject *),
    sizeof(COObject *),
    COType_FLAG_GC,
    0,                            /* tp_new                  */
    (deallocfunc)tuple_dealloc,   /* tp_dealloc              */
    (reprfunc)tuple_repr,         /* tp_repr                 */
    0,                            /* tp_print                */
    (hashfunc)tuple_hash,         /* tp_hash                 */
    0,                            /* tp_compare              */
    (traversefunc)tuple_traverse, /* tp_traverse             */
    0,                            /* tp_clear                */
    0,                            /* tp_call                 */
    (getiterfunc)tuple_iter,      /* tp_iter                 */
    0,                            /* tp_iternext             */
    (lenfunc)COTuple_Size,        /* tp_len */
    0,                            /* tp_arithmetic_interface */
    0,                            /* tp_mapping_interface    */
    &sequence_interface,      /* tp_sequence_interface */
};

static COObject *
tuple_slice(COTupleObject *this, int ilow, int ihigh)
{
    COObject **src, **dest;
    int i, len;
    COTupleObject *co;
    if (ilow < 0)
        ilow = 0;
    else if (ilow > CO_SIZE(this))
        ilow = CO_SIZE(this);
    if (ihigh < ilow)
        ihigh = ilow;
    else if (ihigh > CO_SIZE(this))
        ihigh = CO_SIZE(this);
    len = ihigh - ilow;

    co = (COTupleObject *)COTuple_New(len);
    if (co == NULL)
        return NULL;

    src = this->co_item + ilow;
    dest = co->co_item;
    for (i = 0; i < len; i++) {
        dest[i] = src[i];
    }
    return (COObject *)co;
}

COObject *
COTuple_New(ssize_t size)
{
    COTupleObject *this;

    if (size < 0) {
        COErr_BadInternalCall();
        return NULL;
    }
#if COTuple_MAXSAVESIZE > 0
    if (size == 0 && free_list[0]) {
        this = free_list[0];
        CO_INCREF(this);
        return (COObject *)this;
    }
    if (size < COTuple_MAXSAVESIZE && (this = free_list[size]) != NULL) {
        free_list[size] = (COTupleObject *)this->co_item[0];    /* abused as next pointer */
        numfree[size]--;
        CO_REFCNT(this) = 1;
    } else
#endif
    {
        this = COVarObject_GC_NEW(COTupleObject, &COTuple_Type, size);
        if (!this)
            return NULL;
    }
    memset(this->co_item, 0, size * sizeof(COObject *));

#if COTuple_MAXSAVESIZE > 0
    if (size == 0) {
        free_list[0] = this;
        ++numfree[0];
        CO_INCREF(this);
    }
#endif

    COObject_GC_TRACK(this);

    return (COObject *)this;
}

ssize_t
COTuple_Size(COObject *this)
{
    return CO_SIZE(this);
}

COObject *
COTuple_GetItem(COObject *this, ssize_t index)
{
    if (index >= CO_SIZE(this)) {
        // TODO errors
        return NULL;
    }
    return ((COTupleObject *)this)->co_item[index];
}

int
COTuple_SetItem(COObject *this, ssize_t index, COObject *item)
{
    COObject **p;
    COObject *olditem;

    if (this == item) {
        error("tuple is immutable object, it's impossible to contain itself");
        return -1;
    }

    if (!COTuple_Check(this) || this->co_refcnt != 1) {
        CO_XDECREF(item);
        COErr_BadInternalCall();
        return -1;
    }

    if (index < 0 || index >= CO_SIZE(this)) {
        CO_XDECREF(item);
        COErr_SetString(COException_IndexError,
                        "tuple assignment index out of range");
        return -1;
    }

    p = ((COTupleObject *)this)->co_item + index;
    olditem = *p;
    *p = item;
    CO_XINCREF(item);
    CO_XDECREF(olditem);
    return 0;
}

COObject *
COTuple_GetSlice(COObject *this, int ilow, int ihigh)
{
    return tuple_slice((COTupleObject *)this, ilow, ihigh);
}
