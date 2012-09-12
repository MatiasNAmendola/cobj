#include "../cobj.h"

static COObject *
list_repr(COObject *this)
{
    ssize_t i = ((COListObject *)this)->co_size;
    if (i == 0) {
        return COStr_FromString("[]");
    }
    COObject *s;
    s = COStr_FromString("[");
    for (i = 0; i < ((COListObject *)this)->co_size; i++) {
        COObject *co = COList_GetItem(this, i);
        if (i != 0)
            COStr_ConcatAndDel(&s, COStr_FromString(", "));
        COStr_ConcatAndDel(&s, COObject_Repr(co));
    }
    COStr_ConcatAndDel(&s, COStr_FromString("]"));
    return s;
}

static void
list_dealloc(COListObject *this)
{
    ssize_t i;
    if (this->co_size > 0) {
        i = this->co_size;
        while (--i >= 0)
            CO_DECREF(this->co_item[i]);
    }
    COObject_Mem_FREE(this->co_item);
    COObject_GC_Free(this);
}

static COObject *
list_item(COListObject *this, ssize_t i)
{
    if (i < 0 || i >= this->co_size) {
        COErr_SetString(COException_IndexError, "list index out of range");
        return NULL;
    }

    CO_INCREF(this->co_item[i]);
    return this->co_item[i];
}

static COObject *
list_subscript(COListObject *this, COObject *index)
{
    if (COInt_Check(index)) {
        ssize_t i;
        i = COInt_AsSsize_t(index);
        if (i == -1 && COErr_Occurred()) {
            COErr_SetString(COException_IndexError, "list index out of range");
            return NULL;
        }
        if (i < 0)
            i += COList_GET_SIZE(this);
        return list_item(this, i);
    } else {
        COErr_Format(COException_TypeError,
                     "list indices must be integers, not %.200s",
                     index->co_type->tp_name);
        return NULL;
    }
}

static int
list_clear(COListObject *this)
{
    ssize_t i;
    COObject **item = this->co_item;
    if (item != NULL) {
        /* Because CO_XDECREF can recursively invoke operations on this list, we
         * make it empty first!
         */
        i = this->co_size;
        this->co_size = 0;
        this->co_item = NULL;
        this->allocated = 0;
        while (--i >= 0) {
            CO_XDECREF(item[i]);
        }
        COObject_Mem_FREE(item);
    }
    return 0;
}

static int
list_traverse(COListObject *this, visitfunc visit, void *arg)
{
    ssize_t i;
    for (i = this->co_size; --i >= 0;)
        CO_VISIT(this->co_item[i]);
    return 0;
}

static COObject *
list_iter(COObject *seq)
{
    COListIterObject *it;
    if (!COList_Check(seq)) {
        COErr_BadInternalCall();
        return NULL;
    }
    it = COObject_GC_NEW(COListIterObject, &COListIter_Type);
    if (!it)
        return NULL;
    it->it_index = 0;
    CO_INCREF(seq);
    it->it_seq = (COListObject *)seq;
    COObject_GC_TRACK(it);
    return (COObject *)it;
}

static int
list_ass_subscript(COObject *this, COObject *index, COObject *value)
{
    if (value == NULL)
        return COList_DelItem(this, COInt_AsSsize_t(index));
    else
        return COList_SetItem(this, COInt_AsSsize_t(index), value);
}

static int
list_contains(COListObject *this, COObject *a)
{
    ssize_t i;
    int cmp;
    for (i = 0, cmp = 0; cmp == 0 && i < CO_SIZE(this); i++)
        cmp = COObject_CompareBool(a, COList_GET_ITEM(this, i), Cmp_EQ);
    return cmp;
}

static COSequenceInterface sequence_interface = {
    (lenfunc)COList_Size,
    (binaryintfunc)list_contains,
};

static COMappingInterface mapping_interface = {
    (binaryfunc)list_subscript,
    (ternaryintfunc)list_ass_subscript,
};

COTypeObject COList_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "list",
    sizeof(COListObject),
    0,
    COType_FLAG_GC,
    0,                          /* tp_new                  */
    0,                          /* tp_init */
    (deallocfunc)list_dealloc,  /* tp_dealloc              */
    (reprfunc)list_repr,        /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    (traversefunc)list_traverse,        /* tp_traverse             */
    (inquiryfunc)list_clear,    /* tp_clear                */
    0,                          /* tp_call                 */
    (getiterfunc)list_iter,     /* tp_iter                 */
    0,                          /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    &mapping_interface,         /* tp_mapping_interface    */
    &sequence_interface,        /* tp_sequence_interface */
};

/*
 * Ensure co_item has room for at least newsize elements, and set co_size to
 * newsize. If newsize > co_size on entry, the content of the new slots at exit
 * is undefined heap trash; it's the caller's responsibility to overwrite them
 * with same values.
 *
 * The number of allocated elements may grow, shrink, or stay the same. Failure
 * is impossible if newsize <= allocated on entry, although that partly relies
 * on assumption that the system realloc() never fails when passed a number of
 * bytes <= the number of bytes last allocated.
 *
 * Note that co_item may change, and even if newsize is less than co_size on
 * entry.
 */
