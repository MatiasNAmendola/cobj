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

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

/* Smallest negative value of type ssize_t. */
#define SSIZE_MIN   ((ssize_t)(-SSIZE_MAX-1))

#endif
