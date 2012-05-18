#ifndef VM_H
#define VM_H
/**
 * Stack-based Virtual Machine
 *
 * http://en.wikipedia.org/wiki/Stack_machine
 */

#include "object.h"

COObject *vm_eval(COObject *main, COObject *globals);

#endif
