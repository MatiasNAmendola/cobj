#include "co.h"

struct co_executor_globals {
    COObject *next_func;
    struct co_exec_data *current_exec_data;
    co_stack argument_stack;
};

struct co_executor_globals executor_globals;
#define EG(v)   executor_globals.v

/* Forward declaration */
extern COObject *COObject_get(COObject *co);
extern int COObject_put(COObject *name, COObject *co);
extern int COObject_del(COObject *name);
extern void COObject_print(COObject *co);

COObject *
COObject_get(COObject *name)
{
    COObject *co;

    struct co_exec_data *current_exec_data = EG(current_exec_data);
    if (current_exec_data->function_called) {
        co = CODict_GetItem(((COFunctionObject *)
                             current_exec_data->function_called)->func_upvalues,
                            name);
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
        myco = CODict_GetItem(((COFunctionObject *)
                               current_exec_data->
                               function_called)->func_upvalues, name);
        if (myco) {
            CODict_SetItem(((COFunctionObject *)
                            current_exec_data->function_called)->func_upvalues,
                           name, co);
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
        return COTuple_GetItem(EG(current_exec_data)->co_consts, node->u.var);
        break;
    case IS_VAR:
        co = COTuple_GetItem(EG(current_exec_data)->co_names, node->u.var);
        co = COObject_get(co);
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
        {
            COObject *name = COTuple_GetItem(EG(current_exec_data)->co_names, node->u.var);
            return COObject_put(name, co);
        }
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

#ifdef CO_DEBUG
#define OP_JUMP(offset) \
    do {                \
        EG(current_exec_data)->op += offset - 1;             \
    } while (0)
#else
#define OP_JUMP(offset) \
    do {                \
        EG(current_exec_data)->op += offset - 1;             \
    } while (0)
#endif

void
co_vm_execute(COCodeObject *main)
{
    struct co_exec_data *exec_data;
    COObject *f = COFrame_New();

vm_enter:
    exec_data =
        (struct co_exec_data *)COFrame_Alloc(f, sizeof(struct co_exec_data) +
                                             sizeof(COObject *) *
                                             main->co_numoftmpvars);
    exec_data->ts =
        (COObject **)((char *)exec_data + sizeof(struct co_exec_data));
    exec_data->oplines = main->co_oplines;
    exec_data->op = ((COTupleObject *)main->co_oplines)->co_item;
    exec_data->prev_exec_data = NULL;
    exec_data->symbol_table = CODict_New();
    exec_data->function_called = EG(next_func);
    exec_data->prev_exec_data = EG(current_exec_data);
    exec_data->co_consts = main->co_consts;
    exec_data->co_names = main->co_names;

    EG(current_exec_data) = exec_data;

#ifdef CO_DEBUG
    printf("vm enter: exec_data: %p, prev_exec_data: %p\n",
           EG(current_exec_data), EG(current_exec_data)->prev_exec_data);
#endif

    struct co_opline *op;
    COObject *co1, *co2;
    while (true) {
        switch ((op = *(EG(current_exec_data)->op++))->opcode) {
        case OP_ADD:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) +
                                           COInt_AsLong(co2)));
#ifdef CO_DEBUG
            printf("ADD: %ld\n", COInt_AsLong(CNode_GetObject(&op->result)));
#endif
            continue;
        case OP_SUB:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) -
                                           COInt_AsLong(co2)));
            continue;
        case OP_MUL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) *
                                           COInt_AsLong(co2)));
#ifdef CO_DEBUG
            printf("MUL: %ld\n", COInt_AsLong(CNode_GetObject(&op->result)));
