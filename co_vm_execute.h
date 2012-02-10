#ifndef CO_EXECUTE_H
#define CO_EXECUTE_H

#include "co_compile.h"
#include "co_parser.h"

#define CO_VM_CONTINUE 0
#define CO_VM_RETURN   1
#define CO_VM_ENTER    2
#define CO_VM_LEAVE    3

#define EX_T(offset)    (*(temp_variable *)((char*)EX(ts) + offset))
#define T(offset)       (*(temp_variable *)((char*)ts + offset))

typedef struct _co_vm_stack co_vm_stack;

struct _co_vm_stack {
    void **top;
    void **end;
    struct _co_vm_stack *prev;
    void *elements[1];
};

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

/* executor */
void co_vm_init();
void co_vm_execute(co_op_array *op_array);

#endif
