#include "co.h"

/* status code for eval main loop (reason for stack unwind) */
enum status_code {
    STATUS_NONE = 0x0001,       /* None */
    STATUS_EXCEPTION = 0x0002,  /* Exception occurred */
};

/* Forward declarations */
COObject *COObject_get(COObject *co);
int COObject_put(COObject *name, COObject *co);

COObject *
COObject_get(COObject *name)
{
    COObject *co;

    COFrameObject *current_frame = (COFrameObject *)TS(frame);
    if (current_frame->f_func) {
        co = CODict_GetItem(((COFunctionObject *)
                             current_frame->f_func)->func_upvalues, name);
        if (co) {
            return co;
        }
    }
    do {
        co = CODict_GetItem(current_frame->f_locals, name);
        if (co) {
            return co;
        }
        current_frame = (COFrameObject *)current_frame->f_prev;
        if (!current_frame) {
            break;
        }
    } while (true);
    return NULL;
}

int
COObject_set(COObject *name, COObject *co)
{
    COFrameObject *current_frame = (COFrameObject *)TS(frame);
    if (current_frame->f_func) {
        COObject *myco;
        myco = CODict_GetItem(((COFunctionObject *)
                               current_frame->f_func)->func_upvalues, name);
        if (myco) {
            CODict_SetItem(((COFunctionObject *)
                            current_frame->f_func)->func_upvalues, name, co);
        }
    }
    return CODict_SetItem(current_frame->f_locals, name, co);
}

/*
 * Evaluate a function object into a object.
 */
COObject *
vm_eval(COObject *func)
{
#define JUMPBY(offset)  frame->bytecode += offset
#define JUMPTO(offset)  frame->bytecode = frame->firstcode + offset
#define NEXTOP()        (*frame->bytecode++)
#define NEXTARG()       (frame->bytecode += 2, (frame->bytecode[-1]<<8) + frame->bytecode[-2])
#define GETITEM(v, i)   COTuple_GET_ITEM((COTupleObject *)(v), i)
#define PUSH(o)         (*stack_top++ = (o))
#define POP()           (*--stack_top)

    COFrameObject *frame;
    COCodeObject *code;
    COObject *names;
    COObject *consts;

    COObject **stack_top;      /* Stack top, points to next free slot in stack */
    unsigned char opcode;      /* Current opcode */
    int oparg;         /* Current opcode argument, if any */
    COObject *x;       /* Result object -- NULL if error */
    COObject *o1, *o2, *o3;    /* Temporary objects popped of stack */
    int status;        /* VM status */
    int err;           /* C function error code */

    TS(mainfunc) = func;

new_frame:      /* reentry point when function call */
    code = (COCodeObject *)((COFunctionObject *)func)->func_code;
    frame = (COFrameObject *)COFrame_New();
    stack_top = frame->f_stacktop;
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
            PUSH(COList_GetItem(TS(funcargs), 0));
            COList_DelItem(TS(funcargs), 0);
        }
    }
    frame->f_prev = TS(frame);
    frame->f_locals = CODict_New();
    frame->f_func = func;
    frame->bytecode = (unsigned char *)COBytes_AsString(code->co_code);
    frame->firstcode = frame->bytecode;
    TS(frame) = (COObject *)frame;

start_frame:    /* reentry point when function return */
    status = STATUS_NONE;
    code = (COCodeObject *)((COFunctionObject *)frame->f_func)->func_code;
    names = code->co_names;
    consts = code->co_consts;

    for (;;) {
        opcode = NEXTOP();
        switch (opcode) {
        case OP_BINARY_ADD:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_add(o1, o2);
            PUSH(x);
            break;
        case OP_BINARY_SUB:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_sub(o2, o1);
            PUSH(x);
            break;
        case OP_BINARY_MUL:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_mul(o2, o1);
            PUSH(x);
            break;
        case OP_BINARY_DIV:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_div(o2, o1);
            PUSH(x);
            break;
        case OP_BINARY_MOD:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_mod(o2, o1);
            PUSH(x);
            break;
        case OP_BINARY_SL:
            o1 = POP();
            o2 = POP();
            x = COInt_Type.tp_int_interface->int_lshift(o2, o1);
            PUSH(x);
            break;
        case OP_BINARY_SR:
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
                JUMPTO(oparg);
            } else {
                err = COObject_IsTrue(o1);
                if (err > 0)
                    err = 0;
                else if (err == 0)
                    JUMPTO(oparg);
            }
            break;
        case OP_JMP:
            oparg = NEXTARG();
            JUMPTO(oparg);
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
            while (oparg--) {
                o2 = POP();
                COList_Append(TS(funcargs), o2);
            }
            func = o1;
            frame->f_stacktop = stack_top;
            goto new_frame;
            break;
        case OP_RETURN:
            x = POP();
            COFrameObject *old_frame = (COFrameObject *)TS(frame);
            TS(frame) = old_frame->f_prev;
            COFrame_Destory((COObject *)old_frame);
            if (!TS(frame)) {
                goto vm_exit;
            }
            frame = (COFrameObject *)TS(frame);
            stack_top = frame->f_stacktop;
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
