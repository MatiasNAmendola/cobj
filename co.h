#ifndef CO_H
#define CO_H
/* 
 * Cache for header files for simplicity.
 *
 * Notice:
 *  Other header file should not include this!
 *
 * Dependencies:
 *          the-compat.h
 *      /       |       \
 *  a.h         b.h     c.h ...
 *      |   \   |       /   |
 *      |   co.h (cached)   |
 *      |   /   |       \   |
 *  a.c         b.c        c.c
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
#include "malloc.h"
#include "gc.h"

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

#include "argparse/argparse.h"

/* Helper Functions */
static inline ssize_t
xread(int fd, void *buf, size_t len)
{
    ssize_t nr;

    while (1) {
        nr = read(fd, buf, len);
        if ((nr < 0) && (errno == EAGAIN || errno == EINTR))
            continue;
        return nr;
    }
}

static inline ssize_t
xwrite(int fd, const void *buf, size_t len)
{
    ssize_t nr;

    while (1) {
        nr = write(fd, buf, len);
        if ((nr < 0) && (errno == EAGAIN || errno == EINTR))
            continue;
        return nr;
    }
}

int verbose;

#endif
