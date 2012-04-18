#ifndef PEEPHOLE_H
#define PEEPHOLE_H
/**
 * Perform basic peephole optimizations to code object.
 */

#include "object.h"
COObject *peephole_optimize(COObject *code, COObject *consts, COObject *names);

#endif
