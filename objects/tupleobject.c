#include "../co.h"

static COObject *
tuple_repr(COTupleObject *this)
{
    size_t i = ((COTupleObject *)this)->co_size;
    if (i == 0) {
        return COStr_FromString("()");
    }
    COObject *s;
    s = COStr_FromString("(");
    for (i = 0; i < ((COTupleObject *)this)->co_size; i++) {
        COObject *co = COList_GetItem((COObject *)this, i);
        if (i != 0)
            COStr_Concat(&s, COStr_FromString(", "));
        COStr_Concat(&s, CO_TYPE(co)->tp_repr(co));
    }
    COStr_Concat(&s, COStr_FromString(")"));
    return s;
}

COTypeObject COTuple_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "tuple",
    sizeof(COTupleObject),
    0,
    (reprfunc)tuple_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_richcompare */
    0,                          /* tp_int_interface */
};

static COObject *
tuple_slice(COTupleObject *this, int ilow, int ihigh)
{
    COObject **src, **dest;
    int i, len;
    COTupleObject *co;
    if (ilow < 0)
        ilow = 0;
    else if (ilow > ((COTupleObject *)this)->co_size)
        ilow = ((COTupleObject *)this)->co_size;
    if (ihigh < ilow)
        ihigh = ilow;
    else if (ihigh > ((COTupleObject *)this)->co_size)
        ihigh = ((COTupleObject *)this)->co_size;
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
COTuple_New(size_t size)
{
    COTupleObject *this;
    size_t nbytes;
    nbytes = size * sizeof(COObject *);

    this = COObject_New(COTupleObject, &COTuple_Type);
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
    ((COTupleObject *)this)->co_size = size;
    return (COObject *)this;
}

size_t
COTuple_Size(COObject *this)
{
    return ((COTupleObject *)this)->co_size;
}

COObject *
COTuple_GetItem(COObject *this, size_t index)
{
    if (index >= ((COTupleObject *)this)->co_size) {
        // TODO errors
        return NULL;
    }
    return ((COTupleObject *)this)->co_item[index];
}

int
COTuple_SetItem(COObject *this, size_t index, COObject *item)
{
    COObject **p;
    COObject *olditem;
    if (index >= ((COTupleObject *)this)->co_size) {
        // TODO errors
        return -1;
    }
    p = ((COTupleObject *)this)->co_item + index;
    olditem = *p;
    *p = item;
    CO_XDECREF(olditem);
    return 0;
}

COObject *
COTuple_GetSlice(COObject *this, int ilow, int ihigh)
{
    return tuple_slice((COTupleObject *)this, ilow, ihigh);
}
