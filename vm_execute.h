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
    struct co_exception_buf exception_buf;
} co_executor_globals;
extern co_executor_globals executor_globals;

/* vm opcode handler */
typedef int (*op_handler_t) (co_execute_data *execute_data);

/* vm executor opcode handlers */
extern int co_do_add(co_execute_data *execute_data);
extern int co_do_sub(co_execute_data *execute_data);
extern int co_do_mul(co_execute_data *execute_data);
extern int co_do_div(co_execute_data *execute_data);
extern int co_do_mod(co_execute_data *execute_data);
extern int co_do_smaller(co_execute_data *execute_data);
extern int co_do_print(co_execute_data *execute_data);
extern int co_do_assign(co_execute_data *execute_data);
extern int co_do_exit(co_execute_data *execute_data);
extern int co_do_jmpz(co_execute_data *execute_data);
extern int co_do_jmp(co_execute_data *execute_data);
extern int co_do_declare_function(co_execute_data *execute_data);
extern int co_do_return(co_execute_data *execute_data);
extern int co_do_init_fcall(co_execute_data *execute_data);
extern int co_do_fcall(co_execute_data *execute_data);
extern int co_do_pass_param(co_execute_data *execute_data);
extern int co_do_recv_param(co_execute_data *execute_data);

extern void co_vm_init();
extern void co_vm_execute(co_opline_array *op_array);

#endif
