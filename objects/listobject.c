#include "../co.h"

static COObject *
list_repr(COObject *this)
{
    size_t i = CO_SIZE(this);
    if (i == 0) {
        return COStr_FromString("[]");
    }
    COStrObject *s;
    s = COStr_FromString("[");
    for (i = 0; i < CO_SIZE(this); i++) {
        COObject *co = COList_GetItem(this, i);
        if (i != 0)
            COStr_Concat(&s, COStr_FromString(", "));
        COStr_Concat(&s, (COStrObject *)CO_TYPE(co)->tp_repr(co));
    }
    COStr_Concat(&s, COStr_FromString("]"));
    return s;
}

COTypeObject COList_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "list",
    sizeof(COListObject),
    0,
    (reprfunc)list_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
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
list_resize(COListObject *this, size_t newsize)
{
    COObject **items;
    size_t new_allocated;
    size_t allocated = this->allocated;

    /* Bypass realloc() when a previous overallocation is large enough to
     * accommodate the newsize. If the newsize falls lower than half the
     * allocated size, then proceed with the realloc() to shrink the list.
     */
    if (allocated >= newsize && newsize >= (allocated >> 1)) {
        CO_SIZE(this) = newsize;
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
    if (new_allocated <= ((~(size_t) 0) / sizeof(COObject *))) {
        items = xrealloc(items, new_allocated * sizeof(COObject *));
    } else {
        // TODO errors: no memory
        return -1;
    }

    this->co_item = items;
    CO_SIZE(this) = newsize;
    this->allocated = new_allocated;

    return 0;
}

COObject *
COList_New(size_t size)
{
    COListObject *this;
    size_t nbytes;
    nbytes = size * sizeof(COObject *);

    this = xmalloc(sizeof(COListObject));
    COObject_Init(this, &COList_Type);
    if (size <= 0) {
        this->co_item = NULL;
    } else {
        this->co_item = (COObject **)xmalloc(nbytes);
        if (this->co_item == NULL) {
            // TODO errors
            return NULL;
        }
        memset(this->co_item, 0, nbytes);
    }
    CO_SIZE(this) = size;
    this->allocated = size;
    return (COObject *)this;
}

size_t
COList_Size(COObject *this)
{
    return CO_SIZE(this);
}

COObject *
COList_GetItem(COObject *this, size_t index)
{
    if (index < 0 || index >= CO_SIZE(this)) {
        // TODO errors
        return NULL;
    }
    return ((COListObject *)this)->co_item[index];
}

int
COList_SetItem(COObject *this, size_t index, COObject *item)
{
    COObject **p;
    COObject *olditem;
    if (index < 0 || index >= CO_SIZE(this)) {
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
COList_Insert(COObject *this, size_t index, COObject *item)
{
    size_t n = CO_SIZE(this);

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
    size_t i;
    for (i = n; i > index; i--)
        items[i + 1] = items[i];

    CO_XINCREF(item);
    items[index] = item;
    return 0;
}

int
COList_Append(COObject *this, COObject *item)
{
    return COList_Insert(this, CO_SIZE(this), item);
}

COObject *
COList_AsTuple(COObject *this)
{
    size_t n;
    COObject *co;
    COObject **p;
    COObject **q;

    n = CO_SIZE(this);
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
