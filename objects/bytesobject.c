#include "../cobj.h"

static COObject *
bytes_repr(COBytesObject *this)
{
    static const char *hexdigits = "0123456789abcdef";
    const char *quote_prefix = "b";
    const char *quote_postfix = "";
    ssize_t len = this->co_size;
    /* LEN_MIN = strlen(quote_prefix) + 2 + strlen(quote_postfix) */
#define LEN_MIN 3
    if (len > (SIZE_MAX - LEN_MIN) / 4) {
        // TODO "bytes object is too large to make repr"
        return NULL;
    }
    ssize_t newsize;
    newsize = LEN_MIN + 4 * len;
    COObject *s = COStr_FromStringN(NULL, newsize);
    if (s == NULL) {
        return NULL;
    } else {
        ssize_t i;
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

static long
bytes_hash(COBytesObject *this)
{
    ssize_t len;
    unsigned char *p;
    long x;
    len = this->co_size;
    p = (unsigned char *)this->co_bytes;
    x = *p << 7;
    while (--len >= 0)
        x = (1000003 * x) ^ *p++;
    x ^= this->co_size;
    return x;
}

static void
bytes_dealloc(COBytesObject *this)
{
    if (this->co_bytes) {
        COObject_Mem_FREE(this->co_bytes);
    }
    COObject_Mem_FREE(this);
}

COTypeObject COBytes_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "bytes",
    sizeof(COBytesObject),
    0,
    0,
    0,                          /* tp_alloc */
    0,                          /* tp_init */
    (deallocfunc)bytes_dealloc, /* tp_dealloc */
    (reprfunc)bytes_repr,       /* tp_repr */
    0,                          /* tp_print */
    (hashfunc)bytes_hash,       /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface */
    0,                          /* tp_sequence_interface */
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
COBytes_FromStringN(const char *bytes, ssize_t len)
{
    COBytesObject *new;

    new = COObject_NEW(COBytesObject, &COBytes_Type);
    if (new == NULL) {
        return NULL;
    }

    if (len == 0) {
        new->co_bytes = NULL;
        new->co_alloc = 0;
    } else {
        new->co_bytes = COObject_Mem_MALLOC(len + 1);
        if (new->co_bytes == NULL) {
            // TODO errors
            return NULL;
        }
        if (bytes != NULL)
            memcpy(new->co_bytes, bytes, len);
        new->co_bytes[len] = '\0';      /* trailing null byte */
        new->co_alloc = len + 1;
    }

    new->co_size = len;
    return (COObject *)new;
}

void
COBytes_Concat(COObject **pv, COObject *bytes)
{
}

int
COBytes_Resize(COObject *this, ssize_t size)
{
    ssize_t alloc = ((COBytesObject *)this)->co_alloc;
    char *bytes;

    if (size == ((COBytesObject *)this)->co_size) {
        return 0;
    }

    if (size < alloc / 2) {
        /* Major downsize; resize down to exact size */
        alloc = size + 1;
    } else if (size < alloc) {
        /* Within allocated size; quick exit */
        ((COBytesObject *)this)->co_size = size;
        ((COBytesObject *)this)->co_bytes[size] = '\0';
        return 0;
    } else {
        /* Major upsize: resize up to exact size */
        alloc = size + 1;
    }

    bytes =
        (char *)COObject_Mem_REALLOC(((COBytesObject *)this)->co_bytes, alloc);
    if (bytes == NULL) {
        // TODO errors
        return -1;
    }

    ((COBytesObject *)this)->co_bytes = bytes;
    ((COBytesObject *)this)->co_size = size;
    ((COBytesObject *)this)->co_alloc = alloc;
    ((COBytesObject *)this)->co_bytes[size] = '\0';

    return 0;
}

ssize_t
COBytes_Size(COObject *this)
{
    return ((COBytesObject *)this)->co_size;
}
