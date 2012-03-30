#include "../co.h"

#define MAX_LONG_DIGITS \
    ((SIZE_MAX - offsetof(COIntObject, co_digit))/sizeof(digit))
#define ABS(x) ((x) < 0 ? -(x) : (x))

/* Convert 0 or 1 size of COInt to an sdigit */
#define ONEDIGIT_VALUE(x) \
    (CO_SIZE(x) < 0 ? -(sdigit)(x)->co_digit[0] : \
     (CO_SIZE(x) == 0 ? (sdigit) 0 : (sdigit)(x)->co_digit[0]))

/*
 * Small integers are preallocated in this array so that they can be shared.
 *
 * [SMALL_NEG_INT, SMALL_POS_INT)
 */
#define SMALL_NEG_INT   10
#define SMALL_POS_INT   257
static COIntObject small_ints[SMALL_NEG_INT + SMALL_POS_INT];

#if SMALL_NEG_INT + SMALL_POS_INT > 0
#define CHECK_SMALL_INT(ival)   \
    do if (-SMALL_NEG_INT <= ival && ival < SMALL_POS_INT) {    \
        return (COObject *)(small_ints + ival + SMALL_NEG_INT); \
    } while (0);

static COIntObject *
maybe_small_int(COIntObject *o)
{
    if (o && ABS(CO_SIZE(o)) <= 1) {
        sdigit ival = ONEDIGIT_VALUE(o);
        if (-SMALL_NEG_INT <= ival && ival < SMALL_POS_INT) {
            CO_DECREF(o);
            return (COIntObject *)(small_ints + ival + SMALL_NEG_INT);
        }
    }
    return o;
}
#else
#define CHECK_SMALL_INT(ival)
#define maybe_small_int(val) (val)
#endif

/* Table of digit values for 8-bit string -> integer conversion.
 * '0' maps to 0, ..., '9' maps to 9.
 * 'a' and 'A' map to 10, ..., 'z' and 'Z' map to 35.
 * All other indices map to 37.
 * Note that when converting a base B string, a char c is a legitimate
 * base B digit if digit_values[c] < B.
 */
unsigned char digit_values[256] = {
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 37, 37, 37, 37, 37, 37,
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
};

COIntObject *
_COIntObject_New(ssize_t size)
{
    COIntObject *result;
    if (size > MAX_LONG_DIGITS) {
        COErr_SetString(COException_SystemError, "too many digits in integer");
        return NULL;
    }
    result = COVarObject_New(COIntObject, &COInt_Type, size);
    if (!result) {
        // TODO errors
        return NULL;
    }
    return result;
}

/*
 * Normalize a int object, removing leading zeros. Due to algorithm, there maybe
 * leading zeors.
 * Doesn't attempt to free the storage, which will change object address.
 */
static COIntObject *
int_normalize(COIntObject *o)
{
    ssize_t j = ABS(CO_SIZE(o));
    ssize_t i = j;

    while (i > 0 && o->co_digit[i - 1] == 0)
        --i;
    if (i != j)
        CO_SIZE(o) = (CO_SIZE(o) < 0) ? -i : i;
    return o;
}

/*
 * *str points to the first digit in a string of base `base` digits. Base is pow
 * of 2 (2, 4, 8, 16, or 32).
 */
