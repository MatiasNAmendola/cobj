#ifndef CO_EXECUTE_H
#define CO_EXECUTE_H

#include "compile.h"
#include "parser.h"
#include "error.h"

#define EX_T(offset)    (*(temp_variable *)((char*)EX(ts) + offset))
#define T(offset)       (*(temp_variable *)((char*)ts + offset))

typedef struct _co_vm_stack {
    void **top;
    void **end;
    struct _co_vm_stack *prev;
    void *elements[1];
} co_vm_stack;

/* execute data */
typedef struct _co_execute_data {
    co_opline *op;
    HashTable *symboltable;
    temp_variable *ts;
    co_opline_array *op_array;
    struct _co_execute_data *prev_execute_data;
} co_execute_data;

/* vm executor globals */
typedef struct _co_executor_globals {
    HashTable symbol_table;
    co_opline_array *active_op_array;
    co_execute_data *current_execute_data;
    co_vm_stack *vm_stack;
    co_stack argument_stack;
    struct co_exception_buf *exception_buf;
    HashTable variable_symboltable;
} co_executor_globals;
extern co_executor_globals executor_globals;

extern void co_vm_init();
extern void co_vm_execute(co_opline_array *op_array);

/* cval handlers */
cval *getcval(const char *name);
bool putcval(const char *name, cval *val);
bool delcval(const char *name);

#endif
