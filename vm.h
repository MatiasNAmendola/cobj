#ifndef VM_H
#define VM_H
/**
 * Stack-based Virtual Machine
 *
 * http://en.wikipedia.org/wiki/Stack_machine
 */

#include "object.h"

struct vm_stack {
    struct vm_stack *prev_stack;
    void **top;
    void **end;
    void *elements[1];
};

COObject *vm_eval(COObject *main);

#endif