static COIntObject *
int_from_binary_base(char **str, int base)
{
    char *p = *str;
    char *start = p;
    int bits_per_char;
    ssize_t n;
    COIntObject *o;
    twodigits accum;
    int bits_in_accum;
    digit *pdigit;

    assert(base >= 2 && base <= 32 && (base & (base - 1)) == 0);
    n = base;
    for (bits_per_char = -1; n; ++bits_per_char)
        n >>= 1;

    while (digit_values[CHAR_MASK(*p)] < base)
        ++p;
    *str = p;

    /* n <- number of digits need, = ceil(((p - start) * bits_per_char) / COInt_SHIFT). */
    n = (p - start) * bits_per_char + COInt_SHIFT - 1;
    if (n / bits_per_char < p - start) {
        COErr_SetString(COException_ValueError,
                        "int string too large to convert");
        return NULL;
    }
    n = n / COInt_SHIFT;

    o = _COIntObject_New(n);
    if (!o) {
        return NULL;
    }

    /* Read string from right, and fill in digits from left.
     * From least to most significant in both.
     */
    accum = 0;
    bits_in_accum = 0;
    pdigit = o->co_digit;

    while (--p >= start) {
        int k = digit_values[CHAR_MASK(*p)];
        accum |= (twodigits)k << bits_in_accum;
        bits_in_accum += bits_per_char;
        if (bits_in_accum >= COInt_SHIFT) {
            *pdigit++ = (digit)(accum & COInt_MASK);
            assert(pdigit - o->co_digit <= n);
            accum >>= COInt_SHIFT;
            bits_in_accum -= COInt_SHIFT;
            assert(bits_in_accum < COInt_SHIFT);
        }
    }
    if (bits_in_accum) {
        assert(bits_in_accum <= COInt_SHIFT);
        *pdigit++ = (digit)accum;
        assert(pdigit - o->co_digit <= n);
    }

    return o;
}

/*
 * Convert a integer to a base 10 string.
 */
static COObject *
int_repr(COIntObject *this)
{
    COIntObject *scratch;
    COObject *str;
    digit *pout, *pin, rem, tenpow;
    ssize_t size, size_a, i, j;
    int negative;

    if (this == NULL) {
        return NULL;
    }
    size_a = ABS(CO_SIZE(this));
    negative = CO_SIZE(this) < 0;

    /* TODO check overflow */
    size = 1 + size_a * COInt_SHIFT / (3 * COInt_DECIMAL_SHIFT);
    scratch = _COIntObject_New(size);
    if (scratch == NULL)
        return NULL;

    /*
     * convert array of base COInt_BASE digits in pin to an array of base
     * COInt_DECIMAL_BASE digits in pout, following Kunth (TAOCP, Volume 2 (3rd
     * edn), section 4.4, Method 1b).
     */
    pin = this->co_digit;
    pout = scratch->co_digit;
    size = 0;
    for (i = size_a; --i >= 0;) {
        digit hi = pin[i];
        for (j = 0; j < size; j++) {
            twodigits z = (twodigits)pout[j] << COInt_SHIFT | hi;
            hi = (digit)(z / COInt_DECIMAL_BASE);
            pout[j] = (digit)(z - (twodigits)hi * COInt_DECIMAL_BASE);
        }
        while (hi) {
            pout[size++] = hi % COInt_DECIMAL_BASE;
            hi /= COInt_DECIMAL_BASE;
        }
    }

    /* pout should have at least one digit, so that the case when this = 0
     * works correctly */
    if (size == 0)
        pout[size++] = 0;

    /* calculate exact length of output string, and allocate */
    ssize_t strlen = negative + 1 + (size - 1) * COInt_DECIMAL_SHIFT;
    tenpow = 10;
    rem = pout[size - 1];
    while (rem >= tenpow) {
        tenpow *= 10;
        strlen++;
    }
    str = COStr_FromStringN(NULL, strlen);
    if (!str) {
        CO_XDECREF(scratch);
        return NULL;
    }

    /* fill the string right-to-left */
    char *p = COStr_AsString(str) + strlen;
    *p = '\0';

    /* pout[0] through pout[size - 2] contribute exactly COInt_DECIMAL_SHIFT
     * digits each */
    for (i = 0; i < size - 1; i++) {
        rem = pout[i];
        for (j = 0; j < COInt_DECIMAL_SHIFT; j++) {
            *--p = '0' + rem % 10;
            rem /= 10;
        }
    }

    /* pout[size - 1]: always produce at least one decimal digit */
    rem = pout[i];
    do {
        *--p = '0' + rem % 10;
        rem /= 10;
    } while (rem != 0);

    /* and sign */
    if (negative)
        *--p = '-';

    /* check we've counted correctly */
    assert(p == COStr_AsString(str));
    CO_DECREF(scratch);
    return str;
}

/*
 * Subtract the absolute values of two integers.
 */
