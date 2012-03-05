#include "vm_execute.h"
#include "vm_opcodes.h"
#include "compile.h"
#include "parser.h"
#include "error.h"

co_executor_globals executor_globals;

#define EX(v)   execute_data->v

#define CO_VM_CONTINUE 0
#define CO_VM_RETURN   1
#define CO_VM_ENTER    2
#define CO_VM_LEAVE    3

#define CO_VM_STACK_PAGE_SIZE (64 * 1024)
#define CO_VM_STACK_GROW_IF_NEEDED(size)                \
    if (size > EG(vm_stack)->end                  \
            - EG(vm_stack)->top) {                \
            co_vm_stack_extend(size);                   \
    }

static co_vm_stack *
co_vm_stack_new_page(size_t size)
{
    co_vm_stack *page =
        (co_vm_stack *)xmalloc(sizeof(co_vm_stack) + sizeof(page->elements[0]) * (size - 1));
    page->top = page->elements;
    page->end = page->elements + size;
    page->prev = NULL;
    return page;
}

static void
co_vm_stack_extend(size_t size)
{
    co_vm_stack *p =
        co_vm_stack_new_page(size >= CO_VM_STACK_PAGE_SIZE ? size : CO_VM_STACK_PAGE_SIZE);
    p->prev = EG(vm_stack);
    EG(vm_stack) = p;
}

static void
co_vm_stack_destory()
{
    co_vm_stack *stack = EG(vm_stack);

    while (stack != NULL) {
        co_vm_stack *p = stack->prev;

        free(stack);
        stack = p;
    }
}

static void
co_vm_stack_push(void *ptr)
{
    CO_VM_STACK_GROW_IF_NEEDED(1);
    *(EG(vm_stack)->top++) = ptr;
}

static void *
co_vm_stack_pop()
{
    void *e = *(--EG(vm_stack)->top);

    if (EG(vm_stack)->top == EG(vm_stack)->elements) {
        co_vm_stack *p = EG(vm_stack);

        EG(vm_stack) = p->prev;
        free(p);
    }

    return e;
}

static void *
co_vm_stack_alloc(size_t size)
{
    void *ret;

    size = (size + (sizeof(void *) - 1)) / sizeof(void *);

    CO_VM_STACK_GROW_IF_NEEDED(size);

    ret = (void *)EG(vm_stack)->top;
    EG(vm_stack)->top += size;
    return ret;
}

static void
co_vm_stack_free(void *ptr)
{
    if ((void **)EG(vm_stack) == (void **)ptr) {
        co_vm_stack *p = EG(vm_stack);
        EG(vm_stack) = p->prev;
        free(p);
    } else {
        EG(vm_stack)->top = (void **)ptr;
    }
}

cval *
cval_get(const char *name)
{
    cval *val;

    if (co_symtable_find(&EG(variable_symboltable), name, strlen(name), (void **)&val)) {
        return val;
    } else {
        return NULL;
    }
}

bool
cval_put(const char *name, cval *val)
{
    return co_symtable_update(&EG(variable_symboltable), name, strlen(name), val, sizeof(cval));
}

bool
cval_del(const char *name)
{
    return co_symtable_del(&EG(variable_symboltable), name, strlen(name));
}

void
cval_print(cval *val)
{
    switch (val->type) {
    case CVAL_IS_NONE:
        printf("%s\n", "None");
        break;
    case CVAL_IS_BOOL:
        if (val->u.ival) {
            printf("%s\n", "True");
        } else {
            printf("%s\n", "False");
        }
        break;
    case CVAL_IS_INT:
        printf("%ld\n", val->u.ival);
        break;
    case CVAL_IS_FLOAT:
        printf("%g\n", val->u.fval);
        break;
    case CVAL_IS_STRING:
        printf("%s\n", val->u.str.val);
        break;
    default:
        co_throw(EG(exception_buf), 1);
    }
}

static cval *
get_cval_ptr(cnode *node, const temp_variable *ts)
{
    cval *cvalptr;

    switch (node->op_type) {
    case IS_CONST:
        return &node->u.val;
        break;
    case IS_VAR:
        cvalptr = cval_get(node->u.val.u.str.val);
        if (!cvalptr) {
            cval cvalnew;

            cval_put(node->u.val.u.str.val, &cvalnew);
            cvalptr = cval_get(node->u.val.u.str.val);
        }
        return cvalptr;
        break;
    case IS_TMP_VAR:
        return (temp_variable *)((char*)ts + node->u.var);
        break;
    case IS_UNUSED:
        return NULL;
        break;
    }

    return NULL;
}

