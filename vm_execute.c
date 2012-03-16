#include "co.h"

struct co_vm_stack {
    void **top;
    void **end;
    struct co_vm_stack *prev;
    void *elements[1];
};

struct co_executor_globals {
    COObject *next_func;
    struct co_exec_data *current_exec_data;
    struct co_vm_stack *vm_stack;
    co_stack argument_stack;
};

struct co_executor_globals executor_globals;
#define EG(v)   executor_globals.v

/* Forward declaration */
extern COObject *COObject_get(COObject *co);
extern int COObject_put(COObject *name, COObject *co);
extern int COObject_del(COObject *name);
extern void COObject_print(COObject *co);

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

COObject *
COObject_get(COObject *name)
{
    COObject *co;

    struct co_exec_data *current_exec_data = EG(current_exec_data);
    if (current_exec_data->function_called) {
        co = CODict_GetItem(((COFunctionObject *)current_exec_data->function_called)->func_upvalues, name);
        if (co) {
            return co;
        }
    }
    do {
        co = CODict_GetItem(current_exec_data->symbol_table, name);
        if (co) {
            return co;
        }
        current_exec_data = current_exec_data->prev_exec_data;
        if (!current_exec_data) {
            break;
        }
    } while (true);
    return NULL;
}

int
COObject_put(COObject *name, COObject *co)
{
    struct co_exec_data *current_exec_data = EG(current_exec_data);
    if (current_exec_data->function_called) {
        COObject *myco;
        myco = CODict_GetItem(((COFunctionObject *)current_exec_data->function_called)->func_upvalues, name);
        if (myco) {
            CODict_SetItem(((COFunctionObject *)current_exec_data->function_called)->func_upvalues, name, co);
        }
    }
    return CODict_SetItem(EG(current_exec_data)->symbol_table, name, co);
}

int
COObject_del(COObject *name)
{
    return CODict_DelItem(EG(current_exec_data)->symbol_table, name);
}

void
COObject_print(COObject *co)
{
    COStrObject *s = (COStrObject *)CO_TYPE(co)->tp_repr(co);
    printf("%s\n", s->co_sval);
}

static COObject *
CNode_GetObject(struct cnode *node)
{
    COObject *co;
    switch (node->type) {
    case IS_CONST:
        return node->u.co;
        break;
    case IS_VAR:
        co = COObject_get(node->u.co);
        if (!co) {
            error("not bind: %s", COStr_AsString(node->u.co));
        }
        return co;
        break;
    case IS_TMP_VAR:
        return *(COObject **)((char *)EG(current_exec_data)->ts + node->u.var);
        break;
    case IS_UNUSED:
        return NULL;
        break;
    }

    return NULL;
}

static int
CNode_SetObject(struct cnode *node, COObject *co)
{
    switch (node->type) {
    case IS_CONST:
        error("you cannot change const");
        break;
    case IS_VAR:
        return COObject_put(node->u.co, co);
        break;
    case IS_TMP_VAR:
        *(COObject **)((char *)EG(current_exec_data)->ts + node->u.var) = co;
        return 0;
        break;
    case IS_UNUSED:
        error("it's unused cnode");
        return -1;
        break;
    }

    return -1;
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

#define OP_JUMP(offset) EG(current_exec_data)->op += offset - 1

void
co_vm_execute(COFunctionObject *main)
{
    struct co_exec_data *exec_data;
    struct co_opline_array *opline_array;
    opline_array = main->opline_array;

vm_enter:
    exec_data =
        (struct co_exec_data *)co_vm_stack_alloc(sizeof(struct co_exec_data) +
                                                 sizeof(COObject *) *
                                                 opline_array->t);
    exec_data->ts =
        (COObject **)((char *)exec_data +
                                sizeof(struct co_exec_data));
    exec_data->opline_array = opline_array;
    exec_data->op = opline_array->ops;
    exec_data->prev_exec_data = NULL;
    exec_data->symbol_table = CODict_New();
    exec_data->function_called = EG(next_func);

    exec_data->prev_exec_data = EG(current_exec_data);
    EG(current_exec_data) = exec_data;

#ifdef CO_DEBUG
    printf("vm enter: exec_data: %p, prev_exec_data: %p\n", EG(current_exec_data), EG(current_exec_data)->prev_exec_data);
#endif

    struct co_opline *op;
    COObject *val1, *val2;
    while (true) {
        switch ((op = EG(current_exec_data)->op++)->opcode) {
        case OP_ADD:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) + COInt_AsLong(val2)));
            continue;
        case OP_SUB:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) - COInt_AsLong(val2)));
            continue;
        case OP_MUL:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) * COInt_AsLong(val2)));
            continue;
        case OP_DIV:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) / COInt_AsLong(val2)));
            continue;
        case OP_MOD:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) % COInt_AsLong(val2)));
            continue;
        case OP_SR:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) >> COInt_AsLong(val2)));
            continue;
        case OP_SL:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) << COInt_AsLong(val2)));
            continue;
        case OP_IS_EQUAL:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) == COInt_AsLong(val2)));
            continue;
        case OP_IS_NOT_EQUAL:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) != COInt_AsLong(val2)));
            continue;
        case OP_IS_SMALLER:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) < COInt_AsLong(val2)));
            continue;
        case OP_IS_SMALLER_OR_EQUAL:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) <= COInt_AsLong(val2)));
            continue;
        case OP_ASSIGN:
            val2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->op1, val2);
            CNode_SetObject(&op->result, val2);
            continue;
        case OP_PRINT:
            val1 = CNode_GetObject(&op->op1);
            COObject_print(val1);

            continue;
        case OP_JMPZ:
            val1 = CNode_GetObject(&op->op1);

            if (val1 != CO_True && COBool_FromLong(COInt_AsLong(val1)) != CO_True) {
                OP_JUMP(op->op2.u.opline_num);
#if CO_DEBUG
                printf("JMPZ to: %ld\n",
                       op -
                       opline_array->ops);
#endif
                continue;
            }

            continue;
        case OP_JMP:
            val1 = CNode_GetObject(&op->op1);
            OP_JUMP(op->op1.u.opline_num);