static COIntObject *
x_sub(COIntObject *a, COIntObject *b)
{
    ssize_t size_a = ABS(CO_SIZE(a));
    ssize_t size_b = ABS(CO_SIZE(b));
    COIntObject *o;
    ssize_t i;
    int sign = 1;

    /* Ensure a is larger than b */
    if (size_a < size_b) {
        // if exchanged, negative sign
        sign = -1;
        {
            COIntObject *tmp = a;
            a = b;
            b = tmp;
            size_t size_tmp = size_a;
            size_a = size_b;
            size_b = size_tmp;
        }
    } else if (size_a == size_b) {
        // Find highest digit whare a and b differ.
        i = size_a;
        while (--i >= 0 && a->co_digit[i] == b->co_digit[i]);
        if (i < 0)
            return (COIntObject *)COInt_FromLong(0);
        if (a->co_digit[i] < b->co_digit[i]) {
            sign = -1;
            {
                COIntObject *tmp = a;
                a = b;
                b = tmp;
            }
        }
        size_a = size_b = i + 1;
    }
    o = _COIntObject_New(size_a);
    if (!o)
        return NULL;

    digit borrow = 0;
    for (i = 0; i < size_b; i++) {
        borrow = a->co_digit[i] - b->co_digit[i] - borrow;
        o->co_digit[i] = borrow & COInt_MASK;
        borrow >>= COInt_SHIFT;
        borrow &= 1;
    }
    for (; i < size_a; i++) {
        borrow = a->co_digit[i] - borrow;
        o->co_digit[i] = borrow & COInt_MASK;
        borrow >>= COInt_SHIFT;
        borrow &= 1;
    }
    assert(borrow == 0);
    if (sign < 0)
        CO_SIZE(o) = -(CO_SIZE(o));
    return o;
}

/*
 * Add the absolute values of two integers.
 */
static COIntObject *
x_add(COIntObject *a, COIntObject *b)
{
    ssize_t size_a = ABS(CO_SIZE(a));
    ssize_t size_b = ABS(CO_SIZE(b));
    COIntObject *o;
    ssize_t i;

    /* Ensure a is larger than b */
    if (size_a < size_b) {
        {
            COIntObject *tmp = a;
            a = b;
            b = tmp;
            size_t size_tmp = size_a;
            size_a = size_b;
            size_b = size_tmp;
        }
    }
    o = _COIntObject_New(size_a + 1);
    if (!o)
        return NULL;

    digit carry = 0;
    for (i = 0; i < size_b; ++i) {
        carry += a->co_digit[i] + b->co_digit[i];
        o->co_digit[i] = carry & COInt_MASK;
        carry >>= COInt_SHIFT;
    }
    for (; i < size_a; ++i) {
        carry += a->co_digit[i];
        o->co_digit[i] = carry & COInt_MASK;
        carry >>= COInt_SHIFT;
    }
    o->co_digit[i] = carry;
    return int_normalize(o);
}

static COIntObject *
int_add(COIntObject *a, COIntObject *b)
{
    COIntObject *o;
    if (ABS(CO_SIZE(a)) <= 1 && ABS(CO_SIZE(b)) <= 1) {
        return (COIntObject *)COInt_FromLong(ONEDIGIT_VALUE(a) +
                                             ONEDIGIT_VALUE(b));
    }

    if (CO_SIZE(a) < 0) {
        if (CO_SIZE(b) < 0) {
            o = x_add(a, b);
            if (o != NULL && CO_SIZE(o) != 0)
                CO_SIZE(o) = -(CO_SIZE(o));
        } else {
            o = x_sub(b, a);
        }
    } else {
        if (CO_SIZE(b) < 0) {
            o = x_sub(a, b);
        } else {
            o = x_add(a, b);
        }
    }
    return int_normalize(o);
}

static COIntInterface int_interface = {
    (binaryfunc)int_add,
};

static long
int_hash(COIntObject *o)
{
    unsigned long x;
    int i;
    int sign;
    i = CO_SIZE(o);
    switch (i) {
    case -1:
        return o->co_digit[0] == 1 ? -2 : -(sdigit)o->co_digit[0];
    case 0:
        return 0;
    case 1:
        return o->co_digit[0];
    }
    sign = 1;
    x = 0;
    if (i < 0) {
        sign = -1;
        i = -(i);
    }
    return x;
}