#endif
            continue;
        case OP_DIV:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) /
                                           COInt_AsLong(co2)));
            continue;
        case OP_MOD:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) %
                                           COInt_AsLong(co2)));
            continue;
        case OP_SR:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) >>
                                           COInt_AsLong(co2)));
            continue;
        case OP_SL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) <<
                                           COInt_AsLong(co2)));
            continue;
        case OP_IS_EQUAL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) ==
                                            COInt_AsLong(co2)));
            continue;
        case OP_IS_NOT_EQUAL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) !=
                                            COInt_AsLong(co2)));
            continue;
        case OP_IS_SMALLER:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) <
                                            COInt_AsLong(co2)));
            continue;
        case OP_IS_SMALLER_OR_EQUAL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) <=
                                            COInt_AsLong(co2)));
            continue;
        case OP_ASSIGN:
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->op1, co2);
            CNode_SetObject(&op->result, co2);
            continue;
        case OP_PRINT:
            co1 = CNode_GetObject(&op->op1);
            COObject_print(co1);

            continue;
        case OP_JMPZ:
            co1 = CNode_GetObject(&op->op1);

            if (co1 != CO_True
                && COBool_FromLong(COInt_AsLong(co1)) != CO_True) {
                OP_JUMP(op->op2.u.opline_num);
                continue;
            }

            continue;
        case OP_JMP:
            co1 = CNode_GetObject(&op->op1);
            OP_JUMP(op->op1.u.opline_num);
            continue;
        case OP_EXIT:
            goto vm_exit;
        case OP_DECLARE_FUNCTION:
            {
                COFunctionObject *func =
                    (COFunctionObject *)COFunction_New(COTuple_GetItem(exec_data->co_names, op->op1.u.var));
                uint start = EG(current_exec_data)->op - (struct co_opline **)((COTupleObject *)main->co_oplines)->co_item - 1;
                COTupleObject *suboplines = COTuple_GetSlice(main->co_oplines, start + 1, start + 1 + op->op2.u.opline_num);
                COCodeObject *code = COCode_New(suboplines, NULL, NULL);
                code->co_numoftmpvars = main->co_numoftmpvars;// hack fix, using same temp variables num
                code->co_consts = main->co_consts;
                code->co_names = main->co_names;
                func->func_code = code;
                if (EG(current_exec_data)->function_called) {
                    // setup function's func_upvalues
                    COObject *co;
                    COObject *name;
                    for (int i = 0; i < CO_SIZE(code->co_oplines); i++) {
                        struct co_opline *tmp = COTuple_GET_ITEM(code->co_oplines, i);
                        if (tmp->op1.type == IS_VAR) {
                            name = COTuple_GetItem(exec_data->co_names, tmp->op1.u.var);
                            co = COObject_get(name);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, name, co);
                            }
                        }
                        if (tmp->op2.type == IS_VAR) {
                            name = COTuple_GetItem(exec_data->co_names, tmp->op2.u.var);
                            co = COObject_get(name);
                            if (co) {
                                CODict_SetItem(func->func_upvalues,
                                        name, co);
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
                printf("vm leave: %p, back: %p\n", EG(current_exec_data),
                       EG(current_exec_data)->prev_exec_data);
#endif
                EG(current_exec_data) = EG(current_exec_data)->prev_exec_data;
                COFrame_Free(f, old_exec_data);
                struct cnode *rt = COFrame_Pop(f);
                if (op->op1.type != IS_UNUSED) {
                    CNode_SetObject(rt, co);
                } else {

                }
                continue;
            }
        case OP_DO_FCALL:
            co1 = CNode_GetObject(&op->op1);
#ifdef CO_DEBUG
            printf("call function: %p\n", co1);
#endif
            if (CO_TYPE(co1) != &COFunction_Type) {
                error("not a function");
            }
            COFrame_Push(f, &op->result);
            EG(next_func) = co1;

            main = ((COFunctionObject *)EG(next_func))->func_code;
            goto vm_enter;
        case OP_PASS_PARAM:
            co1 = CNode_GetObject(&op->op1);
            co_stack_push(&EG(argument_stack), &co1, sizeof(COObject *));
            continue;
        case OP_RECV_PARAM:
            {
                COObject **co;
                co_stack_top(&EG(argument_stack), (void **)&co);
                co_stack_pop(&EG(argument_stack));
                COObject_put(COTuple_GetItem(exec_data->co_names, op->op1.u.var), *co);
                continue;
            }
        case OP_LOAD_NAME:
            co1 = COTuple_GetItem(exec_data->co_names, op->op1.u.var);
            co1 = COObject_get(co1);
            if (!co1) {
                CNode_SetObject(&op->op1, CO_None);
            }
            continue;
        case OP_LIST_BUILD:
            CNode_SetObject(&op->result, COList_New(0));
            continue;
        case OP_APPEND_ELEMENT:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            COList_Append(co1, co2);
            continue;
        default:
            error("unknown handle for opcode(%ld)\n", op->opcode);
            return -1;
        }
    }

vm_exit:
    COFrame_Destory(f);
}
