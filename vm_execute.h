#ifndef VM_EXECUTE_H
#define VM_EXECUTE_H

#include "compile.h"
#include "parser.h"
#include "error.h"
#include "object.h"
#include "objects/boolobject.h"
#include "objects/intobject.h"
#include "objects/strobject.h"
#include "objects/floatobject.h"
#include "objects/functionobject.h"

struct co_vm_stack {
    void **top;
    void **end;
    struct co_vm_stack *prev;
    void *elements[1];
};

/* execution frame */
struct co_exec_data {
    struct co_opline *op;
    union temp_variable *ts;
    struct co_opline_array *opline_array;
    struct co_exec_data *prev_exec_data;
    struct COObject *function_called;
    HashTable symbol_table;
};

struct co_executor_globals {
    struct COObject *next_func;
    struct co_exec_data *current_exec_data;
    struct co_vm_stack *vm_stack;
    co_stack argument_stack;
};

extern struct co_executor_globals executor_globals;
#define EG(v)   executor_globals.v

extern void co_vm_init();
extern void co_vm_execute(struct co_opline_array *opline_array);
extern void co_vm_shutdown();

/* COObject handlers */
extern struct COObject **COObject_get(struct COObject *co);
extern bool COObject_put(struct COObject *name, struct COObject * co);
extern bool COObject_del(const char *name);
extern void COObject_print(struct COObject *co);

#endif
