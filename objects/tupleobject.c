#include "../co.h"

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
            COStr_Concat(&s, COStr_FromString(", "));
        COStr_Concat(&s, COObject_repr(co));
    }
    COStr_Concat(&s, COStr_FromString(")"));
    return s;
}

static long
tuple_hash(COTupleObject *this)
{
    long x, y;
    ssize_t len;
    COObject **p;
    x = 0x345678L;
    len = CO_SIZE(this);
    p = this->co_item;
    while (--len >= 0) {
        y = COObject_Hash(*p++);
        x = (x ^ y) * len;
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
    }
    COMem_FREE(this);
}

COTypeObject COTuple_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "tuple",
    sizeof(COTupleObject) - sizeof(COObject *),
    sizeof(COObject *),
    (deallocfunc)tuple_dealloc, /* tp_dealloc */
    (reprfunc)tuple_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    (hashfunc)tuple_hash,       /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
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

    this = COVarObject_New(COTupleObject, &COTuple_Type, size);
    for (int i = 0; i < size; i++) {
        this->co_item[i] = NULL;
    }
    /*memset(this->co_item, 0, size * sizeof(COObject *));*/
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
    if (index >= CO_SIZE(this)) {
        // TODO errors
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
