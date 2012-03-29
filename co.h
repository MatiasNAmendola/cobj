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
#include "debug.h"
#include "error.h"
#include "llist.h"
#include "object.h"
#include "parser.h"
#include "scanner.h"
#include "stack.h"
#include "usage.h"
#include "vm.h"
#include "vm_opcodes.h"
#include "wrapper.h"
#include "serialize.h"
#include "mem.h"
#include "gc.h"
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
#include "objects/oplineobject.h"
#include "objects/bytesobject.h"
#include "objects/fileobject.h"
#include "objects/exceptionobject.h"

#include "argparse/argparse.h"
#include "linenoise/linenoise.h"

/* Help macros */
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

#endif
