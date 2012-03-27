#include "co.h"

/* status code for eval main loop (reason for stack unwind) */
enum status_code {
    STATUS_NONE = 0x0001,       /* None */
    STATUS_EXCEPTION = 0x0002,  /* Exception occurred */
};

/* execution frame */
struct co_exec_data {
    COOplineObject **op;
    struct co_exec_data *prev_exec_data;
    COObject *function_called;
    COObject *symbol_table;     /* dict object for names */
    COObject **ts;              /* temp objects */
};

/* Forward declaration */
COObject *COObject_get(COObject *co);
int COObject_put(COObject *name, COObject *co);
int COObject_del(COObject *name);
void COObject_print(COObject *co);

COObject *
COObject_get(COObject *name)
{
    COObject *co;

    struct co_exec_data *current_exec_data = TS(current_exec_data);
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
    struct co_exec_data *current_exec_data = TS(current_exec_data);
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
    return CODict_SetItem(TS(current_exec_data)->symbol_table, name, co);
}

int
COObject_del(COObject *name)
{
    return CODict_DelItem(TS(current_exec_data)->symbol_table, name);
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
            COErr_Format(COException_NameError, "name '%s' is not defined",
                         COStr_AsString(node->u.co));
        }
        return co;
        break;
    case IS_TMP_VAR:
        return *(COObject **)((char *)TS(current_exec_data)->ts + node->u.var);
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
        *(COObject **)((char *)TS(current_exec_data)->ts + node->u.var) = co;
        return 0;
        break;
    case IS_UNUSED:
        error("it's unused cnode");
        return -1;
        break;
    }

    return -1;
}

#define OP_JUMP(offset) \
    do {                \
        TS(current_exec_data)->op += offset - 1;             \
    } while (0)

COObject *
co_vm_eval(COObject *_mainfunc)
{
    COObject *ret = NULL;
    struct co_exec_data *exec_data;
    COObject *f = TS(frame);
    COCodeObject *mainfunc = (COCodeObject *)_mainfunc;

vm_enter:
    exec_data =
        (struct co_exec_data *)COFrame_Alloc(f, sizeof(struct co_exec_data) +
                                             sizeof(COObject *) *
                                             mainfunc->co_numoftmpvars);
    exec_data->ts =
        (COObject **)((char *)exec_data + sizeof(struct co_exec_data));
    exec_data->op =
        (COOplineObject **)((COTupleObject *)mainfunc->co_oplines)->co_item;
    exec_data->prev_exec_data = NULL;
    exec_data->symbol_table = CODict_New();
    exec_data->function_called = TS(next_func);
    exec_data->prev_exec_data = TS(current_exec_data);

    TS(current_exec_data) = exec_data;

#ifdef CO_DEBUG
    printf("vm enter: exec_data: %p, prev_exec_data: %p\n",
           TS(current_exec_data), TS(current_exec_data)->prev_exec_data);
#endif

    int status = STATUS_NONE;
    COOplineObject *op;
    COObject *co1, *co2, *co3;
    for (;;) {
        op = *(TS(current_exec_data)->op++);
        switch (op->opcode) {
        case OP_ADD:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) +
                                           COInt_AsLong(co2)));
            break;
        case OP_SUB:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) -
                                           COInt_AsLong(co2)));
            break;
        case OP_MUL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) *
                                           COInt_AsLong(co2)));
            break;
        case OP_DIV:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) /
                                           COInt_AsLong(co2)));
            break;
        case OP_MOD:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) %
                                           COInt_AsLong(co2)));
            break;
        case OP_SR:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) >>
                                           COInt_AsLong(co2)));
            break;
        case OP_SL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(co1) <<
                                           COInt_AsLong(co2)));
            break;
        case OP_IS_EQUAL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) ==
                                            COInt_AsLong(co2)));
            break;
        case OP_IS_NOT_EQUAL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) !=
                                            COInt_AsLong(co2)));
            break;
        case OP_IS_SMALLER:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) <
                                            COInt_AsLong(co2)));
            break;
        case OP_IS_SMALLER_OR_EQUAL:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(co1) <=
                                            COInt_AsLong(co2)));
            break;
        case OP_ASSIGN:
            co2 = CNode_GetObject(&op->op2);
            CNode_SetObject(&op->op1, co2);
            CNode_SetObject(&op->result, co2);
            break;
        case OP_PRINT:
            co1 = CNode_GetObject(&op->op1);
            if (co1 == NULL) {
                goto on_error;
            }
            COObject_print(co1);
            break;
        case OP_JMPZ:
            co1 = CNode_GetObject(&op->op1);

            if (co1 != CO_True && COBool_FromLong(COInt_AsLong(co1)) != CO_True) {
                OP_JUMP(op->op2.u.opline_num);
            }

            break;
        case OP_JMP:
            co1 = CNode_GetObject(&op->op1);
            OP_JUMP(op->op1.u.opline_num);
            break;
        case OP_DECLARE_FUNCTION:
            {
                COFunctionObject *func =
                    (COFunctionObject *)COFunction_New(op->op1.u.co);
                uint start =
                    TS(current_exec_data)->op -
                    (COOplineObject **)((COTupleObject *)mainfunc->
                                        co_oplines)->co_item - 1;
                COObject *suboplines =
                    COTuple_GetSlice(mainfunc->co_oplines, start + 1,
                                     start + 1 + op->op2.u.opline_num);
                // hack fix, using same temp variables num
                COObject *code =
                    COCode_New(suboplines, mainfunc->co_numoftmpvars);
                func->func_code = code;
                if (TS(current_exec_data)->function_called) {
                    // setup function's func_upvalues
                    COObject *co;
                    COObject *name;
                    for (int i = 0;
                         i < CO_SIZE(((COCodeObject *)code)->co_oplines); i++) {
                        COOplineObject *tmp = (COOplineObject *)
                            COTuple_GET_ITEM(((COCodeObject *)code)->co_oplines,
                                             i);
                        if (tmp->op1.type == IS_VAR) {
                            name = tmp->op1.u.co;
                            co = COObject_get(name);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, name, co);
                            }
                        }
                        if (tmp->op2.type == IS_VAR) {
                            name = tmp->op2.u.co;
                            co = COObject_get(name);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, name, co);
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
                break;
            }
        case OP_RETURN:
            {
                COObject *co;
                struct co_exec_data *old_exec_data;
                old_exec_data = TS(current_exec_data);
                if (op->op1.type != IS_UNUSED) {
                    co = CNode_GetObject(&op->op1);
                }
#ifdef CO_DEBUG
                printf("vm leave: %p, back: %p\n", TS(current_exec_data),
                       TS(current_exec_data)->prev_exec_data);
#endif
                TS(current_exec_data) = TS(current_exec_data)->prev_exec_data;
                COFrame_Free(f, (COObject *)old_exec_data);
                struct cnode *rt = (struct cnode *)COFrame_Pop(f);
                if (op->op1.type != IS_UNUSED) {
                    CNode_SetObject(rt, co);
                } else {

                }
                if (TS(current_exec_data) == NULL) {
                    goto vm_exit;
                }
                break;
            }
        case OP_DO_FCALL:
            co1 = CNode_GetObject(&op->op1);
