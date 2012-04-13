#ifndef COMPAT_H
#define COMPAT_H
/* 
 * CObject Compatible Header.
 *
 * Provide ccompatibility for systems that co runs.
 */

/* ISO C99 */
#include <assert.h>
#include <complex.h>
#include <ctype.h>
#include <errno.h>
#include <fenv.h>
#include <float.h>
#include <inttypes.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

/* POSIX */
#include <unistd.h>
#include <fcntl.h>

/* Smallest negative value of type ssize_t. */
#define SSIZE_MIN   ((ssize_t)(-SSIZE_MAX-1))

/* Checking for overflow macros. Since c doesn't define anything about what
 * happens when a signed integer operation overflows, and some compilers think
 * they're doing you a favor by being "clever" then. The bit pattern for the
 * largest positive signed long is (unsigned long)LONG_MAX, and for the
 * smallerst negative signed long it is abs(LONG_MAX), which we can write
 * -(unsigned long)LONG_MIN. However, some compilers warn about applying unary
 *  minus to an unsigned operand. Hence the weird "0-".
 *
 * Same as ssize_t.
 */
#define CO_ABS_LONG_MIN     (0 - (unsigned long)LONG_MIN)
#define CO_ABS_SSIZE_MIN    (0 - (size_t)SSIZE_MIN)

#endif