#if CO_DEBUG
            printf("JMP to: %ld\n",
                   op -
                   opline_array->ops);
#endif
            continue;
        case OP_EXIT:
            return;
        case OP_DECLARE_FUNCTION:
            {
                COFunctionObject *func =
                    (COFunctionObject *)COFunctionObject_New(op->op1.u.co);
                func->opline_array = xmalloc(sizeof(struct co_opline_array));
                func->opline_array->ops = op + 1;
                func->opline_array->last = op->op2.u.opline_num;
                func->opline_array->t = opline_array->t;     // hack fix, using same temp variables num
                if (EG(current_exec_data)->function_called) {
                    // setup function's func_upvalues
                    struct co_opline *start = op;
                    struct co_opline *end =
                        op + op->op2.u.opline_num;

                    COObject *co;
                    for (; start <= end; start++) {
                        if (start->op1.type == IS_VAR) {
                            co = COObject_get(start->op1.u.co);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, start->op1.u.co, co);
                            }
                        }
                        if (start->op2.type == IS_VAR) {
                            co = COObject_get(start->op2.u.co);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, start->op2.u.co, co);
                            }
                        }
                    }
                }
                if (op->op1.type != IS_UNUSED) {
                    CNode_SetObject(&op->op1, (COObject *)func);
                }
                if (op->result.type != IS_UNUSED) {
                    CNode_SetObject(&op->result, (COObject *)func);
                }
                OP_JUMP(op->op2.u.opline_num + 1);
#ifdef CO_DEBUG
                printf("declare func jump over to: %ld\n",
                        op -
                        opline_array->ops);
#endif
                continue;
            }
        case OP_RETURN:
            {
                COObject *co;
                struct co_exec_data *old_exec_data;
                old_exec_data = EG(current_exec_data);
                if (op->op1.type != IS_UNUSED) {
                    co = CNode_GetObject(&op->op1);
                }

#ifdef CO_DEBUG
                printf("vm leave: %p, back: %p\n", EG(current_exec_data), EG(current_exec_data)->prev_exec_data);
#endif
                EG(current_exec_data) = EG(current_exec_data)->prev_exec_data;
                co_vm_stack_free(old_exec_data);
                struct cnode *rt = co_vm_stack_pop();
                if (op->op1.type != IS_UNUSED) {
                    CNode_SetObject(rt, co);
                } else {

                }
                continue;
            }
        case OP_DO_FCALL:
            val1 = CNode_GetObject(&op->op1);
#ifdef CO_DEBUG
            printf("call function: %p\n", val1);
#endif
            if (CO_TYPE(val1) != &COFunction_Type) {
                error("not a function");
            }
            co_vm_stack_push(&op->result);
            EG(next_func) = val1;
            opline_array = ((COFunctionObject *)EG(next_func))->opline_array;
            goto vm_enter;
        case OP_PASS_PARAM:
            val1 = CNode_GetObject(&op->op1);
            co_stack_push(&EG(argument_stack), &val1, sizeof(COObject *));
            continue;
        case OP_RECV_PARAM:
            {
                COObject **co;
                co_stack_top(&EG(argument_stack), (void **)&co);
                co_stack_pop(&EG(argument_stack));
                COObject_put(op->op1.u.co, *co);
                continue;
            }
        case OP_LOAD_NAME:
            if (!COObject_get(op->op1.u.co)) {
                COObject_put(op->op1.u.co, CO_None);
            }
            continue;
        case OP_LIST_BUILD:
            CNode_SetObject(&op->result, COList_New(0));
            continue;
        case OP_APPEND_ELEMENT:
            val1 = CNode_GetObject(&op->op1);
            val2 = CNode_GetObject(&op->op2);
            COList_Append(val1, val2);
            continue;
        default:
            error("unknown handle for opcode(%ld)\n",
                  op->opcode);
            return -1;
        }
    }
}
