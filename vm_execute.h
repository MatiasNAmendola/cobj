#ifndef VM_EXECUTE_H
#define VM_EXECUTE_H

#include "compile.h"
#include "parser.h"
#include "error.h"

struct co_vm_stack {
    void **top;
    void **end;
    struct co_vm_stack *prev;
    void *elements[1];
};

struct co_exec_data {
    struct co_opline *op;
    union temp_variable *ts;
    struct co_opline_array *opline_array;
    struct co_exec_data *prev_exec_data;
};

struct co_executor_globals {
    struct co_opline_array *active_opline_array;
    struct co_exec_data *current_exec_data;
    struct co_vm_stack *vm_stack;
    co_stack argument_stack;
    HashTable variable_symboltable;
};

extern struct co_executor_globals executor_globals;
#define EG(v)   executor_globals.v

extern void co_vm_init();
extern void co_vm_execute(struct co_opline_array *opline_array);

/* cval handlers */
extern struct cval *cval_get(const char *name);
extern bool cval_put(const char *name, struct cval *val);
extern bool cval_del(const char *name);
extern void cval_print(struct cval *val);

#endif
