#include "../co.h"

static COStrObject *null_str = NULL;
static COStrObject *char_strs[UCHAR_MAX + 1];

#ifdef STR_COUNT
ssize_t null_str_num = 0;
ssize_t char_str_num = 0;
#endif

/*
 * It gives the base size of str object; any memory allocation for a string of
 * length n should request COStr_BASESIZE + n bytes.
 *
 * Instead of sizeof(COStrObject), it saves bytes per string allocation on a
 * typical system, you can compare this with sizeof(COStrObject) + n bytes.
 */
#define COStr_BASESIZE    (offsetof(COStrObject, co_sval) + 1)

static COObject *
str_repr(COObject *this)
{
    CO_INCREF(this);
    return this;
}

static COStrObject *
str_concat(COStrObject *this, COStrObject *s)
{
    ssize_t size;
    COStrObject *co;

    size = CO_SIZE(this) + CO_SIZE(s);

    co = COVarObject_NEW(COStrObject, &COStr_Type, size);
    if (co == NULL) {
        // TODO errors
        return NULL;
    }

    memcpy(co->co_sval, this->co_sval, CO_SIZE(this));
    memcpy(co->co_sval + CO_SIZE(this), s->co_sval, CO_SIZE(s));
    co->co_shash = -1;
    co->co_sval[size] = '\0';
    return co;
}

static COObject *
str_join(COStrObject *this, COObject *list)
{
    char *sep = COStr_AS_STRING(this);
    const ssize_t seplen = CO_SIZE(this);
    ssize_t seqlen = 0;
    char *p;
    COObject *result;

    seqlen = COList_GET_SIZE(list);

    if (seqlen == 0) {
        return COStr_FromString("");
    }

    if (seqlen == 1) {
        CO_INCREF(COList_GET_ITEM(list, 0));
        return COList_GET_ITEM(list, 0);
    }

    int i;
    ssize_t sz = 0;
    COObject *item;
    for (i = 0; i < seqlen; i++) {
        const size_t old_sz = sz;
        item = COList_GET_ITEM(list, i);
        if (!COStr_Check(item)) {
            COErr_BadInternalCall();
            return NULL;
        }
        sz += CO_SIZE(item);
        if (i != 0)
            sz += seplen;
        if (sz < old_sz || sz > SSIZE_MAX) {
            COErr_SetString(COException_OverflowError,
                            "join() result is too long");
            return NULL;
        }
    }

    /* Allocate result space. */
    result = COStr_FromStringN(NULL, sz);
    if (!result) {
        return NULL;
    }

    /* Catenate everything. */
    p = COStr_AS_STRING(result);
    for (i = 0; i < seqlen; i++) {
        size_t n;
        item = COList_GET_ITEM(list, i);
        n = CO_SIZE(item);
        memcpy(p, COStr_AS_STRING(item), n);
        p += n;
        if (i < seqlen - 1) {
            memcpy(p, sep, seplen);
            p += seplen;
        }
    }

    return result;
}

/*
 * The following function breaks the notion that strings are immutable:
 *  - it changes the size of string
 *  - it can only be used if there is only one reference
 */
int
str_resize(COStrObject **pv, ssize_t newsize)
{
    COStrObject *v;
    v = *pv;
    if (CO_REFCNT(v) != 1) {
        // TODO errors
        return -1;
    }

    *pv = (COStrObject *)COMem_REALLOC((char *)v, COStr_BASESIZE + newsize);
    if (*pv == NULL) {
        // TODO errors
        return -1;
    }
    CO_SIZE((*pv)) = newsize;
    (*pv)->co_sval[newsize] = '\0';
    return 0;
}

/*
 * Borrowed From PHP/Zend Engine
 *
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + str[i]''. This is one of the best
 * known hash functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an explanation: if one experimentally tests all
 * multipliers between 1 and 256 (as RSE did now) one detects that even
 * numbers are not useable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%.
 *
 * If one compares the Chi^2 values of the variants, the number 33 not
 * even has the best value. But the number 33 and a few other equally
 * good numbers like 17, 31, 63, 127 and 129 have nevertheless a great
 * advantage to the remaining numbers in the large set of possible
 * multipliers: their multiply operation can be replaced by a faster
 * operation based on just one shift plus either a single addition
 * or subtraction operation. And because a hash function has to both
 * distribute good _and_ has to be very fast to compute, those few
 * numbers should be preferred and seems to be the reason why Daniel J.
 * Bernstein also preferred it.
 *
 *
 *                  -- Ralf S. Engelschall <rse@engelschall.com>
 */
static unsigned long
str_hash(COStrObject *this)
{
    if (this->co_shash != -1)
        return this->co_shash;

    const char *arKey = COStr_AsString((COObject *)this);
    unsigned int nKeyLen = CO_SIZE(this);
    unsigned long hash = 5381;

    /* variant with the hash unrolled eight times */
    for (; nKeyLen >= 8; nKeyLen -= 8) {
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
    }
    switch (nKeyLen) {
    case 7:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 6:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 5:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 4:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 3:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 2:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 1:
        hash = ((hash << 5) + hash) + *arKey++;
        break;
    case 0:
        break;
    default:
        break;
    }
    this->co_shash = hash;
    return hash;
}

