#include "vm_execute.h"
#include "vm_opcodes.h"
#include "compile.h"
#include "parser.h"
#include "error.h"

struct co_executor_globals executor_globals;

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

static struct co_vm_stack *
co_vm_stack_new_page(size_t size)
{
    struct co_vm_stack *page =
        (struct co_vm_stack *)xmalloc(sizeof(struct co_vm_stack) +
                                      sizeof(page->elements[0]) * (size - 1));
    page->top = page->elements;
    page->end = page->elements + size;
    page->prev = NULL;
    return page;
}

static void
co_vm_stack_extend(size_t size)
{
    struct co_vm_stack *p =
        co_vm_stack_new_page(size >=
                             CO_VM_STACK_PAGE_SIZE ? size :
                             CO_VM_STACK_PAGE_SIZE);
    p->prev = EG(vm_stack);
    EG(vm_stack) = p;
}

static void
co_vm_stack_destory()
{
    struct co_vm_stack *stack = EG(vm_stack);

    while (stack != NULL) {
        struct co_vm_stack *p = stack->prev;

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
        struct co_vm_stack *p = EG(vm_stack);

        EG(vm_stack) = p->prev;
        free(p);
    }

    return e;
}

static void *
co_vm_stack_alloc(size_t size)
{
    void *ret;

    // ceil(size / sizeof(void*))
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
        // free if it's current stack frame
        struct co_vm_stack *p = EG(vm_stack);
        EG(vm_stack) = p->prev;
        free(p);
    } else {
        // else only mark it free
        EG(vm_stack)->top = (void **)ptr;
    }
}

struct cval *
cval_get(const char *name)
{
    struct cval *val;

    struct co_exec_data *current_exec_data = EG(current_exec_data);
    if (current_exec_data->function_called) {
        struct cval **upvalue;
        if (co_symtable_find
            (&current_exec_data->function_called->upvalues, name, strlen(name),
             (void **)&upvalue)) {
            return *upvalue;
        }
    }
    do {
        if (co_symtable_find
            (&current_exec_data->symbol_table, name, strlen(name),
             (void **)&val)) {
            return val;
        }
        current_exec_data = current_exec_data->prev_exec_data;
        if (!current_exec_data) {
            break;
        }
    } while (true);
    return NULL;
}

bool
cval_put(const char *name, struct cval * val)
{
    return co_symtable_update(&EG(current_exec_data)->symbol_table, name,
                              strlen(name), val, sizeof(struct cval));
}

bool
cval_del(const char *name)
{
    return co_symtable_del(&EG(current_exec_data)->symbol_table, name,
                           strlen(name));
}

void
cval_print(struct cval *val)
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
    case CVAL_IS_FUNCTION:
        printf("<function>%s\n", val->u.func->name);
        break;
    default:
        error("unknow cval type: %d", val->type);
        break;
    }
}

static void
cval_bind(struct cnode *node)
{
    struct cval *cval;
    cval = cval_get(node->u.val.u.str.val);
    if (!cval) {
        struct cval cvalnew;
        cval_put(node->u.val.u.str.val, &cvalnew);
    }
}

static struct cval *
get_cval_ptr(struct cnode *node, const union temp_variable *ts)
{
    struct cval *cvalptr;

    switch (node->type) {
    case IS_CONST:
        return &node->u.val;
        break;
    case IS_VAR:
        cvalptr = cval_get(node->u.val.u.str.val);
        if (!cvalptr) {
            error("not bind: %s", node->u.val.u.str.val);
        }
        return cvalptr;
        break;
    case IS_TMP_VAR:
        return (struct cval *)(union temp_variable *)((char *)ts + node->u.var);
        break;
    case IS_UNUSED:
        return NULL;
        break;
    }

    return NULL;
}

int
co_vm_handler(void)
{
    struct co_opline *op = EG(current_exec_data)->op;
    struct cval *val1, *val2, *result;
    switch (EG(current_exec_data)->op->opcode) {
    case OP_ADD:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival + val2->u.ival;
        result->type = CVAL_IS_INT;
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SUB:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival - val2->u.ival;
        result->type = CVAL_IS_INT;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_MUL:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival * val2->u.ival;
        result->type = CVAL_IS_INT;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_DIV:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival / val2->u.ival;
        result->type = CVAL_IS_INT;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_MOD:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival % val2->u.ival;
        result->type = CVAL_IS_INT;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_EQUAL:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival == val2->u.ival;
        result->type = CVAL_IS_BOOL;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_NOT_EQUAL:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival != val2->u.ival;
        result->type = CVAL_IS_BOOL;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival < val2->u.ival;
        result->type = CVAL_IS_BOOL;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER_OR_EQUAL:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        result->u.ival = val1->u.ival <= val2->u.ival;
        result->type = CVAL_IS_BOOL;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_ASSIGN:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_cval_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);

        result->u.ival = val1->u.ival = val2->u.ival;
        result->type = val1->type = val2->type;

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_PRINT:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        cval_print(val1);

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_JMPZ:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);

        if (!val1->u.ival) {
            EG(current_exec_data)->op += op->op2.u.opline_num;
#if CO_DEBUG
            printf("JMPZ to: %d\n",
                   EG(current_exec_data)->op -
                   EG(current_exec_data)->opline_array->ops);
#endif
            return CO_VM_CONTINUE;
        }

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_JMP:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        EG(current_exec_data)->op += op->op1.u.opline_num;
#if CO_DEBUG
        printf("JMP to: %d\n",
               EG(current_exec_data)->op -
               EG(current_exec_data)->opline_array->ops);
