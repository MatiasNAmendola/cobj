#ifndef OBJECTS_INTOBJECT_H
#define OBJECTS_INTOBJECT_H
/**
 * Integer object type (Arbitrary Precision/Big Number)
 *
 * @link http://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic
 */

#include "../object.h"

/**
 * Parameters of the big number representation. There are two different sets of
 * parameters: 
 *  one set of 30-bit digits, stored in an unsigned 32-bit integer type
 *  one set of 15-bit digits, stored in an unsinged 16-bit integer type
 *
 * The value of COInt_BITS_IN_DIGIT is used to decide which digit size to use.
 *
 * Type 'digit' should be able to hold 2*COInt_BASE-1, and type 'twodigits'
 * should be an unsigned integer type able to hold all integers up to
 * COInt_BASE*COInt_BASE-1.
 */
#define COInt_BITS_IN_DIGIT 30
#define COInt_SHIFT 30
typedef uint32_t digit;
typedef int32_t sdigit;
typedef uint64_t twodigits;
typedef int64_t stwodigits;
#define COInt_BASE ((digit)1 << COInt_SHIFT)
#define COInt_MASK ((digit)(COInt_BASE - 1))
#define COInt_DECIMAL_SHIFT   9 /* max(e such that 10**e fits in a digit) */
#define COInt_DECIMAL_BASE    ((digit)1000000000) /* 10 ** DECIMAL_SHIFT */

typedef struct _COIntObject {
    COVarObject_HEAD;
    /* Big Integer Representation.
     *
     * The absolute value of a number is equal to 
     *  SUM(for i=0 through abs(co_size) - 1 co_digit[i] * 2 ** (SHIFT * i))
     * Negative numbers are represented with co_size < 0.
     * Zero is represented by co_size = 0.
     *
     * In a normalized number, co_digit[abs(co_size) - 1] (the most significaent
     * digit) is never zero. Also, in all cases, for all valid i, 0 <=
     * co_digit[i] <= MASK.
     */
    digit co_digit[1];
} COIntObject;

COTypeObject COInt_Type;

#define COInt_Check(co) (CO_TYPE(co) == &COInt_Type)

int COInt_Init(void);
COObject *COInt_FromString(char *s, char **pend, int base);
COObject *COInt_FromLong(long ival);
long COInt_AsLong(COObject *co);

#endif
