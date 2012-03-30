#include "../co.h"

/*
 * Small integers are preallocated in this array so that they can be shared.
 *
 * [SMALL_NEG_INT, SMALL_POS_INT)
 */
#define SMALL_NEG_INT   10
#define SMALL_POS_INT   257
static COIntObject small_ints[SMALL_NEG_INT + SMALL_POS_INT];

#define CHECK_SMALL_INT(ival)   \
    do if (-SMALL_NEG_INT <= ival && ival < SMALL_POS_INT) {    \
        return (COObject *)(small_ints + ival + SMALL_NEG_INT); \
    } while (0);

#define MAX_LONG_DIGITS \
    ((SIZE_MAX - offsetof(COIntObject, co_digit))/sizeof(digit))
#define ABS(x) ((x) < 0 ? -(x) : (x))

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

/**
 * Convert a big integer to a base 10 string.
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
    size = 1 + size_a * COInt_SHIFT / ( 3 * COInt_DECIMAL_SHIFT);
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
    for (i = size_a; --i >= 0; ) {
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

static long
int_hash(COIntObject *o)
{
    unsigned long x;
    int i;
    int sign;
    i = CO_SIZE(o);
    switch (i) {
    case -1: return o->co_digit[0] == 1 ? -2 : -(sdigit)o->co_digit[0];
    case 0: return 0;
    case 1: return o->co_digit[0];
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
COInt_FromString(char *s, int base)
{
    COIntObject *size;

    if (base != 0 && (base < 2 || base > 36)) {
        // TODO errors
        return NULL;
    }
    long ival = strtol(s, NULL, base);

    CHECK_SMALL_INT(ival);

    size = COVarObject_New(COIntObject, &COInt_Type, 1);
    size->co_digit[0] = ival;
    return (COObject *)size;
}

/**
 * Create a new big int object from a C long int
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
        return (COObject*)o;
    }

    /* big int */
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
