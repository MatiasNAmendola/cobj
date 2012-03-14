#include "co.h"

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

COObject **
COObject_get(COObject *str)
{
    COObject **co;
    char *name = COStr_AsString(str);

    struct co_exec_data *current_exec_data = EG(current_exec_data);
    if (current_exec_data->function_called) {
        if (co_symtable_find
            (&
             ((COFunctionObject *)current_exec_data->function_called)->upvalues,
             name, strlen(name), (void **)&co)) {
            return co;
        }
    }
    do {
        if (co_symtable_find
            (&current_exec_data->symbol_table, name, strlen(name),
             (void **)&co)) {
#ifdef CO_DEBUG
            printf("get: %s, value: %p, data:", COStr_AsString(str), co);
            COObject_print(*co);
#endif
            return co;
        }
        current_exec_data = current_exec_data->prev_exec_data;
        if (!current_exec_data) {
            break;
        }
    } while (true);
#ifdef CO_DEBUG
    printf("get: %s, value: null, data: null\n", COStr_AsString(str));
#endif
    return NULL;
}

bool
COObject_put(COObject *name, COObject *co)
{
#ifdef CO_DEBUG
    printf("put: %s, value: %p, data: ", COStr_AsString(name), co);
    COObject_print(co);
#endif
    return co_symtable_update(&EG(current_exec_data)->symbol_table,
                              COStr_AsString(name),
                              strlen(COStr_AsString(name)), &co,
                              sizeof(COObject *));
}

bool
COObject_del(const char *name)
{
    return co_symtable_del(&EG(current_exec_data)->symbol_table, name,
                           strlen(name));
}

void
COObject_print(COObject *co)
{
    COStrObject *s = (COStrObject *)CO_TYPE(co)->tp_repr(co);
    printf("%s\n", s->co_sval);
}

static void
COObject_bind(COObject *name)
{
    COObject **co;
    co = COObject_get(name);
    if (!co) {
        COObject_put(name, CO_None);
        COObject_get(name);
    }
}

