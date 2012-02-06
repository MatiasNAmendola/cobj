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
extern int co_do_print(co_execute_data *execute_data);
extern int co_do_assign(co_execute_data *execute_data);
extern int co_do_exit(co_execute_data *execute_data);
extern int co_do_if_cond(co_execute_data *execute_data);
extern int co_do_if_after_statement(co_execute_data *execute_data);

/* executor */
void init_executor();
void co_vm_execute(co_op_array *op_array);

static inline cval *
get_cval_ptr(cnode *node, const temp_variable *ts)
{
    cval *cvalptr;

    switch (node->op_type) {
    case T_FNUM:
    case T_NUM:
    case T_STRING:
        return &node->u.val;
        break;
    case T_NAME:
        cvalptr = getcval(node->u.val.u.str.val);
        if (!cvalptr) {
            cval cvalnew;

            putcval(node->u.val.u.str.val, &cvalnew);
            cvalptr = getcval(node->u.val.u.str.val);
        }
        return cvalptr;
        break;
    case IS_TMP_VAR:
        return &T(node->u.var).tmp_var;
        break;
    case IS_UNUSED:
        return NULL;
        break;
    }

    return NULL;
}

#endif
