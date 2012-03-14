#include "../co.h"

static COObject *
tuple_repr(COTupleObject *this)
{
    return COStr_FromString("<tuple>");
}

COTypeObject COTuple_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "tuple",
    sizeof(COTupleObject),
    0,
    (reprfunc)tuple_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};

COObject *
COTuple_New(size_t size)
{
    COTupleObject *this;
    size_t nbytes;
    nbytes = size * sizeof(COObject *);

    this = xmalloc(sizeof(COTupleObject));
    CO_INIT(this, &COTuple_Type);
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
COTuple_Size(COObject *this)
{
}

COObject *
COTuple_GetItem(COObject *this, size_t index)
{
}

int
COTuple_SetItem(COObject *this, size_t index, COObject *item)
{
}

int
COTuple_Resize(COObject **this, size_t newsize)
{
}
