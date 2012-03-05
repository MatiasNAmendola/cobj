#ifndef VM_EXECUTE_H
#define VM_EXECUTE_H

#include "compile.h"
#include "parser.h"
#include "error.h"

typedef struct _co_vm_stack {
    void **top;
    void **end;
    struct _co_vm_stack *prev;
    void *elements[1];
} co_vm_stack;

typedef struct _co_execute_data {
    co_opline *op;
    temp_variable *ts;
    co_opline_array *op_array;
    struct _co_execute_data *prev_execute_data;
} co_execute_data;

typedef struct _co_executor_globals {
    co_opline_array *active_op_array;
    co_execute_data *current_execute_data;
    co_vm_stack *vm_stack;
    co_stack argument_stack;
    struct co_exception_buf *exception_buf;
    HashTable variable_symboltable;
} co_executor_globals;
extern co_executor_globals executor_globals;
#define EG(v)   executor_globals.v

extern void co_vm_init();
extern void co_vm_execute(co_opline_array *op_array);

/* cval handlers */
extern cval *cval_get(const char *name);
extern bool cval_put(const char *name, cval *val);
extern bool cval_del(const char *name);
extern void cval_print(cval *val);

#endif
