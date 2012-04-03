#ifndef CO_H
#define CO_H
/*
 * Cache for header files for simplicity.
 *
 * Notice:
 *  Other header file should not include this!
 *
 * Dependencies:
 *         the_compat.h
 *      /      |       \
 *  a.h       b.h      c.h ...
 *    |   \    |       /  |
 *    |  co.h (cached)    |
 *    |   /    |       \  |
 *  a.c       b.c        c.c
 *
 * Author: Yecheng Fu <cofyc.jackson@gmail.com>
 */

#include "co_compat.h"
#include "compile.h"
#include "error.h"
#include "llist.h"
#include "object.h"
#include "parser.h"
#include "scanner.h"
#include "usage.h"
#include "vm.h"
#include "vm_opcodes.h"
#include "wrapper.h"
#include "serialize.h"
#include "mem.h"
#include "gc.h"
#include "ast.h"
#include "state.h"
#include "object.h"
#include "objects/typeobject.h"
#include "objects/intobject.h"
#include "objects/boolobject.h"
#include "objects/floatobject.h"
#include "objects/strobject.h"
#include "objects/functionobject.h"
#include "objects/codeobject.h"
#include "objects/listobject.h"
#include "objects/tupleobject.h"
#include "objects/frameobject.h"
#include "objects/dictobject.h"
#include "objects/codeobject.h"
#include "objects/bytesobject.h"
#include "objects/fileobject.h"
#include "objects/exceptionobject.h"
#include "objects/capsuleobject.h"

#include "argparse/argparse.h"
#include "linenoise/linenoise.h"

/*** Help Macros ***/
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/* CO_SAFE_DOWNCAST(VALUE, WIDE, NARROW)
 * Cast VALUE to type NARROW from type WIDE.  In CO_DEBUG mode, this
 * assert-fails if any information is lost.
 * Caution:
 *    VALUE may be evaluated more than once.
 */
#ifdef CO_DEBUG
#define CO_SAFE_DOWNCAST(VALUE, WIDE, NARROW) \
    (assert((WIDE)(NARROW)(VALUE) == (VALUE)), (NARROW)(VALUE))
#else
#define CO_SAFE_DOWNCAST(VALUE, WIDE, NARROW) (NARROW)(VALUE)
#endif

#define CHAR_MASK(c) ((unsigned char)((c) & 0xff))


/* COARITHMETIC_RIGHT_SHIFT
 * C doesn't define whether a right-shift of a signed integer sign-extends
 * or zero-fills.  Here a macro to force sign extension:
 * COARITHMETIC_RIGHT_SHIFT(TYPE, I, J)
 *    Return I >> J, forcing sign extension.  Arithmetically, return the
 *    floor of I/2**J.
 * Requirements:
 *    I should have signed integer type.  In the terminology of C99, this can
 *    be either one of the five standard signed integer types (signed char,
 *    short, int, long, long long) or an extended signed integer type.
 *    J is an integer >= 0 and strictly less than the number of bits in the
 *    type of I (because C doesn't define what happens for J outside that
 *    range either).
 *    TYPE used to specify the type of I, but is now ignored.  It's been left
 *    in for backwards compatibility with versions <= 2.6 or 3.0.
 * Caution:
 *    I may be evaluated more than once.
 */
#define CO_ARITHMETIC_RIGHT_SHIFT(TYPE, I, J) \
    ((I) < 0 ? -1-((-1-(I)) >> (J)) : (I) >> (J))

/*** ! Help Macros ***/

#endif