COTypeObject COInt_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "int",
    offsetof(COIntObject, co_digit),
    sizeof(digit),
    (reprfunc)int_repr,         /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    (hashfunc)int_hash,         /* tp_hash */
    &int_interface,             /* tp_int_interface */
};

int
COInt_Init(void)
{
    int ival, size;
    COIntObject *o = small_ints;
    for (ival = -SMALL_NEG_INT; ival < SMALL_POS_INT; ival++, o++) {
        size = (ival < 0) ? -1 : ((ival == 0) ? 0 : 1);
        (void)COVarObject_Init(o, &COInt_Type, size);
        o->co_digit[0] = abs(ival);
    }
    return 0;
}

long
COInt_AsLong(COObject *co)
{
    return (long)((COIntObject *)co)->co_digit[0];
}

COObject *
COInt_FromString(char *s, char **pend, int base)
{
    COIntObject *o;
    int sign = 1;
    char *start;
    int error_if_nonzero = 0;

    if (base != 0 && (base < 2 || base > 36)) {
        COErr_SetString(COException_ValueError, "base must be >= 2 and <= 36");
        return NULL;
    }

    while (*s != '\0' and isspace(*s))
        s++;

    if (*s == '+') {
        ++s;
    } else if (*s == '-') {
        ++s;
        sign = -1;
    }

    if (base == 0) {
        // auto-detect
        if (s[0] != '0')
            base = 10;
        else if (s[1] == 'x' || s[1] == 'X')
            base = 16;
        else if (s[1] == 'o' || s[1] == 'O')
            base = 8;
        else if (s[1] == 'b' || s[1] == 'B')
            base = 2;
        else {
            /* it should be zero */
            error_if_nonzero = 1;
            base = 10;
        }
    }

    if (s[0] == '0' &&
        ((base == 16 && (s[1] == 'x' || s[1] == 'X')) ||
         (base == 8 && (s[1] == 'o' || s[1] == 'O')) ||
         (base == 2 && (s[1] == 'b' || s[1] == 'B')))) {
        s += 2;
    }

    start = s;

    if ((base & (base - 1)) == 0) {
        o = int_from_binary_base(&s, base);
    } else {
        /* Other bases use the simple quadratic-time algorithm below.
         *
         * The largest integer that can be expressed in N base-B digits is
         * B*N-1. Consequently, if we have an N-digit input in base B, the
         * worest number of digits needed to hold it is the smallest integer n
         * s.t. (BASE is COInt_BASE)
         *
         *  BASE**n - 1 >= B**N - 1 [adding 1 to both sides]
         *  BASE**n >= B**N         [taking logs to base BASE]
         *  n >= log(B**N)/log(BASE)  = N * log(B) / log(BASE)
         *
         *  The static array log_base_BASE[base] == log(base)/log(BASE) so we can
         *  compute this quickly. A int with that much space is reserved near
         *  the start, and the result is computed into it.
         *
         *  The input string is actually treated as being in base base**i (i.e.,
         *  i digits are processed at a time), where two more static arrays
         *  hold:
         *
         *      convwidth_base[base] = the largest integer i such that base**i
         *      <= BASE
         *      convmultmax_base[base] = base ** convwidth_base[base]
         * 
         * The first of these is the largest i such that i consecutive input
         * digits must fit in a single digit. The second is effectively the
         * input base we're really using.
         */
        ssize_t size;
        char *scan;

        // compute & cache
        static double log_base_BASE[37] = { 0.0e0, };
        static int convwidth_base[37] = { 0, };
        static twodigits convmultmax_base[37] = { 0, };

        if (log_base_BASE[base] == 0.0) {
            twodigits convmax = base;
            int i = 1;
            log_base_BASE[base] = (log((double)base) / log((double)COInt_BASE));

            for (;;) {
                twodigits next = convmax * base;
                if (next > COInt_BASE)
                    break;

                convmax = next;
                ++i;
            }

            convmultmax_base[base] = convmax;
            assert(i > 0);
            convwidth_base[base] = i;
        }

        /* Find length of the string of numeric characters. */
        scan = s;
        while (digit_values[CHAR_MASK(*scan)] < base)
            ++scan;

        /* Create a int object that can contain the largest possible integer
         * with this base and length.
         */
        size = (ssize_t) ((scan - s) * log_base_BASE[base]) + 1;
        o = _COIntObject_New(size);
        if (!o)
            return NULL;
        CO_SIZE(o) = 0;

        /* `convwidth` consecutive input digits are treated as a single digit in
         * base `convmultmax`.
         */
        int convwidth = convwidth_base[base];
        twodigits convmultmax = convmultmax_base[base];

        /* Loop */
        twodigits c;
        twodigits convmult;
        int i;
        digit *pdigit;
        digit *pdigit_end;
        while (s < scan) {
            /* grap up to convwidth digits from the input string */
            c = (digit)digit_values[CHAR_MASK(*s++)];
            for (i = 1; i < convwidth && s != scan; ++i, ++s) {
                c = (twodigits)(c * base + digit_values[CHAR_MASK(*s)]);
                assert(c < COInt_BASE);
            }

            convmult = convmultmax;

            /* Calculate the shift only if we couldn't get convwidth digits. */
            if (i != convwidth) {
                convmult = base;
                for (; i > 1; --i)
                    convmult *= base;
            }

            /* Multiply o by convmult */
            pdigit = o->co_digit;
            pdigit_end = pdigit + CO_SIZE(o);
            for (; pdigit < pdigit_end; ++pdigit) {
                c += (twodigits)*pdigit * convmult;
                *pdigit = (digit)(c & COInt_MASK);
                c >>= COInt_SHIFT;
            }

            /* Add c. */
            if (c) {
                if (CO_SIZE(o) < size) {
                    *pdigit = (digit)c;
                    ++CO_SIZE(o);
                } else {
                    // TODO
                    assert(0);
                }
            }
        }
    }
    if (!o)
        return NULL;
    if (error_if_nonzero) {
        base = 0;
        if (CO_SIZE(o) != 0) {
            goto on_error;
        }
    }
    if (s == start) {
        goto on_error;
    }
    if (sign < 0) {
        CO_SIZE(o) = -(CO_SIZE(o));
    }
    while (*s && isspace(CHAR_MASK(*s)))
        s++;
    if (*s != '\0')
        goto on_error;

    if (pend)
        *pend = s;

    int_normalize(o);
    return (COObject *)maybe_small_int(o);

on_error:
    COErr_Format(COException_ValueError,
                 "invalid literal for int() with base %d: %s", base, start);
    CO_XDECREF(o);
    return NULL;
}

