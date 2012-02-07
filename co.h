#ifndef CO_H
#define CO_H

/* C99 Standand Headers */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

/* local header */
#include "local.h"

#if SIZEOF_LONG == 4
#define MAX_LENGTH_OF_LONG 11
#elif SIZEOF_LONG == 8
#define MAX_LENGTH_OF_LONG 22
#else
#error "Unknow SIZEOF_LONG"
#endif

#define MAX_LENGTH_OF_DOUBLE 32

typedef unsigned int uint;

typedef unsigned long ulong;

typedef unsigned char uchar;

/* Compiler */
extern int coparse(void *compiler_globals);
extern void coerror(const char *str);

/* Wrapper Functions */
extern void *xmalloc(size_t size);
extern void *xrealloc(void *ptr, size_t size);
extern void *xcalloc(size_t nmemb, size_t size);
extern char *xstrndup(const char *str, size_t len);
extern bool safe_strtol(const char *str, long int *out);

/* Helper Functions */
extern void die(const char *err, ...);
extern void usage(const char *err);
extern int error(const char *err, ...);
extern void warning(const char *err, ...);

#endif
