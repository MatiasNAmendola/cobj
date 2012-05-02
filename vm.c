#include "co.h"

/* status code for eval main loop (reason for stack unwind) */
enum status_code {
    STATUS_NONE = 0x0001,       /* None */
    STATUS_EXCEPTION = 0x0002,  /* Exception occurred */
    STATUS_BREAK = 0x0003,
    STATUS_CONTINUE = 0x0004,
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
            return CODict_SetItem(((COFunctionObject *)
                                   current_frame->f_func)->func_upvalues, name,
                                  co);
        }
    }
    return CODict_SetItem(current_frame->f_locals, name, co);
}

static inline COObject *
_vm_cmp(int op, COObject *o1, COObject *o2)
{
    COObject *x;
    switch (op) {
    case Cmp_EXC_MATCH:
        if (!COTuple_Check(o1)) {
            assert(0);
        }
        int i = COTuple_Size(o1);
        for (; --i >= 0;) {
            if (COObject_CompareBool(COTuple_GET_ITEM(o1, i), o2, Cmp_EQ)) {
                x = CO_True;
                CO_INCREF(x);
                goto end;
            }
        }
        x = CO_False;
        CO_INCREF(x);
        goto end;
        break;
    default:
        x = COObject_Compare(o2, o1, op);
        break;
    }

end:
    return x;
}

/*
 * Evaluate a function object into a object.
 */