static int
list_resize(COListObject *this, ssize_t newsize)
{
    COObject **items;
    ssize_t new_allocated;
    ssize_t allocated = this->allocated;

    /* Bypass realloc() when a previous overallocation is large enough to
     * accommodate the newsize. If the newsize falls lower than half the
     * allocated size, then proceed with the realloc() to shrink the list.
     */
    if (allocated >= newsize && newsize >= (allocated >> 1)) {
        ((COListObject *)this)->co_size = newsize;
        return 0;
    }

    /* This over-allocates proportional to the list size, making room for
     * additional growth.
     */
    new_allocated = (newsize >> 3) + (newsize < 9 ? 3 : 6);

    /* check for overflow */
    if (new_allocated > SIZE_MAX - newsize) {
        COErr_NoMemory();
        return -1;
    }

    new_allocated += newsize;
    if (newsize == 0) {
        new_allocated = 0;
    }

    items = this->co_item;

    /* check for overflow */
    if (new_allocated <= ((~(ssize_t) 0) / sizeof(COObject *))) {
        items = COObject_Mem_REALLOC(items, new_allocated * sizeof(COObject *));
    } else {
        COErr_NoMemory();
        return -1;
    }

    this->co_item = items;
    ((COListObject *)this)->co_size = newsize;
    this->allocated = new_allocated;

    return 0;
}

static COObject *
list_slice(COListObject *this, int ilow, int ihigh)
{
    COObject **src, **dest;
    int i, len;
    COListObject *co;
    if (ilow < 0)
        ilow = 0;
    else if (ilow > ((COListObject *)this)->co_size)
        ilow = ((COListObject *)this)->co_size;
    if (ihigh < ilow)
        ihigh = ilow;
    else if (ihigh > ((COListObject *)this)->co_size)
        ihigh = ((COListObject *)this)->co_size;
    len = ihigh - ilow;

    co = (COListObject *)COList_New(len);
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
COList_New(ssize_t size)
{
    COListObject *this;
    ssize_t nbytes;
    nbytes = size * sizeof(COObject *);

    this = COObject_GC_NEW(COListObject, &COList_Type);
    if (size <= 0) {
        this->co_item = NULL;
    } else {
        this->co_item = (COObject **)COObject_Mem_MALLOC(nbytes);
        if (this->co_item == NULL) {
            CO_DECREF(this);
            return COErr_NoMemory();
        }
        memset(this->co_item, 0, nbytes);
    }
    ((COListObject *)this)->co_size = size;
    this->allocated = size;
    COObject_GC_TRACK(this);
    return (COObject *)this;
}

ssize_t
COList_Size(COObject *this)
{
    return ((COListObject *)this)->co_size;
}

COObject *
COList_GetItem(COObject *this, ssize_t index)
{
    if (index >= ((COListObject *)this)->co_size) {
        COErr_Format(COException_IndexError, "list index out of range");
        return NULL;
    }
    return ((COListObject *)this)->co_item[index];
}

int
COList_DelItem(COObject *this, ssize_t index)
{
    ssize_t n = ((COListObject *)this)->co_size;
    if (index < 0 || index >= n) {
        return -1;
    }

    COObject **items = ((COListObject *)this)->co_item;
    CO_DECREF(items[index]);

    ssize_t i;
    for (i = 0; i < n; i++) {
        if (i > index) {
            items[i - 1] = items[i];
        }
    }

    ((COListObject *)this)->co_size -= 1;
    return 0;
}

int
COList_SetItem(COObject *this, ssize_t index, COObject *item)
{
    COObject **p;
    COObject *olditem;
    if (index >= ((COListObject *)this)->co_size) {
        COErr_SetString(COException_IndexError, "list index out of range");
        return -1;
    }
    p = ((COListObject *)this)->co_item + index;
    olditem = *p;
    *p = item;
    CO_INCREF(item);
    CO_XDECREF(olditem);
    return 0;
}

int
COList_Insert(COObject *this, int index, COObject *item)
{
    ssize_t n = ((COListObject *)this)->co_size;

    if (list_resize((COListObject *)this, n + 1) == -1)
        return -1;

    if (index < 0) {
        index += n;
        if (index < 0) {
            index = 0;
        }
    }

    if (index > n)
        index = n;

    COObject **items = ((COListObject *)this)->co_item;
    ssize_t i;
    for (i = n; i > index; i--)
        items[i + 1] = items[i];

    CO_XINCREF(item);
    items[index] = item;
    return 0;
}

int
COList_Append(COObject *this, COObject *item)
{
    return COList_Insert(this, ((COListObject *)this)->co_size, item);
}

COObject *
COList_AsTuple(COObject *this)
{
    ssize_t n;
    COObject *co;
    COObject **p;
    COObject **q;

    n = ((COListObject *)this)->co_size;
    co = COTuple_New(n);
    p = ((COTupleObject *)co)->co_item;
    q = ((COListObject *)this)->co_item;

    while (n-- > 0) {
        CO_INCREF(*q);
        *p = *q;
        p++;
        q++;
    }

    return co;
}

COObject *
COList_GetSlice(COObject *this, int ilow, int ihigh)
{
    return list_slice((COListObject *)this, ilow, ihigh);
}
