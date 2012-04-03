#include "co.h"

/* status code for eval main loop (reason for stack unwind) */
enum status_code {
    STATUS_NONE = 0x0001,       /* None */
    STATUS_EXCEPTION = 0x0002,  /* Exception occurred */
};

/* execution frame */
struct co_exec_data {
    struct co_exec_data *prev_exec_data;
    COOplineObject **op;
    COObject *function_called;
    COObject *symbol_table;     /* dict object for names */
    COObject **ts;              /* temp objects */
};

/* Forward declaration */
COObject *COObject_get(COObject *co);
int COObject_put(COObject *name, COObject *co);
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
COObject_set(COObject *name, COObject *co)
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

#define JUMPOP(offset)  bytecode += offset
#define NEXTOP()    (*bytecode++)
#define NEXTARG()   (bytecode += 2, (bytecode[-1]<<8) + bytecode[-2])
#define GETITEM(v, i)   COTuple_GET_ITEM((COTupleObject *)(v), i)
#define PUSH(o)     COFrame_Push(f, o);
#define POP()       COFrame_Pop(f);

/*
 * Evaluate a function object into a object.
 */
COObject *
vm_eval(COObject *mainfunc)
{
    struct co_exec_data *exec_data;
    COObject *f = TS(frame);
    COCodeObject *code;
    unsigned char *bytecode;
    unsigned char *firstcode;
    COObject *names;
    COObject *consts;
    register unsigned char opcode;       /* Current opcode */
    register int oparg;                  /* Current opcode argument, if any */
    register COObject *x;                /* Result object -- NULL if error */
    register COObject *o1, *o2, *o3;     /* Temporary objects popped of stack */
    TS(mainfunc) = mainfunc;

vm_enter:
    code = (COCodeObject *)((COFunctionObject *)mainfunc)->func_code;
    exec_data = (struct co_exec_data *)COFrame_Alloc(f, sizeof(struct co_exec_data));
    exec_data->prev_exec_data = NULL;
    exec_data->symbol_table = CODict_New();
    exec_data->function_called = mainfunc;
    exec_data->prev_exec_data = TS(current_exec_data);

    TS(current_exec_data) = exec_data;


    int status = STATUS_NONE;
    names = code->co_names;
    consts = code->co_consts;
    bytecode = (unsigned char *)COBytes_AsString(code->co_code);
    firstcode = bytecode;

    for (;;) {
        opcode = NEXTOP();
        switch (opcode) {
        case OP_ADD:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_add(o1, o2);
            PUSH(x);
            break;
        case OP_SUB:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_sub(o2, o1);
            PUSH(x);
            break;
        case OP_MUL:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_mul(o2, o1);
            PUSH(x);
            break;
        case OP_DIV:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_div(o2, o1);
            PUSH(x);
            break;
        case OP_MOD:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_mod(o2, o1);
            PUSH(x);
            break;
        case OP_SL:
            // TODO support int object
            o1 = POP();
            o2 = POP();
            x = COInt_FromLong(COInt_AsLong(o2) << COInt_AsLong(o1));
            PUSH(x);
            break;
        case OP_SR:
            // TODO support int object
            o1 = POP();
            o2 = POP();
            x = COInt_FromLong(COInt_AsLong(o2) >> COInt_AsLong(o1));
            PUSH(x);
            break;
        case OP_IS_SMALLER:
            o1 = POP();
            o2 = POP();
            x = COBool_FromLong(COInt_AsLong(o2) < COInt_AsLong(o1));
            PUSH(x);
            break;
        case OP_IS_SMALLER_OR_EQUAL:
            o1 = POP();
            o2 = POP();
            x = COBool_FromLong(COInt_AsLong(o2) <= COInt_AsLong(o1));
            PUSH(x);
            break;
        case OP_IS_EQUAL:
            o1 = POP();
            o2 = POP();
            x = COBool_FromLong(COInt_AsLong(o2) == COInt_AsLong(o1));
            PUSH(x);
            break;
        case OP_IS_NOT_EQUAL:
            o1 = POP();
            o2 = POP();
            x = COBool_FromLong(COInt_AsLong(o2) != COInt_AsLong(o1));
            PUSH(x);
            break;
        case OP_LOAD_NAME:
            oparg = NEXTARG();
            o1 = GETITEM(names, oparg);
            x = COObject_get(o1);
            if (!x) {
                COErr_Format(COException_NameError, "name '%s' is not defined",
                         COStr_AsString(o1)); 
                status = STATUS_EXCEPTION;
                goto on_error;
            }
            PUSH(x);
            break;
        case OP_LOAD_CONST:
            oparg = NEXTARG();
            x = GETITEM(consts, oparg);
            PUSH(x);
            break;
        case OP_PRINT:
            x = POP();
            COObject_print(x);
            break;
        case OP_LIST_BUILD:
            x = COList_New(0);
            PUSH(x);
            break; 
        case OP_LIST_ADD:
            o1 = POP();
            o2 = POP();
            COList_Append(o2, o1);
            x = o2;
            PUSH(x);
            break;
        case OP_DICT_BUILD:
            x = CODict_New();
            PUSH(x);
            break;
        case OP_DICT_ADD:
            o1 = POP();
            o2 = POP();
            o3 = POP();
            CODict_SetItem(o3, o1, o2);
            x = o3;
            PUSH(x);
            break;
        case OP_ASSIGN:
            oparg = NEXTARG();
            o1 = GETITEM(names, oparg);
            o2 = POP();
            COObject_set(o1, o2);
            break;
        case OP_JMPZ:
            oparg = NEXTARG();
            o1 = POP();
            if (o1 != CO_True && COBool_FromLong(COInt_AsLong(o1)) != CO_True) {
                JUMPOP(oparg);
            }
            break;
        case OP_JMP:
            oparg = NEXTARG();
            JUMPOP(oparg);
            break;
        case OP_RETURN:
            goto vm_exit;
        }
        /*
        switch (op->opcode) {
        case OP_ADD:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            o3 = COInt_Type.tp_int_interface->int_add(o1, o2);
            CNode_SetObject(&op->result, o3);
            break;
        case OP_SUB:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            o3 = COInt_Type.tp_int_interface->int_sub(o1, o2);
            CNode_SetObject(&op->result, o3);
            break;
        case OP_MUL:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            o3 = COInt_Type.tp_int_interface->int_mul(o1, o2);
            CNode_SetObject(&op->result, o3);
            break;
        case OP_POW:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result, COInt_FromLong((int)
                                                        pow(COInt_AsLong(o1),
                                                            COInt_AsLong(o2))
                            )
                );
            break;
        case OP_DIV:
            co1 = CNode_GetObject(&op->arg1);
            co2 = CNode_GetObject(&op->arg2);
            co3 = COInt_Type.tp_int_interface->int_div(co1, co2);
            CNode_SetObject(&op->result, co3);
            break;
        case OP_MOD:
            co1 = CNode_GetObject(&op->arg1);
            co2 = CNode_GetObject(&op->arg2);
            co3 = COInt_Type.tp_int_interface->int_mod(co1, co2);
            CNode_SetObject(&op->result, co3);
            break;
        case OP_SR:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(o1) >>
                                           COInt_AsLong(o2)));
            break;
        case OP_SL:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result,
                            COInt_FromLong(COInt_AsLong(o1) <<
                                           COInt_AsLong(o2)));
            break;
        case OP_IS_EQUAL:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(o1) ==
                                            COInt_AsLong(o2)));
            break;
        case OP_IS_NOT_EQUAL:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(o1) !=
                                            COInt_AsLong(o2)));
            break;
        case OP_IS_SMALLER:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(o1) <
                                            COInt_AsLong(o2)));
            break;
        case OP_IS_SMALLER_OR_EQUAL:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->result,
                            COBool_FromLong(COInt_AsLong(o1) <=
                                            COInt_AsLong(o2)));
            break;
        case OP_ASSIGN:
            o2 = CNode_GetObject(&op->arg2);
            CNode_SetObject(&op->arg1, o2);
            CNode_SetObject(&op->result, o2);
            break;
        case OP_PRINT:
            o1 = CNode_GetObject(&op->arg1);
            if (o1 == NULL) {
                goto on_error;
            }
            COObject_print(o1);
            break;
        case OP_JMPZ:
            o1 = CNode_GetObject(&op->arg1);

            if (o1 != CO_True && COBool_FromLong(COInt_AsLong(o1)) != CO_True) {
                JUMPOP(op->arg2.u.opline_num);
            }

            break;
        case OP_JMP:
            o1 = CNode_GetObject(&op->arg1);
            JUMPOP(op->arg1.u.opline_num);
            break;
        case OP_DECLARE_FUNCTION:
            {
                COFunctionObject *func =
                    (COFunctionObject *)COFunction_New(op->arg1.u.co, NULL,
                                                       NULL);
                uint start =
                    TS(current_exec_data)->op -
                    (COOplineObject **)((COTupleObject *)code->
                                        co_oplines)->co_item - 1;
                COObject *suboplines =
                    COTuple_GetSlice(code->co_oplines, start + 1,
                                     start + 1 + op->arg2.u.opline_num);
                int numoftmpvars = code->co_numoftmpvars;
                if (TS(current_exec_data)->function_called) {
                    // setup function's func_upvalues
                    COObject *co;
                    COObject *name;
                    for (int i = 0; i < CO_SIZE(suboplines); i++) {
                        COOplineObject *tmp = (COOplineObject *)
                            COTuple_GET_ITEM(suboplines,
                                             i);
                        if (tmp->arg1.type == IS_VAR) {
                            name = tmp->arg1.u.co;
                            co = COObject_get(name);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, name, co);
                            }
                        }
                        if (tmp->arg2.type == IS_VAR) {
                            name = tmp->arg2.u.co;
                            co = COObject_get(name);
                            if (co) {
                                CODict_SetItem(func->func_upvalues, name, co);
                            }
                        }
                    }
                }
                func->func_code = COCode_New(suboplines, numoftmpvars);
                if (op->arg1.type != IS_UNUSED) {
                    CNode_SetObject(&op->arg1, (COObject *)func);
                }
                if (op->result.type != IS_UNUSED) {
                    CNode_SetObject(&op->result, (COObject *)func);
                }
                JUMPOP(op->arg2.u.opline_num + 1);
                break;
            }
        case OP_RETURN:
            {
                COObject *co;
                struct co_exec_data *old_exec_data;
                old_exec_data = TS(current_exec_data);
                if (op->arg1.type != IS_UNUSED) {
                    co = CNode_GetObject(&op->arg1);
                }
#ifdef CO_DEBUG
                printf("vm leave: %p, back: %p\n", TS(current_exec_data),
                       TS(current_exec_data)->prev_exec_data);
#endif
                TS(current_exec_data) = TS(current_exec_data)->prev_exec_data;
                COFrame_Free(f, (COObject *)old_exec_data);
                struct cnode *rt = (struct cnode *)COFrame_Pop(f);
                if (op->arg1.type != IS_UNUSED) {
                    CNode_SetObject(rt, co);
                } else {

                }
                if (TS(current_exec_data) == NULL) {
                    goto vm_exit;
                }
                break;
            }
        case OP_DO_FCALL:
            o1 = CNode_GetObject(&op->arg1);
#ifdef CO_DEBUG
            printf("call function: %p\n", o1);
#endif
            if (CO_TYPE(o1) != &COFunction_Type) {
                error("not a function");
            }
            COFrame_Push(f, (COObject *)&op->result);
            mainfunc = o1;

            goto vm_enter;
        case OP_PASS_PARAM:
            o1 = CNode_GetObject(&op->arg1);
            co_stack_push(&TS(argument_stack), &o1, sizeof(COObject *));
            break;
        case OP_RECV_PARAM:
            {
                COObject **co;
                co_stack_top(&TS(argument_stack), (void **)&co);
                co_stack_pop(&TS(argument_stack));
                COObject_put(op->arg1.u.co, *co);
                break;
            }
        case OP_LIST_BUILD:
            CNode_SetObject(&op->result, COList_New(0));
            break;
        case OP_LIST_ADD:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            COList_Append(o1, o2);
            break;
        case OP_DICT_BUILD:
            CNode_SetObject(&op->result, CODict_New());
            break;
        case OP_DICT_ADD:
            o1 = CNode_GetObject(&op->arg1);
            o2 = CNode_GetObject(&op->arg2);
            o3 = CNode_GetObject(&op->result);
            CODict_SetItem(o1, o2, o3);
            break;
        default:
            error("unknown handle for opcode(%ld)\n", op->opcode);
        }
        */
on_error:
        /* End the loop if we still have an error (or return) */
        if (status != STATUS_NONE)
            break;
    }

vm_exit:
    return x;
}