/*
 * Create a new int object from a C long int
 */
COObject *
COInt_FromLong(long ival)
{
    COIntObject *o;
    unsigned long abs_ival;
    int sign = 1;

    CHECK_SMALL_INT(ival);

    if (ival < 0) {
        abs_ival = 0U - (unsigned long)ival;
        sign = -1;
    } else {
        abs_ival = (unsigned long)ival;
    }

    /* single-digit int */
    if (!(abs_ival >> COInt_SHIFT)) {
        o = _COIntObject_New(1);
        if (!o) {
            return NULL;
        }
        CO_SIZE(o) = sign;
        o->co_digit[0] = CO_SAFE_DOWNCAST(abs_ival, unsigned long, digit);
        return (COObject *)o;
    }

    /* multi-digit int */
    unsigned long t;
    int ndigits = 0;
    t = abs_ival;
    while (t) {
        ++ndigits;
        t >>= COInt_SHIFT;
    }
    o = _COIntObject_New(ndigits);
    if (!o) {
        return NULL;
    }
    digit *p = o->co_digit;
    CO_SIZE(o) = ndigits * sign;
    t = abs_ival;
    while (t) {
        *p++ = CO_SAFE_DOWNCAST(t & COInt_MASK, unsigned long, digit);
        t >>= COInt_SHIFT;
    }

    return (COObject *)o;
}