int
co_vm_handler(int opcode, co_execute_data *execute_data)
{
    co_opline *op = EX(op);
    cval *val1, *val2, *result;
    switch (opcode) {
    case OP_ADD:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));
        result->u.ival = val1->u.ival + val2->u.ival;
        result->type = CVAL_IS_INT;
        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_SUB:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));
        result->u.ival = val1->u.ival - val2->u.ival;
        result->type = CVAL_IS_INT;

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_MUL:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));
        result->u.ival = val1->u.ival * val2->u.ival;
        result->type = CVAL_IS_INT;

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_DIV:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));
        result->u.ival = val1->u.ival / val2->u.ival;
        result->type = CVAL_IS_INT;

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_MOD:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));
        result->u.ival = val1->u.ival % val2->u.ival;
        result->type = CVAL_IS_INT;

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));
        result->u.ival = val1->u.ival < val2->u.ival;
        result->type = CVAL_IS_BOOL;

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_ASSIGN:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        val2 = get_cval_ptr(&op->op2, EX(ts));
        result = get_cval_ptr(&op->result, EX(ts));

        result->u.ival = val1->u.ival = val2->u.ival;
        result->type = val1->type = val2->type;

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_PRINT:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        cval_print(val1);

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_JMPZ:
        val1 = get_cval_ptr(&op->op1, EX(ts));

        if (!val1->u.ival) {
            EX(op) = &EX(op_array)->ops[op->op2.u.opline_num];
            return CO_VM_CONTINUE;
        }

        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_JMP:
        val1 = get_cval_ptr(&op->op1, EX(ts));

        EX(op) = &EX(op_array)->ops[op->op1.u.opline_num];

        return CO_VM_CONTINUE;
    case OP_EXIT:
        return CO_VM_RETURN;
    case OP_DECLARE_FUNCTION:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        Function *func = xmalloc(sizeof(Function));
        func->op_array = xmalloc(sizeof(co_opline_array));
        func->op_array->start_op = NULL;
        func->op_array->ops = EX(op) + 1;
        func->op_array->last = op->op2.u.opline_num;
        val1->u.func = func;
        val1->type = CVAL_IS_FUNCTION;
        EX(op) += op->op2.u.opline_num + 1;
        return CO_VM_CONTINUE;
    case OP_RETURN:
        EG(current_execute_data) = EX(prev_execute_data);
        co_vm_stack_free(execute_data);
        execute_data = EG(current_execute_data);
        return CO_VM_LEAVE;
    case OP_INIT_FCALL:
        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_DO_FCALL:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        if (val1->type != CVAL_IS_FUNCTION) {
            coerror("not a function");
        }
        EX(op)++;
        EG(active_op_array) = val1->u.func->op_array;
        return CO_VM_ENTER;
    case OP_PASS_PARAM:
        val1 = get_cval_ptr(&op->op1, EX(ts));
        co_stack_push(&EG(argument_stack), &val1, sizeof(cval *));
        EX(op)++;
        return CO_VM_CONTINUE;
    case OP_RECV_PARAM:
        do {
            cval **val;
            co_stack_top(&EG(argument_stack), (void **)&val);
            cval_put(op->op1.u.val.u.str.val, *val);

            EX(op)++;
            return CO_VM_CONTINUE;
        } while (false);
    default:
        coerror("unknown handle for opcode(%d)\n", opcode);
        return -1;
    }
}

void
co_vm_execute(co_opline_array *op_array)
{
    co_execute_data *execute_data;

vm_enter:
    /* initialize execute_data */
    execute_data =
        (co_execute_data *)co_vm_stack_alloc(sizeof(co_execute_data) +
                                             sizeof(temp_variable) * op_array->t);
    EX(ts) = (temp_variable *)((char *)execute_data + sizeof(co_execute_data));
    EX(op_array) = op_array;

    EX(prev_execute_data) = EG(current_execute_data);
    EG(current_execute_data) = execute_data;

    if (op_array->start_op) {
        EX(op) = op_array->start_op;
    } else {
        EX(op) = op_array->ops;
    }

    while (true) {
        switch (co_vm_handler(EX(op)->opcode, execute_data)) {
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
}

void
co_vm_init()
{
    EG(vm_stack) = co_vm_stack_new_page(CO_VM_STACK_PAGE_SIZE);
    co_hash_init(&EG(variable_symboltable), 2, NULL);
    EG(exception_buf) = NULL;
}
