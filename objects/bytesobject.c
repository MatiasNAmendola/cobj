#include "../co.h"

static COObject *
bytes_repr(COBytesObject *this)
{
    static const char *hexdigits = "0123456789abcdef";
    const char *quote_prefix = "b";
    const char *quote_postfix = "";
    size_t len = CO_SIZE(this);
    /* LEN_MIN = strlen(quote_prefix) + 2 + strlen(quote_postfix) */
#define LEN_MIN 3
    if (len > (SIZE_MAX - LEN_MIN) / 4) {
        // TODO "bytes object is too large to make repr"
        return NULL;
    }
    size_t newsize;
    newsize = LEN_MIN + 4 * len;
    COObject *s = COStr_FromStringN(NULL, newsize);
    if (s == NULL) {
        return NULL;
    } else {
        size_t i;
        char *p = COStr_AsString(s);
        char c;
        char quote = '\'';

        while (*quote_prefix)
            *p++ = *quote_prefix++;

        *p++ = quote;
        for (i = 0; i < len; i++) {
            c = this->co_bytes[i];
            *p++ = '\\';
            *p++ = 'x';
            *p++ = hexdigits[(c & 0xf0) >> 4];
            *p++ = hexdigits[c & 0xf];
        }
        *p++ = quote;

        while (*quote_postfix)
            *p++ = *quote_postfix++;

        *p = '\0';
        return s;
    }
}

COTypeObject COBytes_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "bytes",
    sizeof(COBytesObject),
    0,
    (reprfunc)bytes_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};

char *
COBytes_AsString(COObject *co)
{
    return ((COBytesObject *)co)->co_bytes;
}

COObject *
COBytes_FromString(const char *bytes)
{
    return COBytes_FromStringN(bytes, strlen(bytes));
}

COObject *
COBytes_FromStringN(const char *bytes, size_t len)
{
    COBytesObject *new;

    new = xmalloc(sizeof(COBytesObject));
    if (new == NULL) {
        return NULL;
    }
    COVarObject_Init(new, &COBytes_Type, len);

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
        new->co_bytes[len] = '\0';      /* trailing null byte */
        new->co_alloc = len + 1;
    }

    return (COObject *)new;
}

void
COBytes_Concat(COObject **pv, COObject *bytes)
{
}

int
COBytes_Resize(COObject *this, size_t size)
{
    size_t alloc = ((COBytesObject *)this)->co_alloc;
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
        ((COBytesObject *)this)->co_bytes[size] = '\0';
        return 0;
    } else {
        /* Major upsize: resize up to exact size */
        alloc = size + 1;
    }

    bytes = (char *)xrealloc(((COBytesObject *)this)->co_bytes, alloc);
    if (bytes == NULL) {
        // TODO errors
        return -1;
    }

    ((COBytesObject *)this)->co_bytes = bytes;
    CO_SIZE(this) = size;
    ((COBytesObject *)this)->co_alloc = alloc;
    ((COBytesObject *)this)->co_bytes[size] = '\0';

    return 0;
}

size_t
COBytes_Size(COObject *co)
{
    return CO_SIZE(co);
}