#endif
        return CO_VM_CONTINUE;
    case OP_EXIT:
        return CO_VM_RETURN;
    case OP_DECLARE_FUNCTION:
        {
            struct Function *func = xmalloc(sizeof(struct Function));
            func->opline_array = xmalloc(sizeof(struct co_opline_array));
            func->opline_array->ops = EG(current_exec_data)->op + 1;
            func->opline_array->last = op->op2.u.opline_num;
            func->opline_array->t = EG(current_exec_data)->opline_array->t;     // hack fix, using same temp variables num
            func->name = op->op1.u.val.u.str.val;
            co_hash_init(&func->upvalues, 1, NULL);
            if (EG(current_exec_data)->function_called) {
                // setup function's upvalues
                struct co_opline *start = EG(current_exec_data)->op;
                struct co_opline *end =
                    EG(current_exec_data)->op + op->op2.u.opline_num;
                char *name;
                struct cval *val;
                for (; start <= end; start++) {
                    if (start->op1.type == IS_VAR) {
                        name = start->op1.u.val.u.str.val;
                        val = cval_get(name);
                        if (val) {
                            co_symtable_update(&func->upvalues, name,
                                               strlen(name), &val,
                                               sizeof(struct cval *));
                        }
                    }
                    if (start->op2.type == IS_VAR) {
                        name = start->op2.u.val.u.str.val;
                        val = cval_get(name);
                        if (val) {
                            co_symtable_update(&func->upvalues, name,
                                               strlen(name), &val,
                                               sizeof(struct cval *));
                        }
                    }
                }
            }
            if (op->op1.type != IS_UNUSED) {
                val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
                val1->u.func = func;
                val1->type = CVAL_IS_FUNCTION;
            }
            if (op->result.type != IS_UNUSED) {
                result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
                result->u.func = func;
                result->type = CVAL_IS_FUNCTION;
            }
            EG(current_exec_data)->op += op->op2.u.opline_num + 1;
#ifdef CO_DEBUG
            printf("declare func jump over to: %d\n",
                   EG(current_exec_data)->op -
                   EG(current_exec_data)->opline_array->ops);
#endif
            return CO_VM_CONTINUE;
        }
    case OP_RETURN:
        {
            struct cval tmp;
            struct co_exec_data *exec_data;
            exec_data = EG(current_exec_data);
            if (op->op1.type != IS_UNUSED) {
                val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
                tmp = *val1;
            }
            EG(current_exec_data) = EG(current_exec_data)->prev_exec_data;
            co_vm_stack_free(exec_data);
            result = co_vm_stack_pop();
            if (op->op1.type != IS_UNUSED) {
                *result = tmp;
            }
            return CO_VM_LEAVE;
        }
    case OP_INIT_FCALL:
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_DO_FCALL:
#ifdef CO_DEBUG
        printf("call\n");
#endif
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        result = get_cval_ptr(&op->result, EG(current_exec_data)->ts);
        if (val1->type != CVAL_IS_FUNCTION) {
            error("not a function");
        }
        co_vm_stack_push(result);
        EG(current_exec_data)->op++;
        EG(next_func) = val1->u.func;
        return CO_VM_ENTER;
    case OP_PASS_PARAM:
        val1 = get_cval_ptr(&op->op1, EG(current_exec_data)->ts);
        co_stack_push(&EG(argument_stack), &val1, sizeof(&val1));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_RECV_PARAM:
        {
            struct cval **val;
            co_stack_top(&EG(argument_stack), (void **)&val);
            cval_put(op->op1.u.val.u.str.val, *val);

            EG(current_exec_data)->op++;
            return CO_VM_CONTINUE;
        }
    case OP_BIND_NAME:
#ifdef CO_DEBUG
        printf("bind name, %s\n", op->op1.u.val.u.str.val);
#endif
        cval_bind(&op->op1);
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    default:
        error("unknown handle for opcode(%d)\n",
              EG(current_exec_data)->op->opcode);
        return -1;
    }
}

void
co_vm_execute(struct co_opline_array *opline_array)
{
    struct co_exec_data *exec_data;

vm_enter:
    /* initialize exec_data */
#ifdef CO_DEBUG
    printf("tmp: %d\n", opline_array->t);
#endif
    exec_data =
        (struct co_exec_data *)co_vm_stack_alloc(sizeof(struct co_exec_data) +
                                                 sizeof(union temp_variable) *
                                                 opline_array->t);
    exec_data->ts =
        (union temp_variable *)((char *)exec_data +
                                sizeof(struct co_exec_data));
    exec_data->opline_array = opline_array;
    exec_data->op = opline_array->ops;
    exec_data->prev_exec_data = NULL;
    co_hash_init(&exec_data->symbol_table, 2, NULL);
    exec_data->function_called = EG(next_func);

    exec_data->prev_exec_data = EG(current_exec_data);
    EG(current_exec_data) = exec_data;

#ifdef CO_DEBUG
    printf("vm enter: %p\n", &EG(current_exec_data)->symbol_table);
#endif
    while (true) {
        switch (co_vm_handler()) {
        case CO_VM_CONTINUE:
            continue;
        case CO_VM_RETURN:
            return;
        case CO_VM_ENTER:
            opline_array = EG(next_func)->opline_array;
            goto vm_enter;
        case CO_VM_LEAVE:
#ifdef CO_DEBUG
            printf("vm leave: %p\n", &EG(current_exec_data)->symbol_table);
#endif
            continue;
        default:
            break;
        }
    }
}

void
co_vm_init()
{
    EG(vm_stack) = co_vm_stack_new_page(CO_VM_STACK_PAGE_SIZE);
}
