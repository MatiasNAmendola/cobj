#include "../co.h"

static COObject *
tuple_repr(COTupleObject *this)
{
    size_t i = CO_SIZE(this);
    if (i == 0) {
        return COStr_FromString("()");
    }
    COStrObject *s;
    s = COStr_FromString("(");
    for (i = 0; i < CO_SIZE(this); i++) {
        COObject *co = COList_GetItem(this, i);
        if (i != 0)
            COStr_Concat(&s, COStr_FromString(", "));
        COStr_Concat(&s, (COStrObject *)CO_TYPE(co)->tp_repr(co));
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
};

static COObject *
tuple_slice(COTupleObject *this, size_t ilow, size_t ihigh)
{
    COObject **src, **dest;
    size_t i, len;
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
COTuple_New(size_t size)
{
    COTupleObject *this;
    size_t nbytes;
    nbytes = size * sizeof(COObject *);

    this = xmalloc(sizeof(COTupleObject));
    COObject_Init(this, &COTuple_Type);
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
    return (COObject *)this;
}

size_t
COTuple_Size(COObject *this)
{
    return CO_SIZE(this);
}

COObject *
COTuple_GetItem(COObject *this, size_t index)
{
    if (index < 0 || index >= CO_SIZE(this)) {
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
    if (index < 0 || index >= CO_SIZE(this)) {
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
COTuple_GetSlice(COObject *this, size_t ilow, size_t ihigh)
{
    return tuple_slice((COTupleObject *)this, ilow, ihigh);
}
