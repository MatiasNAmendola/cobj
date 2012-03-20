#include "../co.h"

static COObject *
bytearray_repr(COByteArrayObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<bytearray>");
    return s;
}

COTypeObject COByteArray_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "bytearray",
    sizeof(COByteArrayObject),
    0,
    (reprfunc)bytearray_repr,   /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};

char *
COByteArray_AsString(COObject *co)
{
    return ((COByteArrayObject *)co)->co_bytes;
}

COObject *
COByteArray_FromString(const char *bytes)
{
    return COByteArray_FromStringN(bytes, strlen(bytes));
}

COObject *
COByteArray_FromStringN(const char *bytes, size_t len)
{
    COByteArrayObject *new;

    if (len < 0) {
        // TODO errors
        return NULL;
    }

    new = xmalloc(sizeof(COByteArrayObject));
    if (new == NULL) {
        return NULL;
    }
    COVarObject_Init(new, &COByteArray_Type, len);

    if (len == 0) {
        new->co_bytes = NULL;
        new->co_alloc = 0;
    } else {
        new->co_bytes = xmalloc(len + 1);
        if (new->co_bytes == NULL) {
            // TODO errors
            return NULL;
        }
        if (bytes != NULL)
            memcpy(new->co_bytes, bytes, len);
        new->co_bytes[len] = '\0';  /* trailing null byte */
        new->co_alloc = len + 1;
    }

    return (COObject *)new;
}

void
COByteArray_Concat(COObject **pv, COObject *bytes)
{
}

int
COByteArray_Resize(COObject *this, size_t size)
{
    size_t alloc = ((COByteArrayObject *)this)->co_alloc;
    char *bytes;

    if (size == CO_SIZE(this)) {
        return 0;
    }

    if (size < alloc / 2) {
        /* Major downsize; resize down to exact size */
        alloc = size + 1;
    } else if (size < alloc) {
        /* Within allocated size; quick exit */
        CO_SIZE(this) = size;
        ((COByteArrayObject *)this)->co_bytes[size] = '\0';
        return 0;
    } else {
        /* Major upsize: resize up to exact size */
        alloc = size + 1;
    }

    bytes = (char *)xrealloc(((COByteArrayObject *)this)->co_bytes, alloc);
    if (bytes == NULL) {
        // TODO errors
        return -1;
    }

    ((COByteArrayObject *)this)->co_bytes = bytes;
    CO_SIZE(this) = size;
    ((COByteArrayObject *)this)->co_alloc = alloc;
    ((COByteArrayObject *)this)->co_bytes[size] = '\0';

    return 0;
}
