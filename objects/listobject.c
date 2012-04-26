#include "../co.h"

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
            COStr_Concat(&s, COStr_FromString(", "));
        COStr_Concat(&s, CO_TYPE(co)->tp_repr(co));
    }
    COStr_Concat(&s, COStr_FromString("]"));
    return s;
}

static void
list_dealloc(COListObject *this)
{
    ssize_t len = this->co_size;
    size_t i;
    if (len > 0) {
        i = len;
        while (--i >= 0)
            CO_XDECREF(this->co_item[i]);
    }
    COMem_FREE(this);
}

COTypeObject COList_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "list",
    sizeof(COListObject),
    0,
    (deallocfunc)list_dealloc,  /* tp_dealloc */
    (reprfunc)list_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
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
        // TODO errors: no memory
        return -1;
    }

    new_allocated += newsize;
    if (newsize == 0) {
        new_allocated = 0;
    }

    items = this->co_item;

    /* check for overflow */
    if (new_allocated <= ((~(ssize_t) 0) / sizeof(COObject *))) {
        items = COMem_REALLOC(items, new_allocated * sizeof(COObject *));
    } else {
        // TODO errors: no memory
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

    this = COObject_New(COListObject, &COList_Type);
    if (size <= 0) {
        this->co_item = NULL;
    } else {
        this->co_item = (COObject **)COMem_MALLOC(nbytes);
        if (this->co_item == NULL) {
            // TODO errors
            return NULL;
        }
        memset(this->co_item, 0, nbytes);
    }
    ((COListObject *)this)->co_size = size;
    this->allocated = size;
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
        // TODO errors
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
        // TODO errors
        return -1;
    }
    p = ((COListObject *)this)->co_item + index;
    olditem = *p;
    *p = item;
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