static COObject *
str_compare(COStrObject *this, COStrObject *that, int op)
{
    COObject *x;
    int c;
    ssize_t len_this, len_that;
    ssize_t len_min;

    if (this == that) {
        switch (op) {
        case Cmp_EQ:
        case Cmp_LE:
        case Cmp_GE:
            x = CO_True;
            goto end;
        case Cmp_NE:
        case Cmp_LT:
        case Cmp_GT:
            x = CO_False;
            goto end;
        }
    }

    if (op == Cmp_EQ) {
        if (CO_SIZE(this) == CO_SIZE(that)
            && this->co_sval[0] == that->co_sval[0]
            && memcmp(this->co_sval, that->co_sval, CO_SIZE(this)) == 0) {
            x = CO_True;
        } else {
            x = CO_False;
        }
        goto end;
    }

    len_this = CO_SIZE(this);
    len_that = CO_SIZE(that);
    len_min = (len_this < len_that) ? len_this : len_that;
    if (len_min > 0) {
        c = CHAR_MASK(*this->co_sval) - CHAR_MASK(*that->co_sval);
        if (c == 0)
            c = memcmp(this->co_sval, that->co_sval, len_min);
    } else {
        c = 0;
    }
    if (c == 0) {
        c = (len_this < len_that) ? -1 : (len_this > len_that) ? 1 : 0;
    }

    switch (op) {
    case Cmp_LT:
        c = c < 0;
        break;
    case Cmp_LE:
        c = c <= 0;
        break;
    case Cmp_EQ:
        assert(0);
        break;                  /* unreachable */
    case Cmp_NE:
        c = c != 0;
        break;
    case Cmp_GT:
        c = c > 0;
        break;
    case Cmp_GE:
        c = c >= 0;
        break;
    default:
        assert(0);
    }
    x = c ? CO_True : CO_False;
end:
    CO_INCREF(x);
    return x;
}

static void
str_dealloc(COStrObject *this)
{
    COMem_FREE(this);
}

COTypeObject COStr_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "str",
    COStr_BASESIZE,
    sizeof(char),
    0,
    (deallocfunc)str_dealloc,   /* tp_dealloc */
    (reprfunc)str_repr,         /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    (hashfunc)str_hash,         /* tp_hash */
    (comparefunc)str_compare,   /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
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
    return COStr_FromStringN(s, strlen(s));
}

/*
 * `s` points to either NULL or else a string containing at least `len` bytes. `s`
 * do not have to be null-terminated.
 */
COObject *
COStr_FromStringN(const char *s, ssize_t len)
{
    COStrObject *str;

    if (len == 0 && (str = null_str) != NULL) {
#ifdef STR_COUNT
        null_str_num++;
#endif
        CO_INCREF(str);
        return (COObject *)str;
    }
    if (len == 1 && s != NULL && (str = char_strs[*s & UCHAR_MAX]) != NULL) {
#ifdef STR_COUNT
        char_str_num++;
#endif
        CO_INCREF(str);
        return (COObject *)str;
    }

    str = COVarObject_NEW(COStrObject, &COStr_Type, len);
    str->co_shash = -1;
    if (s != NULL) {
        memcpy(str->co_sval, s, len);
    }
    str->co_sval[CO_SIZE(str)] = '\0';

    /* share short strings */
    if (len == 0) {
        null_str = str;
    } else if (len == 1 && s != NULL) {
        char_strs[*s & UCHAR_MAX] = str;
    }

    return (COObject *)str;
}

COObject *
COStr_FromFormat(const char *fmt, ...)
{
    COObject *str;
    va_list params;

    va_start(params, fmt);

    str = COStr_FromFormatV(fmt, params);

    va_end(params);

    return str;
}

COObject *
COStr_FromFormatV(const char *fmt, va_list params)
{
    COStrObject *str;

    /* step 1: figure out how large a buffer we need */
    const char *f;
    ssize_t n = 0;
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
            case 'd':
            case 'u':
            case 'i':
            case 'x':
                (void)va_arg(count, int);
                n += 20;
                break;
            case 's':
                s = va_arg(count, char *);
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
    str = (COStrObject *)COStr_FromStringN(NULL, n);
    if (!str)
        return NULL;

    n = vsnprintf(str->co_sval, n + 1, fmt, params);    // with extra '\0'
    if (str_resize(&str, n)) {
        // TODO errors
        return NULL;
    }

    return (COObject *)str;
}

void
COStr_Concat(COObject **pv, COObject *s)
{
    if (*pv == NULL)
        return;

    if (s == NULL) {
        return;
    }

    COStrObject *co;
    co = str_concat(*(COStrObject **)pv, (COStrObject *)s);
    CO_DECREF(*pv);
    *pv = (COObject *)co;
}

void
COStr_ConcatAndDel(COObject **pv, COObject *s)
{
    COStr_Concat(pv, s);
    CO_XDECREF(s);
}

COObject *
COStr_Join(COObject *sep, COObject *x)
{
    assert(sep != NULL && COStr_Check(sep));
    assert(x != NULL);
    return str_join((COStrObject *)sep, x);
}

COObject *
COStr_Repr(COObject *s, int smartquotes)
{
    // TODO
    return NULL;
}