#ifdef CO_DEBUG
            printf("call function: %p\n", co1);
#endif
            if (CO_TYPE(co1) != &COFunction_Type) {
                error("not a function");
            }
            COFrame_Push(f, (COObject *)&op->result);
            TS(next_func) = co1;

            mainfunc =
                (COCodeObject *)((COFunctionObject *)TS(next_func))->func_code;
            goto vm_enter;
        case OP_PASS_PARAM:
            co1 = CNode_GetObject(&op->op1);
            co_stack_push(&TS(argument_stack), &co1, sizeof(COObject *));
            break;
        case OP_RECV_PARAM:
            {
                COObject **co;
                co_stack_top(&TS(argument_stack), (void **)&co);
                co_stack_pop(&TS(argument_stack));
                COObject_put(op->op1.u.co, *co);
                break;
            }
        case OP_LOAD_NAME:
            co1 = COObject_get(op->op1.u.co);
            if (!co1) {
                CNode_SetObject(&op->op1, CO_None);
            }
            break;
        case OP_LIST_BUILD:
            CNode_SetObject(&op->result, COList_New(0));
            break;
        case OP_LIST_ADD:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            COList_Append(co1, co2);
            break;
        case OP_DICT_BUILD:
            CNode_SetObject(&op->result, CODict_New());
            break;
        case OP_DICT_ADD:
            co1 = CNode_GetObject(&op->op1);
            co2 = CNode_GetObject(&op->op2);
            co3 = CNode_GetObject(&op->result);
            CODict_SetItem(co1, co2, co3);
            break;
        default:
            error("unknown handle for opcode(%ld)\n", op->opcode);
        }

on_error:
        /* End the loop if we still have an error (or return) */
        if (status != STATUS_NONE)
            break;
    }

vm_exit:
    return ret;
}