COObject *
vm_eval(COObject *func)
{
#define JUMPBY(offset)  next_code += offset
#define JUMPTO(offset)  next_code = first_code + offset
#define NEXTOP()        (*next_code++)
#define NEXTARG()       (next_code += 2, (next_code[-1]<<8) + next_code[-2])
#define GETITEM(v, i)   COTuple_GET_ITEM((COTupleObject *)(v), i)
#define PUSH(o)         (*stack_top++ = (o))
#define POP()           (*--stack_top)
#define TOP()           (stack_top[-1])
#define SET_TOP(o)      (stack_top[-1] = (o))
#define SECOND()        (stack_top[-2])
#define THIRD()         (stack_top[-3])
#define FOURTH()        (stack_top[-4])
#define PEEK(n)         (stack_top[-(n)])
#define STACK_LEVEL()   ((int)(stack_top - frame->f_stack))
#define UNWIND_BLOCK(b) \
    do { \
        while (STACK_LEVEL() > (b)->fb_level) { \
            COObject *o = POP(); \
            CO_XDECREF(o); \
        } \
    } while (0)

    CO_INCREF(func);

    COFrameObject *frame;
    COCodeObject *code;
    COObject *names;
    COObject *consts;

    COObject **stack_top;       /* Stack top, points to next free slot in stack */

    unsigned char *next_code;
    unsigned char *first_code;
    unsigned char opcode;       /* Current opcode */
    int oparg;                  /* Current opcode argument, if any */
    COObject *x;                /* Result object -- NULL if error */
    COObject *o1, *o2, *o3;     /* Temporary objects popped of stack */
    int status;                 /* VM status */
    int err;                    /* C function error code */

new_frame:                     /* reentry point when function call */
    status = STATUS_NONE;
    code = (COCodeObject *)((COFunctionObject *)func)->func_code;
    frame = (COFrameObject *)COFrame_New((COObject *)code, (COObject *)TS(frame), func);
    TS(frame) = frame;
    stack_top = frame->f_stacktop;
    names = code->co_names;
    consts = code->co_consts;
    if (COList_Size(TS(funcargs))) {
        // check arguments count
        if (code->co_argcount != COList_Size(TS(funcargs))) {
            COErr_Format(COException_ValueError,
                         "takes exactly %d arguments (%d given)",
                         code->co_argcount, COList_Size(TS(funcargs)));
            status = STATUS_EXCEPTION;
            goto fast_end;
        }
        size_t n = COList_Size(TS(funcargs));
        for (int i = 0; i < n; i++) {
            o1 = GETITEM(names, n - i - 1);
            COObject_set(o1, COList_GetItem(TS(funcargs), 0));
            COList_DelItem(TS(funcargs), 0);
        }
    }

start_frame:                   /* reentry point when function return */
    first_code = (unsigned char *)COBytes_AsString(code->co_code);
    next_code = first_code + frame->f_lasti;
    for (;;) {
        opcode = NEXTOP();
        switch (opcode) {
        case OP_BINARY_ADD:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_add(o1, o2);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SUB:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_sub(o2, o1);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_MUL:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_mul(o2, o1);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_DIV:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_div(o2, o1);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_MOD:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_mod(o2, o1);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SL:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_lshift(o2, o1);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SR:
            o1 = POP();
            o2 = TOP();
            x = COInt_Type.tp_int_interface->int_rshift(o2, o1);
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_CMP:
            o1 = POP();
            o2 = TOP();
            oparg = NEXTARG();
            x = _vm_cmp(oparg, o1, o2);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_UNARY_NEGATE:
            o1 = TOP();
            x = COInt_Type.tp_int_interface->int_neg(o1);
            CO_DECREF(o1);
            SET_TOP(x);
            break;
        case OP_UNARY_INVERT:
            o1 = TOP();
            x = COInt_Type.tp_int_interface->int_invert(o1);
            CO_DECREF(o1);
            SET_TOP(x);
            break;
        case OP_LOAD_NAME:
            oparg = NEXTARG();
            o1 = GETITEM(names, oparg);
            x = COObject_get(o1);
            if (!x) {
                COErr_Format(COException_NameError, "name '%s' is not defined",
                             COStr_AsString(o1));
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_INCREF(x);
            PUSH(x);
            break;
        case OP_LOAD_CONST:
            oparg = NEXTARG();
            x = GETITEM(consts, oparg);
            CO_INCREF(x);
            PUSH(x);
            break;
        case OP_PRINT:
            x = POP();
            COObject_print(x);
            CO_DECREF(x);
            break;
        case OP_BUILD_TUPLE:
            oparg = NEXTARG();
            x = COTuple_New(oparg);
            if (x != NULL) {
                for (; --oparg >= 0;) {
                    o1 = POP();
                    COTuple_SET_ITEM(x, oparg, o1);
                    CO_DECREF(o1);
                }
                CO_INCREF(x);
                PUSH(x);
            }
            break;
        case OP_BUILD_LIST:
            oparg = NEXTARG();
            x = COList_New(oparg);
            if (x != NULL) {
                for (; --oparg >= 0;) {
                    o1 = POP();
                    COList_SetItem(x, oparg, o1);
                    CO_DECREF(o1);
                }
                CO_INCREF(x);
                PUSH(x);
                continue;
            }
            break;
        case OP_DICT_BUILD:
            oparg = NEXTARG();
            x = CODict_New();
            PUSH(x);
            break;
        case OP_DICT_ADD:
            o1 = POP();
            o2 = POP();
            o3 = POP();
            CODict_SetItem(o3, o1, o2);
            x = o3;
            CO_DECREF(o1);
            CO_DECREF(o2);
            PUSH(x);
            break;
        case OP_ASSIGN:
            oparg = NEXTARG();
            o1 = GETITEM(names, oparg);
            o2 = POP();
            COObject_set(o1, o2);
            CO_DECREF(o2);
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
            CO_DECREF(o1);
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
            CO_DECREF(o1);
            PUSH(x);
            break;
        case OP_CALL_FUNCTION:
            o1 = POP();
            oparg = NEXTARG();
            while (oparg--) {
                o2 = POP();
                COList_Append(TS(funcargs), o2);
                CO_DECREF(o2);
            }
            func = o1;
            frame->f_stacktop = stack_top;
            frame->f_lasti = (int)(next_code - first_code);
            CO_DECREF(o1);
            goto new_frame;
            break;
        case OP_RETURN:
            o1 = POP();
            COFrameObject *old_frame = (COFrameObject *)TS(frame);
            TS(frame) = (COFrameObject *)old_frame->f_prev;
            CO_DECREF(old_frame);
            if (!TS(frame)) {
                CO_DECREF(o1);
                goto vm_exit;
            }
            // init function return
            frame = (COFrameObject *)TS(frame);
            stack_top = frame->f_stacktop;
            CO_INCREF(o1);
            PUSH(o1);
            func = frame->f_func;
            code =
                (COCodeObject *)((COFunctionObject *)frame->f_func)->func_code;
            names = code->co_names;
            consts = code->co_consts;
            goto start_frame;
            break;
        case OP_SETUP_LOOP:
            oparg = NEXTARG();
            COFrameBlock_Setup(frame, opcode, oparg, STACK_LEVEL());
            break;
        case OP_SETUP_TRY:
            oparg = NEXTARG();
            COFrameBlock_Setup(frame, opcode, oparg, STACK_LEVEL());
            break;
        case OP_POP_BLOCK:
            {
                COFrameBlock *fb = COFrameBlock_Pop(frame);
                UNWIND_BLOCK(fb);
            }
            break;
        case OP_POP_TRY:
            {
                COFrameBlock *fb = COFrameBlock_Pop(frame);
                UNWIND_BLOCK(fb);
            }
            break;
        case OP_BREAK_LOOP:
            status = STATUS_BREAK;
            break;
        case OP_CONTINUE_LOOP:
            oparg = NEXTOP();
            status = STATUS_CONTINUE;
            break;
        case OP_THROW:
            status = STATUS_EXCEPTION;
            o1 = TOP();
            COErr_SetObject(COException_SystemError, o1);
            break;
        case OP_DUP_TOP:
            o1 = TOP();
            CO_INCREF(o1);
            PUSH(o1);
            break;
        case OP_POP_TOP:
            o1 = POP();
            CO_DECREF(o1);
            break;
        case OP_END_TRY:
            o1 = POP();
            COErr_SetString(COException_SystemError, COStr_AsString(o1));
            status = STATUS_EXCEPTION;
            CO_DECREF(o1);
            break;
        case OP_SETUP_FINALLY:
            oparg = NEXTARG();
            COFrameBlock_Setup(frame, opcode, oparg, STACK_LEVEL());
            break;
        case OP_END_FINALLY:
            o1 = POP();
            if (o1 != CO_None) {
                COErr_SetString(COException_SystemError, COStr_AsString(o1));
                status = STATUS_EXCEPTION;
            }
            CO_DECREF(o1);
            break;
        default:
            error("unknown handle for opcode(%ld)\n", opcode);
        }

fast_end:

        while (status != STATUS_NONE && frame->f_iblock > 0) {
            COFrameBlock *fb = &frame->f_blockstack[frame->f_iblock - 1];
            if (fb->fb_type == OP_SETUP_LOOP && status == STATUS_CONTINUE) {
                status = STATUS_NONE;
                JUMPTO(oparg);
                break;
            }
            frame->f_iblock--;
            UNWIND_BLOCK(fb);
            if (fb->fb_type == OP_SETUP_LOOP && status == STATUS_BREAK) {
                status = STATUS_NONE;
                JUMPTO(fb->fb_handler);
                break;
            }
            if (fb->fb_type == OP_SETUP_TRY && status == STATUS_EXCEPTION) {
                status = STATUS_NONE;
                COObject *exc, *val, *tb;
                COErr_Fetch(&exc, &val, &tb);
                PUSH(val);
                JUMPTO(fb->fb_handler);
                break;
            }
        }

        /* End the loop if we still have an error (or return) */
        x = NULL;
        if (status != STATUS_NONE)
            break;
    }

vm_exit:

    /* Clear frame stack. */
    while (TS(frame)) {
        frame = (COFrameObject *)TS(frame)->f_prev;
        CO_DECREF(TS(frame));
        TS(frame) = frame;
    }
    
    /* Clear current function. */
    CO_DECREF(func);

    return x;
}
