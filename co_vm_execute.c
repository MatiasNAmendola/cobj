#include "co_vm_opcodes.h"
#include "co_vm_execute.h"
#include "co_globals.h"
#include "co_parser.h"

#ifdef EX
#undef EX
#endif
#define EX(v)   execute_data->v

#define CO_VM_STACK_PAGE_SIZE (64 * 1024)
#define CO_VM_STACK_GROW_IF_NEEDED(size)                \
    if (size > EG(argument_stack)->end                  \
            - EG(argument_stack)->top) {                \
            co_vm_stack_extend(size);                   \
    }

static inline co_vm_stack *
co_vm_stack_new_page(size_t size)
{
    co_vm_stack *page =
        (co_vm_stack *)xmalloc(sizeof(co_vm_stack) + sizeof(page->elements[0]) * (size - 1));
    page->top = page->elements;
    page->end = page->elements + size;
    page->prev = NULL;
    return page;
}

static inline void
co_vm_stack_extend(size_t size)
{
    co_vm_stack *p =
        co_vm_stack_new_page(size >= CO_VM_STACK_PAGE_SIZE ? size : CO_VM_STACK_PAGE_SIZE);
    p->prev = EG(argument_stack);
    EG(argument_stack) = p;
}

static inline cval *
get_cval_ptr(cnode *node, const temp_variable *ts)
{
    cval *cvalptr;

    switch (node->op_type) {
    case IS_CONST:
        return &node->u.val;
        break;
    case IS_VAR:
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

static inline void
co_vm_stack_init()
{
    EG(argument_stack) = co_vm_stack_new_page(CO_VM_STACK_PAGE_SIZE);
}

static inline void
co_vm_stack_destory()
{
    co_vm_stack *stack = EG(argument_stack);

    while (stack != NULL) {
        co_vm_stack *p = stack->prev;

        free(stack);
        stack = p;
    }
}

static inline void
co_vm_stack_push(void *ptr)
{
    CO_VM_STACK_GROW_IF_NEEDED(1);
    *(EG(argument_stack)->top++) = ptr;
}

static inline void *
co_vm_stack_pop()
{
    void *e = *(--EG(argument_stack)->top);

    if (EG(argument_stack)->top == EG(argument_stack)->elements) {
        co_vm_stack *p = EG(argument_stack);

        EG(argument_stack) = p->prev;
        free(p);
    }

    return e;
}

static inline void *
co_vm_stack_alloc(size_t size)
{
    void *ret;

    size = (size + (sizeof(void *) - 1)) / sizeof(void *);

    CO_VM_STACK_GROW_IF_NEEDED(size);

    ret = (void *)EG(argument_stack)->top;
    EG(argument_stack)->top += size;
    return ret;
}

static op_handler_t
get_op_handler(int opcode)
{
    switch (opcode) {
    case OP_ADD:
        return co_do_add;
        break;
    case OP_SUB:
        return co_do_sub;
        break;
    case OP_MUL:
        return co_do_mul;
        break;
    case OP_DIV:
        return co_do_div;
        break;
    case OP_MOD:
        return co_do_mod;
    case OP_IS_SMALLER:
        return co_do_smaller;
        break;
    case OP_ASSIGN:
        return co_do_assign;
        break;
    case OP_PRINT:
        return co_do_print;
        break;
    case OP_JMPZ:
        return co_do_jmpz;
        break;
    case OP_JMP:
        return co_do_jmp;
        break;
    case OP_EXIT:
        return co_do_exit;
    case OP_DECLARE_FUNCTION:
        return co_do_declare_function;
    case OP_RETURN:
        return co_do_return;
    case OP_DO_FCALL:
        return co_do_fcall;
        break;
    }
    die("unknown handle for opcode(%d)\n", opcode);
}

void
co_vm_execute(co_op_array *op_array)
{
    co_execute_data *execute_data;

  vm_enter:
    /* initialize execute_data */
    execute_data =
        (co_execute_data *)co_vm_stack_alloc(sizeof(co_execute_data) +
                                             sizeof(temp_variable) * op_array->t);

    EX(ts) = (temp_variable *)((char *)execute_data + sizeof(co_execute_data));

    EX(op_array) = op_array;

    EG(current_execute_data) = execute_data;

    if (op_array->start_op) {
        EX(op) = op_array->start_op;
    } else {
        EX(op) = op_array->ops;
    }

    while (true) {
        int ret;
        op_handler_t handler = get_op_handler(EX(op)->opcode);
        ret = handler(execute_data);

        switch (ret) {
        case CO_VM_CONTINUE:
            break;
        case CO_VM_RETURN:
            return;
        case CO_VM_ENTER:
            op_array = EG(active_op_array);
            goto vm_enter;
        case CO_VM_LEAVE:
            execute_data = EG(current_execute_data);
        default:
            break;
        }
    }

    die("Arrived at end of main loop which shouldn't happen");
}

void
co_vm_init()
{
    co_vm_stack_init();
}

int
co_do_add(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));
    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival + val2->u.ival;
    result->type = CVAL_IS_INT;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_sub(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));
    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival - val2->u.ival;
    result->type = CVAL_IS_INT;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_mul(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));
    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival * val2->u.ival;
    result->type = CVAL_IS_INT;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_div(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));
    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival / val2->u.ival;
    result->type = CVAL_IS_INT;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_mod(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));
    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival % val2->u.ival;
    result->type = CVAL_IS_INT;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_smaller(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));
    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival < val2->u.ival;
    result->type = CVAL_IS_BOOL;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_print(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    switch (val1->type) {
    case CVAL_IS_NONE:
        printf("%s\n", "None");
        break;
    case CVAL_IS_BOOL:
        if (val1->u.ival) {
            printf("%s\n", "True");
        } else {
            printf("%s\n", "False");
        }
        break;
    case CVAL_IS_INT:
        printf("%ld\n", val1->u.ival);
        break;
    case CVAL_IS_STRING:
        printf("%s\n", val1->u.str.val);
        break;
    default:
        die("do print error (type: %d)", val1->type);
    }

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_assign(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1, *val2, *result;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    val2 = get_cval_ptr(&op->op2, EX(ts));

    result = get_cval_ptr(&op->result, EX(ts));
    result->u.ival = val1->u.ival = val2->u.ival;
    val1->type = CVAL_IS_INT;
    result->type = CVAL_IS_INT;

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_exit(co_execute_data *execute_data)
{
    return CO_VM_RETURN;
}

int
co_do_jmpz(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    cval *val1, *val2;
    val1 = get_cval_ptr(&opline->op1, EX(ts));

    if (!val1->u.ival) {
        EX(op) = &EX(op_array)->ops[opline->op2.u.opline_num];
        return CO_VM_CONTINUE;
    }

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_jmp(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    cval *val1;
    val1 = get_cval_ptr(&opline->op1, EX(ts));

    EX(op) = &EX(op_array)->ops[opline->op1.u.opline_num];

    return CO_VM_CONTINUE;
}

int
co_do_declare_function(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    EX(op)++;
    cval *val1;
    val1 = get_cval_ptr(&opline->op1, EX(ts));
    val1->u.ival = opline->op2.u.val.u.ival;
    return CO_VM_CONTINUE;
}

int
co_do_return(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_fcall(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    cval *val1;
    val1 = get_cval_ptr(&opline->op1, EX(ts));
    // jump to funcation start point
    EX(op) = &EX(op_array)->ops[val1->u.ival];
    return CO_VM_CONTINUE;
}
