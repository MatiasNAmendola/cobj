#ifndef CO_EXECUTE_H
#define CO_EXECUTE_H

#include "co_compile.h"
#include "co_parser.h"

#define EX_T(offset)    (*(temp_variable *)((char*)EX(ts) + offset))
#define T(offset)       (*(temp_variable *)((char*)ts + offset))

typedef struct _co_vm_stack {
    void **top;
    void **end;
    struct _co_vm_stack *prev;
    void *elements[1];
} co_vm_stack;

/* vm executor globals */
typedef struct _co_executor_globals {
    HashTable symbol_table;
    co_op_array *active_op_array;
    co_execute_data *current_execute_data;
    co_vm_stack *argument_stack;
} co_executor_globals;
extern co_executor_globals executor_globals;

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
extern int co_do_fcall(co_execute_data *execute_data);

extern void co_vm_init();
extern void co_vm_execute(co_op_array *op_array);

#endif
