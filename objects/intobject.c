#include "../co.h"

#define MAX_LONG_DIGITS \
    ((SIZE_MAX - offsetof(COIntObject, co_digit))/sizeof(digit))
#define ABS(x) ((x) < 0 ? -(x) : (x))

/* Convert 0 or 1 size of COInt to an sdigit */
#define ONEDIGIT_VALUE(x) \
    (CO_SIZE(x) < 0 ? -(sdigit)(x)->co_digit[0] : \
     (CO_SIZE(x) == 0 ? (sdigit) 0 : (sdigit)(x)->co_digit[0]))

/* If a long is already shared (which must be a small integer), negating it must
 * go to COInt_FromLong */
#define NEGATE(x) \
    do if (CO_REFCNT(x) == 1) {         \
        CO_SIZE(x) = - CO_SIZE(x);      \
    } else {                            \
        COObject *tmp = COInt_FromLong(-ONEDIGIT_VALUE(x)); \
        CO_DECREF(x);                                       \
        (x) = (COIntObject *)tmp;                           \
    } while (0);

/* bits_in_digit(d) returns the unique integer k such that 2**(k-1) <= d <
   2**k if d is nonzero, else 0. */

static const unsigned char BitLengthTable[32] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

static int
bits_in_digit(digit d)
{
    int d_bits = 0;
    while (d >= 32) {
        d_bits += 6;
        d >>= 6;
    }
    d_bits += (int)BitLengthTable[d];
    return d_bits;
}

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
_COInt_New(ssize_t size)
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

COObject *
_COInt_Copy(COIntObject *src)
{
    COIntObject *x;
    ssize_t i;

    assert(src != NULL);

    i = CO_SIZE(src);
    if (i < 0)
        i = -i;
    if (i < 2) {
        sdigit ival = src->co_digit[0];
        CHECK_SMALL_INT(ival);
    }
    x = _COInt_New(i);
    if (x) {
        CO_SIZE(x) = CO_SIZE(src);
        while (--i >= 0)
            x->co_digit[i] = src->co_digit[i];
    }

    return (COObject *)x;
}

/*
 * Get a ssize_t from a int object.
 * Returns -1 and sets an error if overflow occurs.
 */
