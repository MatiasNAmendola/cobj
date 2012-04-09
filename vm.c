#include "co.h"

/* status code for eval main loop (reason for stack unwind) */
enum status_code {
    STATUS_NONE = 0x0001,       /* None */
    STATUS_EXCEPTION = 0x0002,  /* Exception occurred */
};

/* execution frame */
struct co_exec_data {
    struct co_exec_data *prev_exec_data;
    COObject *function_called;
    COObject *symbol_table;     /* dict object for names */
    unsigned char *firstcode;
    unsigned char *bytecode;
};

/* Forward declarations */
COObject *COObject_get(COObject *co);
int COObject_put(COObject *name, COObject *co);

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
                               current_exec_data->function_called)->
                              func_upvalues, name);
        if (myco) {
            CODict_SetItem(((COFunctionObject *)
                            current_exec_data->function_called)->func_upvalues,
                           name, co);
        }
    }
    return CODict_SetItem(TS(current_exec_data)->symbol_table, name, co);
}

#define JUMPBY(offset)  exec_data->bytecode += offset
#define JUMPTO(offset)  exec_data->bytecode = exec_data->firstcode + offset
#define NEXTOP()    (*exec_data->bytecode++)
#define NEXTARG()   (exec_data->bytecode += 2, (exec_data->bytecode[-1]<<8) + exec_data->bytecode[-2])
#define GETITEM(v, i)   COTuple_GET_ITEM((COTupleObject *)(v), i)
#define PUSH(o)     COFrame_Push(f, (COObject *)o);
#define POP()       COFrame_Pop(f);

/*
 * Evaluate a function object into a object.
 */
COObject *
vm_eval(COObject *func)
{
    struct co_exec_data *exec_data;
    COObject *f = TS(frame);
    COObject *names;
    COObject *consts;
    COCodeObject *code;

    register unsigned char opcode;      /* Current opcode */
    register int oparg;                 /* Current opcode argument, if any */
    register COObject *x;               /* Result object -- NULL if error */
    register COObject *o1, *o2, *o3;    /* Temporary objects popped of stack */
    register int status;                /* VM status */
    register int err;                   /* C function error code */

    TS(mainfunc) = func;

new_frame:
    code = (COCodeObject *)((COFunctionObject *)func)->func_code;
    exec_data =
        (struct co_exec_data *)COFrame_Alloc(f, sizeof(struct co_exec_data));
    if (COList_Size(TS(funcargs))) {
        // check arguments count
        if (code->co_argcount != COList_Size(TS(funcargs))) {
            COErr_Format(COException_ValueError,
                         "takes exactly %d arguments (%d given)",
                         code->co_argcount, COList_Size(TS(funcargs)));
            status = STATUS_EXCEPTION;
            goto on_error;
        }
        size_t n = COList_Size(TS(funcargs));
        for (int i = 0; i < n; i++) {
            COFrame_Push(f, COList_GetItem(TS(funcargs), 0));
            COList_DelItem(TS(funcargs), 0);
        }
    }
    exec_data->prev_exec_data = NULL;
    exec_data->symbol_table = CODict_New();
    exec_data->function_called = func;
    exec_data->prev_exec_data = TS(current_exec_data);
    exec_data->bytecode = (unsigned char *)COBytes_AsString(code->co_code);
    exec_data->firstcode = exec_data->bytecode;
    TS(current_exec_data) = exec_data;

start_frame:
    status = STATUS_NONE;
    code =
        (COCodeObject *)((COFunctionObject *)exec_data->function_called)->
        func_code;
    names = code->co_names;
    consts = code->co_consts;

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
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_lshift(o2, o1);
            PUSH(x);
            break;
        case OP_SR:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_rshift(o2, o1);
            PUSH(x);
            break;
        case OP_CMP:
            o1 = POP();
            o2 = POP();
            oparg = NEXTARG();
            x = COObject_Compare(o2, o1, oparg);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto on_error;
            }
            PUSH(x);
            break;
        case OP_UNARY_NEGATE:
            o1 = POP();
            x = COInt_Type.tp_int_interface->int_neg(o1);
            PUSH(x);
            break;
        case OP_UNARY_INVERT:
            o1 = POP();
            x = COInt_Type.tp_int_interface->int_invert(o1);
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
            if (o1 == CO_True) {
            } else if (o1 == CO_False) {
                JUMPBY(oparg);
            } else {
                err = COObject_IsTrue(o1);
                if (err > 0)
                    err = 0;
                else if (err == 0)
                    JUMPBY(oparg);
            }   
            break;
        case OP_JMP:
            oparg = NEXTARG();
            JUMPBY(oparg);
            break;
        case OP_JMPX:
            oparg = NEXTARG();
            JUMPTO(oparg);
            break;
        case OP_DECLARE_FUNCTION:
            o1 = POP();
            x = COFunction_New(o1);
            COCodeObject *c = (COCodeObject *)o1;
            for (int i = 0; i < CO_SIZE(c->co_names); i++) {
                COObject *name = COTuple_GET_ITEM(c->co_names, i);
                COObject *upvalue = COObject_get(name);
                if (upvalue) {
                    CODict_SetItem(((COFunctionObject *)x)->func_upvalues, name,
                                   upvalue);
                }
            }
            PUSH(x);
            break;
        case OP_CALL_FUNCTION:
            o1 = POP();
            oparg = NEXTARG();
            func = o1;
            while (oparg--) {
                o2 = POP();
                COList_Append(TS(funcargs), o2);
            }
            goto new_frame;
            break;
        case OP_RETURN:
            x = POP();
            struct co_exec_data *old_exec_data = TS(current_exec_data);
            TS(current_exec_data) = TS(current_exec_data)->prev_exec_data;
            COFrame_Free(f, (COObject *)old_exec_data);
            if (!TS(current_exec_data)) {
                goto vm_exit;
            }
            exec_data = TS(current_exec_data);
            PUSH(x);
            goto start_frame;
            break;
        default:
            error("unknown handle for opcode(%ld)\n", opcode);
        }

on_error:
        /* End the loop if we still have an error (or return) */
        x = NULL;
        if (status != STATUS_NONE)
            break;
    }

vm_exit:
    return x;
}
