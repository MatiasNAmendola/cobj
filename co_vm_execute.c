#include "co_vm_opcodes.h"
#include "co_vm_execute.h"
#include "co_globals.h"
#include "co_globals_macros.h"
#include "co_parser.h"

#ifdef EX
#undef EX
#endif
#define EX(v)   execute_data->v

#define CO_VM_STACK_PAGE_SIZE (64 * 1024)
#define CO_VM_STACK_GROW_IF_NEEDED(size)                \
    if (size > EG(argument_stack)->end     \
            - EG(argument_stack)->top) {   \
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
        break;
    case OP_ASSIGN:
        return co_do_assign;
        break;
    case OP_PRINT:
        return co_do_print;
        break;
    case OP_JMPZ:
        return co_do_if_cond;
        break;
    case OP_JMP:
        return co_do_if_after_statement;
        break;
    case OP_EXIT:
        return co_do_exit;
        break;
    }
    return NULL;
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
init_executor()
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
co_do_print(co_execute_data *execute_data)
{
    co_op *op = EX(op);

    cval *val1;

    val1 = get_cval_ptr(&op->op1, EX(ts));
    switch (val1->type) {
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
co_do_if_cond(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    cval *val1, *val2;
    val1 = get_cval_ptr(&opline->op1, EX(ts));

    if (!val1->u.ival) {
#if CO_DEBUG >= 1
        printf("Conditional jmp to %d\n", opline->op2.u.opline_num);
#endif
        EX(op) = &EX(op_array)->ops[opline->op2.u.opline_num];
        return CO_VM_CONTINUE;
    }

    EX(op)++;
    return CO_VM_CONTINUE;
}

int
co_do_if_after_statement(co_execute_data *execute_data)
{
    co_op *opline = EX(op);
    cval *val1;
    val1 = get_cval_ptr(&opline->op1, EX(ts));

#if CO_DEBUG >= 1
    printf("Conditional jmp to %d\n", opline->op1.u.opline_num);
#endif
    EX(op) = &EX(op_array)->ops[opline->op1.u.opline_num];

    return CO_VM_CONTINUE;
}