ssize_t
COInt_AsSsize_t(COObject *o)
{
    register COIntObject *_o;
    size_t x, prev;
    ssize_t i;
    int sign;

    assert(o != NULL);
    if (!COInt_Check(o)) {
        COErr_SetString(COException_ValueError, "an integer is required");
        return -1;
    }
    _o = (COIntObject *)o;
    i = CO_SIZE(_o);
    switch (i) {
    case -1:
        return -(sdigit)_o->co_digit[0];
    case 0:
        return 0;
    case 1:
        return _o->co_digit[0];
    }

    sign = 1;
    x = 0;
    if (i < 0) {
        sign = -1;
        i = -i;
    }
    while (--i >= 0) {
        prev = x;
        x = (x << COInt_SHIFT) | _o->co_digit[i];
        if ((x >> COInt_SHIFT) != prev)
            goto overflow;
    }

    if (x <= (size_t) SSIZE_MAX) {
        return (ssize_t) x *sign;
    } else if (sign < 0 && x == (0 - (size_t) SSIZE_MIN)) {
        return SSIZE_MIN;
    }
    /* else overflow */

overflow:
    COErr_SetString(COException_OverflowError,
                    "int too large to convert to C ssize_t");
    return -1;
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

    o = _COInt_New(n);
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
    scratch = _COInt_New(size);
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
        // if exchanged, negate
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
    o = _COInt_New(size_a);
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
        NEGATE(o);
    return int_normalize(o);
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
    o = _COInt_New(size_a + 1);
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

/*
 * Multiply the absolute values of two integers.
 */
static COIntObject *
x_mul(COIntObject *a, COIntObject *b)
{
    ssize_t size_a = ABS(CO_SIZE(a));
    ssize_t size_b = ABS(CO_SIZE(b));
    COIntObject *o;
    ssize_t i;

    o = _COInt_New(size_a + size_b);
    if (!o)
        return NULL;

    memset(o->co_digit, 0, CO_SIZE(o) * sizeof(digit));

    if (a == b) {
        /* Efficient squaring per HAC, Algorithm 14.16:
         * http://www.cacr.math.uwaterloo.ca/hac/about/chap14.pdf
         */
        for (i = 0; i < size_a; ++i) {
            twodigits carry;
            twodigits f = a->co_digit[i];
            digit *po = o->co_digit + (i << 1);
            digit *pa = a->co_digit + i + 1;
            digit *paend = a->co_digit + size_a;

            carry = *po + f * f;
            *po++ = (digit)(carry & COInt_MASK);
            carry >>= COInt_SHIFT;
            assert(carry <= COInt_MASK);

            f <<= 1;
            while (pa < paend) {
                carry += *po + *pa++ * f;
                *po++ = (digit)(carry & COInt_MASK);
                carry >>= COInt_SHIFT;
            }
            if (carry) {
                carry += *po;
                *po++ = (digit)(carry & COInt_MASK);
                carry >>= COInt_SHIFT;
            }
            if (carry)
                *po += (digit)(carry & COInt_MASK);
        }
    } else {
        for (i = 0; i < size_a; ++i) {
            twodigits carry = 0;
            twodigits f = a->co_digit[i];
            digit *po = o->co_digit + i;
            digit *pb = b->co_digit;
            digit *pbend = b->co_digit + size_b;

            while (pb < pbend) {
                carry += *po + *pb++ * f;
                *po++ = (digit)(carry & COInt_MASK);
                carry >>= COInt_SHIFT;
            }
            if (carry)
                *po += (digit)(carry & COInt_MASK);
        }
    }
    return int_normalize(o);
}

/* Shift digit vector a[0:m] d bits left, with 0 <= d < COInt_SHIFT.  Put
 * result in z[0:m], and return the d bits shifted out of the top.
 */
static digit
v_lshift(digit *z, digit *a, ssize_t m, int d)
{
    ssize_t i;
    digit carry = 0;

    assert(0 <= d && d < COInt_SHIFT);
    for (i = 0; i < m; i++) {
        twodigits acc = (twodigits)a[i] << d | carry;
        z[i] = (digit)acc & COInt_MASK;
        carry = (digit)(acc >> COInt_SHIFT);
    }
    return carry;
}

/* Shift digit vector a[0:m] d bits right, with 0 <= d < COInt_SHIFT.  Put
 * result in z[0:m], and return the d bits shifted out of the bottom.
 */
static digit
v_rshift(digit *z, digit *a, ssize_t m, int d)
{
    ssize_t i;
    digit carry = 0;
    digit mask = ((digit)1 << d) - 1U;

    assert(0 <= d && d < COInt_SHIFT);
    for (i = m; i-- > 0;) {
        twodigits acc = (twodigits)carry << COInt_SHIFT | a[i];
        carry = (digit)acc & mask;
        z[i] = (digit)(acc >> d);
    }
    return carry;
}

/*
 * Divide the absolute values of two integers with remainder.
 *
 * The arguments v1 and w1 should satisfy 2 <= ABS(CO_SIZE(w1)) <=
 * ABS(CO_SIZE(v1).
 */
static COIntObject *
x_divrem(COIntObject *v1, COIntObject *w1, COIntObject **prem)
{
    COIntObject *v, *w, *a;
    size_t i, k, size_v, size_w;
    int d;
    digit wm1, wm2, carry, q, r, vtop, *v0, *vk, *w0, *ak;
    twodigits vv;
    sdigit zhi;
    stwodigits z;

    /*
     * We follow Kunth [TAOCP, Vol.2 (3rd, edn), section 4.3.1, Algorithm D],
     * except that we don't explicitly handle the special case when the initial
     * estimate q for a quotinent digit is >= COInt_BASE: the max value for q is
     * COInt_BASE+1, and that won't overflow a digit.
     */

    // allocate space, w will also be used to hold the final remainder
    size_v = ABS(CO_SIZE(v1));
    size_w = ABS(CO_SIZE(w1));
    assert(size_v >= size_w && size_w >= 2);

    v = _COInt_New(size_v + 1);
    if (!v) {
        *prem = NULL;
        return NULL;
    }

    w = _COInt_New(size_w);
    if (!w) {
        CO_DECREF(v);
        *prem = NULL;
        return NULL;
    }

    /* normalize: shift w1 left so that its top digit is >= COInt_BASE/2.
       shift v1 left by the same amount.  Results go into w and v. */
    d = COInt_SHIFT - bits_in_digit(w1->co_digit[size_w - 1]);
    carry = v_lshift(w->co_digit, w1->co_digit, size_w, d);
    assert(carry == 0);
    carry = v_lshift(v->co_digit, v1->co_digit, size_v, d);
    if (carry != 0 || v->co_digit[size_v - 1] >= w->co_digit[size_w - 1]) {
        v->co_digit[size_v] = carry;
        size_v++;
    }

    /* Now v->co_digit[size_v-1] < w->co_digit[size_w-1], so quotient has
       at most (and usually exactly) k = size_v - size_w digits. */
    k = size_v - size_w;
    assert(k >= 0);
    a = _COInt_New(k);
    if (a == NULL) {
        CO_DECREF(w);
        CO_DECREF(v);
        *prem = NULL;
        return NULL;
    }
    v0 = v->co_digit;
    w0 = w->co_digit;
    wm1 = w0[size_w - 1];
    wm2 = w0[size_w - 2];

    for (vk = v0 + k, ak = a->co_digit + k; vk-- > v0;) {
        /* inner loop: divide vk[0:size_w+1] by w0[0:size_w], giving
           single-digit quotient q, remainder in vk[0:size_w]. */

        /* estimate quotient digit q; may overestimate by 1 (rare) */
        vtop = vk[size_w];
        assert(vtop <= wm1);
        vv = ((twodigits)vtop << COInt_SHIFT) | vk[size_w - 1];
        q = (digit)(vv / wm1);
        r = (digit)(vv - (twodigits)wm1 * q);   /* r = vv % wm1 */
        while ((twodigits)wm2 * q > (((twodigits)r << COInt_SHIFT)
                                     | vk[size_w - 2])) {
            --q;
            r += wm1;
            if (r >= COInt_BASE)
                break;
        }
        assert(q <= COInt_BASE);

        /* subtract q*w0[0:size_w] from vk[0:size_w+1] */
        zhi = 0;
        for (i = 0; i < size_w; ++i) {
            /* invariants: -COInt_BASE <= -q <= zhi <= 0;
               -COInt_BASE * q <= z < COInt_BASE */
            z = (sdigit)vk[i] + zhi - (stwodigits)q *(stwodigits)w0[i];
            vk[i] = (digit)z & COInt_MASK;
            zhi = (sdigit)CO_ARITHMETIC_RIGHT_SHIFT(stwodigits, z, COInt_SHIFT);
        }
        /* add w back if q was too large (this branch taken rarely) */
        assert((sdigit)vtop + zhi == -1 || (sdigit)vtop + zhi == 0);
        if ((sdigit)vtop + zhi < 0) {
            carry = 0;
            for (i = 0; i < size_w; ++i) {
                carry += vk[i] + w0[i];
                vk[i] = carry & COInt_MASK;
                carry >>= COInt_SHIFT;
            }
            --q;
        }

        /* store quotient digit */
        assert(q < COInt_BASE);
        *--ak = q;
    }

    /* unshift remainder; we reuse w to store the result */
    carry = v_rshift(w0, v0, size_w, d);
    assert(carry == 0);
    CO_DECREF(v);

    *prem = int_normalize(w);
    return int_normalize(a);
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
                NEGATE(o);
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
    return o;
}

static COIntObject *
int_sub(COIntObject *a, COIntObject *b)
{
    COIntObject *o;

    if (ABS(CO_SIZE(a)) <= 1 && ABS(CO_SIZE(b)) <= 1) {
        return (COIntObject *)COInt_FromLong(ONEDIGIT_VALUE(a) -
                                             ONEDIGIT_VALUE(b));
    }

    if (CO_SIZE(a) < 0) {
        if (CO_SIZE(b) < 0) {
            o = x_sub(a, b);
        } else {
            o = x_add(a, b);
        }
        if (o && CO_SIZE(o) != 0)
            NEGATE(o);
    } else {
        if (CO_SIZE(b) < 0) {
            o = x_add(a, b);
        } else {
            o = x_sub(a, b);
        }
    }
    return o;
}

static COIntObject *
int_mul(COIntObject *a, COIntObject *b)
{
    COIntObject *o;

    o = x_mul(a, b);
    if (!o)
        return NULL;

    /* Negate if exactly one of operands is negative. */
    if ((CO_SIZE(a) ^ CO_SIZE(b)) < 0)
        NEGATE(o);

    return o;
}

/*
 * Divide pin with size digits, by non-zero digit n, storing quotient in pout,
 * and returning the remainer.
 */
static digit
inplace_divrem1(digit *pout, digit *pin, ssize_t size, digit n)
{
    twodigits rem = 0;
    assert(n > 0 && n <= COInt_MASK);
    pin += size;
    pout += size;
    while (--size >= 0) {
        digit hi;
        rem = (rem << COInt_SHIFT) | *--pin;
        *--pout = hi = (digit)(rem / n);
        rem -= (twodigits)hi *n;
    }
    return (digit)rem;
}

/*
 * Divide a int object by a digit, returning both the quotient (as function
 * result) and the remainder (through *prem).
 * The sign of a is ignored; n should not be zero.
 */
static COIntObject *
divrem1(COIntObject *a, digit n, digit *prem)
{
    const ssize_t size = ABS(CO_SIZE(a));
    COIntObject *o;
    o = _COInt_New(size);
    if (!o)
        return NULL;
    *prem = inplace_divrem1(o->co_digit, a->co_digit, size, n);
    return int_normalize(o);
}

/* 
 * Division with remainder.
 */
static int
int_divrem(COIntObject *a, COIntObject *b, COIntObject **pdiv,
           COIntObject **prem)
{
    ssize_t size_a = ABS(CO_SIZE(a));
    ssize_t size_b = ABS(CO_SIZE(b));
    COIntObject *o;

    if (size_b == 0) {
        COErr_SetString(COException_ValueError,
                        "integer division or modulo by zero");
        return -1;
    }
    // If |a| < |b|, it's simple.
    if (size_a < size_b ||
        (size_a == size_b
         && a->co_digit[size_a - 1] < b->co_digit[size_b - 1])) {
        *pdiv = (COIntObject *)COInt_FromLong(0);
        if (!*pdiv)
            return -1;
        CO_INCREF(a);
        *prem = (COIntObject *)a;
        return 0;
    }

    if (size_b == 1) {
        digit rem = 0;
        o = divrem1(a, b->co_digit[0], &rem);
        if (!o)
            return -1;
        *prem = (COIntObject *)COInt_FromLong((long)rem);
        if (!*prem) {
            CO_DECREF(o);
            return -1;
        }
    } else {
        o = x_divrem(a, b, prem);
        if (!o)
            return -1;
    }

    // Set the sign.
    if ((CO_SIZE(a) ^ CO_SIZE(b)) < 0)
        NEGATE(o);
    if (CO_SIZE(a) < 0 && CO_SIZE(*prem) != 0)
        NEGATE(*prem);

    *pdiv = maybe_small_int(o);
    return 0;
}

static COIntObject *
int_div(COIntObject *a, COIntObject *b)
{
    COIntObject *div;
    COIntObject *rem;

    if (int_divrem(a, b, &div, &rem) < 0) {
        div = NULL;
        rem = NULL;
    }
    return div;
}

static COIntObject *
int_mod(COIntObject *a, COIntObject *b)
{
    COIntObject *div;
    COIntObject *rem;

    if (int_divrem(a, b, &div, &rem) < 0) {
        div = NULL;
        rem = NULL;
    }
    return rem;
}

static COObject *
int_neg(COIntObject *o)
{
    COIntObject *x;
    if (ABS(CO_SIZE(o)) <= 1)
        return COInt_FromLong(-ONEDIGIT_VALUE(o));
    x = (COIntObject *)_COInt_Copy(o);
    if (x)
        CO_SIZE(x) = -(CO_SIZE(x));
    return (COObject *)x;
}

static COObject *
int_invert(COIntObject *o)
{
    /* ~x -> -(x+1) */
    if (ABS(CO_SIZE(o)) <= 1)
        return COInt_FromLong(-(ONEDIGIT_VALUE(o) + 1));

    COIntObject *x;
    COIntObject *w;
    w = (COIntObject *)COInt_FromLong(1L);
    if (!w)
        return NULL;
    x = (COIntObject *)int_add(o, w);
    CO_DECREF(w);
    if (!x)
        return NULL;
    CO_SIZE(x) = -(CO_SIZE(x));
    return (COObject *)maybe_small_int(x);
}

static COObject *
int_lshift(COIntObject *a, COIntObject *b)
{
    COIntObject *x = NULL;
    ssize_t shiftby, oldsize, newsize, wordshift, remshift, i, j;
    twodigits accum;

    shiftby = COInt_AsSsize_t((COObject *)b);
    if (shiftby == -1L && COErr_Occurred())
        goto lshift_error;
    if (shiftby < 0) {
        COErr_SetString(COException_ValueError, "negative shift count");
        goto lshift_error;
    }
    wordshift = shiftby / COInt_SHIFT;
    remshift = shiftby - wordshift * COInt_SHIFT;

    oldsize = ABS(CO_SIZE(a));
    newsize = oldsize + wordshift;
    if (remshift)
        ++newsize;
    x = _COInt_New(newsize);
    if (!x)
        goto lshift_error;
    if (CO_SIZE(a) < 0)
        NEGATE(x);

    for (i = 0; i < wordshift; i++)
        x->co_digit[i] = 0;
    accum = 0;
    for (i = wordshift, j = 0; j < oldsize; i++, j++) {
        accum |= (twodigits)a->co_digit[j] << remshift;
        x->co_digit[i] = (digit)(accum & COInt_MASK);
        accum >>= COInt_SHIFT;
    }
    if (remshift)
        x->co_digit[newsize - 1] = (digit)accum;
    else
        assert(!accum);
    x = int_normalize(x);
lshift_error:
    return (COObject *)maybe_small_int(x);
}

static COObject *
int_rshift(COIntObject *a, COIntObject *b)
{
    ssize_t shiftby, wordshift, newsize, loshift, hishift, i, j;
    digit lomask, himask;
    COIntObject *x = NULL;
    if (CO_SIZE(a) < 0) {
        COIntObject *a1, *a2;
        a1 = (COIntObject *)int_invert(a);
        if (!a1)
            goto rshift_error;
        a2 = (COIntObject *)int_rshift(a1, b);
        CO_DECREF(a1);
        if (!a2)
            goto rshift_error;
        x = (COIntObject *)int_invert(a2);
        CO_DECREF(a2);
    } else {
        shiftby = COInt_AsSsize_t((COObject *)b);
        if (shiftby == -1L && COErr_Occurred())
            goto rshift_error;
        if (shiftby < 0) {
            COErr_SetString(COException_ValueError, "negative shift count");
            goto rshift_error;
        }
        wordshift = shiftby / COInt_SHIFT;
        newsize = ABS(CO_SIZE(a)) - wordshift;
        if (newsize <= 0)
            return COInt_FromLong(0);
        loshift = shiftby % COInt_SHIFT;
        hishift = COInt_SHIFT - loshift;
        lomask = ((digit)1 << hishift) - 1;
        himask = COInt_MASK ^ lomask;
        x = _COInt_New(newsize);
        if (!x)
            goto rshift_error;
        for (i = 0, j = wordshift; i < newsize; i++, j++) {
            x->co_digit[i] = (a->co_digit[j] >> loshift) & lomask;
            if (i + 1 < newsize)
                x->co_digit[i] |= (a->co_digit[j + 1] << hishift) & himask;
        }
        x = int_normalize(x);
    }

rshift_error:
    return (COObject *)maybe_small_int(x);
}

static COIntInterface int_interface = {
    (binaryfunc)int_add,
    (binaryfunc)int_sub,
    (binaryfunc)int_mul,
    (binaryfunc)int_div,
    (binaryfunc)int_mod,
    (binaryfunc)int_lshift,
    (binaryfunc)int_rshift,
    (unaryfunc)int_neg,
    (unaryfunc)int_invert,
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

/*
 * -1: a < b
 *  0: a = b
 *  1: a > b
 */
static int
int_cmp(COIntObject *a, COIntObject *b)
{
    ssize_t sign;
    if (CO_SIZE(a) != CO_SIZE(b)) {
        sign = CO_SIZE(a) - CO_SIZE(b);
    } else {
        ssize_t i = ABS(CO_SIZE(a));
        while (--i >= 0 && a->co_digit[i] == b->co_digit[i]);
        if (i < 0) {
            sign = 0;
        } else {
            sign = (sdigit)a->co_digit[i] - (sdigit)b->co_digit[i];
            if (CO_SIZE(a) < 0)
                sign = -sign;
        }
    }
    return sign < 0 ? -1 : sign > 0 ? 1 : 0;
}

static COObject *
int_compare(COIntObject *this, COIntObject *that, int op)
{
    int result;

    if (this == that)
        result = 0;
    else
        result = int_cmp(this, that);

#define TEST_COND(cond) \
        ((cond) ? CO_True : CO_False)

    COObject *x;
    switch (op) {
    case CMP_EQ:
        x = TEST_COND(result == 0);
        break;
    case CMP_NE:
        x = TEST_COND(result != 0);
        break;
    case CMP_LE:
        x = TEST_COND(result <= 0);
        break;
    case CMP_GE:
        x = TEST_COND(result >= 0);
        break;
    case CMP_LT:
        x = TEST_COND(result < 0);
        break;
    case CMP_GT:
        x = TEST_COND(result > 0);
        break;
    default:
        COErr_BadInternalCall();
        return NULL;
    }

    CO_INCREF(x);
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
    (richcmpfunc)int_compare,   /* tp_compare */
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
        o = _COInt_New(size);
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
        o = _COInt_New(1);
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
    o = _COInt_New(ndigits);
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
