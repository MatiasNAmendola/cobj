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

COObject *
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
            return *co;
        }
    }
    do {
        COObject *myco;
        myco = CODict_GetItem(current_exec_data->symbol_table, str);
        if (myco) {
            return myco;
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

int
co_vm_handler(void)
{
    struct co_opline *op = EG(current_exec_data)->op;
    COObject *val1, *val2, *result;
    switch (EG(current_exec_data)->op->opcode) {
    case OP_ADD:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) + COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SUB:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) - COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_MUL:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) * COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_DIV:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) / COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_MOD:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) % COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SR:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) >> COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_SL:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COInt_FromLong(COInt_AsLong(val1) << COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_EQUAL:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) == COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_NOT_EQUAL:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) != COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) < COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_IS_SMALLER_OR_EQUAL:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->result, COBool_FromLong(COInt_AsLong(val1) <= COInt_AsLong(val2)));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_ASSIGN:
        val2 = CNode_GetObject(&op->op2);
        CNode_SetObject(&op->op1, val2);
        CNode_SetObject(&op->result, val2);
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_PRINT:
        val1 = CNode_GetObject(&op->op1);
        COObject_print(val1);

        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_JMPZ:
        val1 = CNode_GetObject(&op->op1);

        if (val1 != CO_True && COBool_FromLong(COInt_AsLong(val1)) != CO_True) {
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
        val1 = CNode_GetObject(&op->op1);
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
                val1 = CNode_GetObject(&op->op1);
                val1 = (COObject *)func;
#ifdef CO_DEBUG
                printf("declare function: %p, itsvalue: %p\n", val1, val1);
                val1 = CNode_GetObject(&op->op1);
                printf("declare function: %p, itsvalue: %p\n", val1, val1);
#endif
            }
            if (op->result.type != IS_UNUSED) {
                result =
                    CNode_GetObject(&op->result);
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
                val1 = CNode_GetObject(&op->op1);
                tmp = val1;
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
        val1 = CNode_GetObject(&op->op1);
#ifdef CO_DEBUG
        printf("call function: %p\n", val1);
#endif
        result = CNode_GetObject(&op->result);
        if (CO_TYPE(val1) != &COFunction_Type) {
            error("not a function");
        }
        co_vm_stack_push(result);
        EG(current_exec_data)->op++;
        EG(next_func) = val1;
        return CO_VM_ENTER;
    case OP_PASS_PARAM:
        val1 = CNode_GetObject(&op->op1);
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
        if (!COObject_get(op->op1.u.co)) {
            COObject_put(op->op1.u.co, CO_None);
        }
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_LIST_BUILD:
        CNode_SetObject(&op->result, COList_New(0));
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    case OP_APPEND_ELEMENT:
        val1 = CNode_GetObject(&op->op1);
        val2 = CNode_GetObject(&op->op2);
        COList_Append(val1, val2);
        EG(current_exec_data)->op++;
        return CO_VM_CONTINUE;
    default:
        error("unknown handle for opcode(%ld)\n",
              EG(current_exec_data)->op->opcode);
        return -1;
    }
}

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
