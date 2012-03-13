#include "../co.h"

static COStrObject *null_str = NULL;

/*
 * It gives the base size of str object; any memory allocation for a string of
 * length n should request COStr_BASESIZE + (n + 1) bytes (with extra
 * '\0').
 *
 * Instead of sizeof(COStrObject), it saves bytes per string allocation on a
 * typical system, you can compare this with sizeof(COStrObject) + n bytes.
 */
#define COStr_BASESIZE    offsetof(COStrObject, co_sval)
 
static COObject *
str_repr(COObject *this)
{
    return this;
}

COTypeObject COStr_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "str",
    sizeof(COStrObject),
    0,
    str_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

char *
COStr_AsString(COObject *co)
{
    return ((COStrObject *)co)->co_sval;
}

/*
 * `s` points to a null-terminated string.
 */
COObject *
COStr_FromString(const char *s)
{
    COStrObject *str;
    size_t len = strlen(s);
    str = xmalloc(COStr_BASESIZE + len + 1);
    CO_INIT(str, &COStr_Type);
    str->co_len = len;
    memcpy(str->co_sval, s, len + 1); // with last '\0'
    return (COObject *)str;
}

/*
 * `s` points to either NULL or else a string containing at least `len` bytes. `s`
 * do not have to be null-terminated.
 */
COObject *
COStr_FromStingN(const char *s, size_t len)
{
    COStrObject *str;

    if (len == 0 && (str = null_str) != NULL) {
        return (COObject *)str;
    }

    str = xmalloc(COStr_BASESIZE + len + 1);
    CO_INIT(str, &COStr_Type);
    str->co_len = len;
    if (s != NULL) {
        memcpy(str->co_sval, s, len);
    }
    str->co_sval[str->co_len] = '\0';

    /* share short strings */
    if (len == 0) {
        null_str = str;
    }

    return (COObject *)str;
}

COObject *
COStr_FromFormat(const char *fmt, ...)
{
    COStrObject *str;

    va_list params;

    va_start(params, fmt);

    /* step 1: figure out how large a buffer we need */
    const char *f;
    size_t n = 0;
    va_list count;
    char *s;
    va_copy(count, params);
    for (f = fmt; *f; f++) {
        if (*f == '%') {
            const char *p = f;

            while (*++f && *f != '%' && !isalpha(*f));

            /* skip the 'l' or 'z' in {%ld, %zd, %lu, %zu] since
             * they don't affect the amount of space we reserve.
             */
            if (*f == 'l') {
                if (f[1] == 'd' || f[1] == 'u') {
                    ++f;
                }
            } else if (*f == 'z' && (f[1] == 'd' || f[1] == 'u')) {
                ++f;
            }

            switch (*f) {
            case 'c':
                (void)va_arg(count, int);
                /* fall through... */
            case '%':
                n++;
                break;
            case 'd': case 'u': case 'i': case 'x':
                (void)va_arg(count, int);
                n += 20;
                break;
            case 's':
                s = va_arg(count, char*);
                n += strlen(s);
                break;
            case 'p':
                (void)va_arg(count, int);
                /* maximum 64-bit pointer representation:
                 * 0xffffffffffffffff
                 * so 18 characters is enough.
                 */
                n += 18;
                break;
            default:
                /* if we stumble upon an unknown formatting code, copy the rest
                 * of the format string to the output string.
                 */
                n += strlen(p);
                goto step2;
            }
        } else {
            n++;
        }
    }

step2:
    /* step 2: fill the buffer */
    str = (COStrObject *)COStr_FromStingN(NULL, n+1);
    if (!str)
        return NULL;

    vsnprintf(str->co_sval, n + 1, fmt, params);

    va_end(params);

    return (COObject *)str;
}