static COObject **
get_COObject_ptr(struct cnode *node, const union temp_variable *ts)
{
    COObject **COObjectptr;

    switch (node->type) {
    case IS_CONST:
        return &node->u.co;
        break;
    case IS_VAR:
        COObjectptr = COObject_get(node->u.co);
        if (!COObjectptr) {
            error("not bind: %s", COStr_AsString(node->u.co));
        }
        return COObjectptr;
        break;
    case IS_TMP_VAR:
        return (COObject **)(union temp_variable *)((char *)ts + node->u.var);
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
    COObject **val1, **val2, **result;
    switch (EG(current_exec_data)->op->opcode) {
    case OP_ADD:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) + COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SUB:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) - COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_MUL:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) * COInt_AsLong(*val2));

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_DIV:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) / COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_MOD:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) % COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SR:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) >> COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SL:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COInt_FromLong(COInt_AsLong(*val1) << COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_EQUAL:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COBool_FromLong(COInt_AsLong(*val1) == COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_NOT_EQUAL:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COBool_FromLong(COInt_AsLong(*val1) != COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COBool_FromLong(COInt_AsLong(*val1) < COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER_OR_EQUAL:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = COBool_FromLong(COInt_AsLong(*val1) <= COInt_AsLong(*val2));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_ASSIGN:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        val2 = get_COObject_ptr(&op->op2, EG(current_exec_data)->ts);
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        *result = *val1 = *val2;
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_PRINT:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        COObject_print(*val1);

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_JMPZ:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);

        if (*val1 != CO_True && COBool_FromLong(COInt_AsLong(*val1)) != CO_True) {
            EG(current_exec_data)->op += op->op2.u.opline_num;
#if CO_DEBUG
            printf("JMPZ to: %ld\n",
                   EG(current_exec_data)->op -
                   EG(current_exec_data)->opline_array->ops);
#endif
            return CO_VM_CONTINUE;
        }

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_JMP:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        EG(current_exec_data)->op += op->op1.u.opline_num;
#if CO_DEBUG
        printf("JMP to: %ld\n",
               EG(current_exec_data)->op -
               EG(current_exec_data)->opline_array->ops);
#endif
        return CO_VM_CONTINUE;
    case OP_EXIT:
        return CO_VM_RETURN;
    case OP_DECLARE_FUNCTION:
        {
            COFunctionObject *func =
                (COFunctionObject *)COFunctionObject_New(op->op1.u.co);
            func->opline_array = xmalloc(sizeof(struct co_opline_array));
            func->opline_array->ops = EG(current_exec_data)->op + 1;
            func->opline_array->last = op->op2.u.opline_num;
            func->opline_array->t = EG(current_exec_data)->opline_array->t;     // hack fix, using same temp variables num
            co_hash_init(&func->upvalues, 1, NULL);
            if (EG(current_exec_data)->function_called) {
                // setup function's upvalues
                struct co_opline *start = EG(current_exec_data)->op;
                struct co_opline *end =
                    EG(current_exec_data)->op + op->op2.u.opline_num;
                COObject **val;
                for (; start <= end; start++) {
                    if (start->op1.type == IS_VAR) {
                        val = COObject_get(start->op1.u.co);
                        if (val) {
                            co_symtable_update(&func->upvalues,
                                               COStr_AsString(start->op1.u.co),
                                               strlen(COStr_AsString
                                                      (start->op1.u.co)), val,
                                               sizeof(COObject *));
                        }
                    }
                    if (start->op2.type == IS_VAR) {
                        val = COObject_get(start->op2.u.co);
                        if (val) {
                            co_symtable_update(&func->upvalues,
                                               COStr_AsString(start->op2.u.co),
                                               strlen(COStr_AsString
                                                      (start->op2.u.co)), val,
                                               sizeof(COObject *));
                        }
                    }
                }
            }
            if (op->op1.type != IS_UNUSED) {
                val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
                *val1 = (COObject *)func;
#ifdef CO_DEBUG
                printf("declare function: %p, itsvalue: %p\n", val1, *val1);
                val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
                printf("declare function: %p, itsvalue: %p\n", val1, *val1);
#endif
            }
            if (op->result.type != IS_UNUSED) {
                result =
                    get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
                *result = (COObject *)func;
            }
            EG(current_exec_data)->op += op->op2.u.opline_num + 1;
#ifdef CO_DEBUG
            printf("declare func jump over to: %ld\n",
                   EG(current_exec_data)->op -
                   EG(current_exec_data)->opline_array->ops);
#endif
            return CO_VM_CONTINUE;
        }
    case OP_RETURN:
        {
            COObject *tmp;
            struct co_exec_data *exec_data;
            exec_data = EG(current_exec_data);
            if (op->op1.type != IS_UNUSED) {
                val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
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
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
#ifdef CO_DEBUG
        printf("call function: %p\n", *val1);
#endif
        result = get_COObject_ptr(&op->result, EG(current_exec_data)->ts);
        if (CO_TYPE(*val1) != &COFunction_Type) {
            error("not a function");
        }
        co_vm_stack_push(result);
        EG(current_exec_data)->op++;
        EG(next_func) = *val1;
        return CO_VM_ENTER;
    case OP_PASS_PARAM:
        val1 = get_COObject_ptr(&op->op1, EG(current_exec_data)->ts);
        co_stack_push(&EG(argument_stack), val1, sizeof(COObject *));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_RECV_PARAM:
        {
            COObject **val;
            co_stack_top(&EG(argument_stack), (void **)&val);
            co_stack_pop(&EG(argument_stack));
            COObject_put(op->op1.u.co, *val);
            EG(current_exec_data)->op++;
            return CO_VM_CONTINUE;
        }
    case OP_BIND_NAME:
        COObject_bind(op->op1.u.co);
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    default:
        error("unknown handle for opcode(%ld)\n",
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
    printf("tmp: %ld\n", opline_array->t);
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
            opline_array = ((COFunctionObject *)EG(next_func))->opline_array;
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

void
co_vm_shutdown()
{
    co_vm_stack_destory();
}
