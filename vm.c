#include "co.h"

/* status code for eval main loop */
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

    COFrameObject *current_frame = TS(frame);
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
    // at last
    co = CODict_GetItem(TS(frame)->f_builtins, name);
    if (co) {
        return co;
    }
    return NULL;
}

int
COObject_set(COObject *name, COObject *co)
{
    COFrameObject *current_frame = TS(frame);
    return CODict_SetItem(current_frame->f_locals, name, co);
}

static inline COObject *
vm_cmp(int op, COObject *o1, COObject *o2)
{
    COObject *x;
    switch (op) {
    case Cmp_EXC_MATCH:
        if (!COTuple_Check(o1)) {
            assert(0);
        }
        int i = 0;
        for (; i < COTuple_Size(o1); i++) {
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
vm_eval(COObject *func, COObject *globals)
{
#define JUMPBY(offset)  next_code += offset
#define JUMPTO(offset)  next_code = first_code + offset
#define NEXTOP()        (*next_code++)
#define NEXTARG()       (next_code += 2, (next_code[-1]<<8) + next_code[-2])
#define GETITEM(v, i)   COTuple_GET_ITEM((COTupleObject *)(v), i)
#define GETLOCAL(i)     (fastlocals[i])
#define SETLOCAL(i, v)                  \
    do {                                \
        COObject *tmp = GETLOCAL(i);    \
        GETLOCAL(i) = v;                \
        CO_XDECREF(tmp);                \
    } while (0);
#define PUSH(o)         (*stack_top++ = (o))
#define POP()           (*--stack_top)
#define TOP()           (stack_top[-1])
#define SET_TOP(o)      (stack_top[-1] = (o))
#define SECOND()        (stack_top[-2])
#define THIRD()         (stack_top[-3])
#define FOURTH()        (stack_top[-4])
#define PEEK(n)         (stack_top[-(n)])
#define STACK_ADJ(n)    (stack_top += n)
#define STACK_LEVEL()   ((int)(stack_top - TS(frame)->f_stack))
#define UNWIND_BLOCK(b) \
    do { \
        while (STACK_LEVEL() > (b)->fb_level) { \
            COObject *o = POP(); \
            CO_XDECREF(o); \
        } \
    } while (0)

    COCodeObject *code;
    COObject *names;
    COObject *consts;
    COObject *localnames;
    COObject *funcargs = COList_New(0);

    COObject **upvalues;
    COObject **fastlocals;
    COObject **stack_top;       /* Stack top, points to next free slot in stack */

    unsigned char *next_code;
    unsigned char *first_code;
    unsigned char opcode;       /* Current opcode */
    int oparg;                  /* Current opcode argument, if any */
    COObject *x;                /* Result object -- NULL if error */
    COObject *o1, *o2, *o3;     /* Temporary objects popped of stack */
    int status;                 /* VM status */
    int err;                    /* C function error code */
    status = STATUS_NONE;

    TS(frame) =
        (COFrameObject *)COFrame_New((COObject *)TS(frame), func, globals);

new_frame:                     /* reentry point when function call/return */
    code = (COCodeObject *)((COFunctionObject *)TS(frame)->f_func)->func_code;
    stack_top = TS(frame)->f_stacktop;
    names = code->co_names;
    localnames = code->co_localnames;
    consts = code->co_consts;
    first_code = (unsigned char *)COBytes_AsString(code->co_code);
    next_code = first_code + TS(frame)->f_lasti;
    fastlocals = TS(frame)->f_extraplus;
    upvalues = TS(frame)->f_extraplus + code->co_nlocals;

    /* Parse arguments. */
    if (COList_GET_SIZE(funcargs)) {
        // check arguments count
        if (code->co_argcount != COList_GET_SIZE(funcargs)) {
            COErr_Format(COException_ValueError,
                         "takes exactly %d arguments (%d given)",
                         code->co_argcount, COList_Size(funcargs));
            status = STATUS_EXCEPTION;
            goto fast_end;
        }
        size_t n = COList_Size(funcargs);
        for (int i = 0; i < n; i++) {
            x = COList_GetItem(funcargs, 0);
            //CO_INCREF(x);
            //SETLOCAL(n - i - 1, x);
            COObject_set(GETITEM(names, n - i - 1), x);
            COList_DelItem(funcargs, 0);
        }
    }

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
        case OP_BINARY_SUBSCRIPT:
            o1 = POP();
            o2 = TOP();
            if (!CO_TYPE(o2)->tp_mapping_interface) {
                COErr_Format(COException_TypeError,
                             "'%.200s' object is not subscriptable",
                             CO_TYPE(o2)->tp_name);
                status = STATUS_EXCEPTION;
            } else {
                x = CO_TYPE(o2)->tp_mapping_interface->mp_subscript(o2, o1);
                if (!x) {
                    status = STATUS_EXCEPTION;
                    goto fast_end;
                }
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_CMP:
            o1 = POP();
            o2 = TOP();
            oparg = NEXTARG();
            x = vm_cmp(oparg, o1, o2);
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
        case OP_LOAD_LOCAL:
            oparg = NEXTARG();
            x = GETLOCAL(oparg);
            CO_INCREF(x);
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
                goto fast_end;
            }
            CO_INCREF(x);
            PUSH(x);
            break;
        case OP_LOAD_UPVAL:
            oparg = NEXTARG();
            o1 = COTuple_GET_ITEM(((COFunctionObject *)func)->func_upvalues, oparg);
            o2 = COCell_Get(o1);
            PUSH(o2);
            break;
        case OP_LOAD_CONST:
            oparg = NEXTARG();
            x = GETITEM(consts, oparg);
            CO_INCREF(x);
            PUSH(x);
            break;
        case OP_PRINT:
            x = POP();
            COObject_Print(x, stdout);
            CO_DECREF(x);
            break;
        case OP_BUILD_TUPLE:
            oparg = NEXTARG();
            x = COTuple_New(oparg);
            if (x != NULL) {
                for (; --oparg >= 0;) {
                    o1 = POP();
                    COTuple_SetItem(x, oparg, o1);
                    CO_DECREF(o1);
                }
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
                PUSH(x);
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
            CODict_SetItem(o3, o2, o1);
            x = o3;
            CO_DECREF(o1);
            CO_DECREF(o2);
            PUSH(x);
            break;
        case OP_STORE_NAME:
            oparg = NEXTARG();
            o1 = GETITEM(names, oparg);
            o2 = POP();
            COObject_set(o1, o2);
            CO_DECREF(o2);
            break;
        case OP_STORE_UPVAL:
            oparg = NEXTARG();
            o1 = COTuple_GET_ITEM(((COFunctionObject *)func)->func_upvalues, oparg);
            o2 = POP();
            COCell_Set(o1, o2);
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
            for (int i = 0; i < CO_SIZE(c->co_upvals); i++) {
                COObject *name = COTuple_GET_ITEM(c->co_upvals, i);
                COObject *upvalue = COObject_get(name);
                if (upvalue) {
                    COObject *cell = COCell_New(upvalue);
                    COTuple_SET_ITEM(((COFunctionObject *)x)->func_upvalues, i, cell);
                }
            }
            CO_DECREF(o1);
            PUSH(x);
            break;
        case OP_CALL_FUNCTION:
            o1 = POP();
            oparg = NEXTARG();
            COObject *args = COTuple_New(oparg);
            while (--oparg >= 0) {
                o2 = POP();
                COTuple_SetItem(args, oparg, o2);
                CO_DECREF(o2);
            }

            if (COCFunction_Check(o1)) {
                COCFunction cfunc = COCFunction_GET_FUNCTION(o1);
                x = cfunc(NULL, args);
                CO_DECREF(o1);
                CO_DECREF(args);
                PUSH(x);
            } else if (COFunction_Check(o1)) {
                ssize_t i = CO_SIZE(args);
                while (--i >= 0) {
                    COList_Append(funcargs, COTuple_GET_ITEM(args, i));
                }
                CO_DECREF(args);
                TS(frame)->f_stacktop = stack_top;
                TS(frame)->f_lasti = (int)(next_code - first_code);
                TS(frame) =
                    (COFrameObject *)COFrame_New((COObject *)TS(frame), o1,
                                                 NULL);
                CO_DECREF(o1);
                func = o1;
                goto new_frame;
            } else {
                x = COObject_Call(o1, args);
                CO_DECREF(args);
                CO_DECREF(o1);
                PUSH(x);
            }
            break;
        case OP_RETURN:
            o1 = POP();
            TS(frame)->f_stacktop = stack_top;
            TS(frame)->f_lasti = (int)(next_code - first_code);
            COFrameObject *old_frame = (COFrameObject *)TS(frame);
            TS(frame) = (COFrameObject *)old_frame->f_prev;
            CO_DECREF(old_frame);
            if (!TS(frame)) {
                CO_DECREF(o1);
                goto vm_exit;
            }
            // init function return
            *(TS(frame)->f_stacktop++) = o1;
            goto new_frame;
            break;
        case OP_SETUP_LOOP:
            oparg = NEXTARG();
            COFrameBlock_Setup(TS(frame), opcode, oparg, STACK_LEVEL());
            break;
        case OP_SETUP_TRY:
            oparg = NEXTARG();
            COFrameBlock_Setup(TS(frame), opcode, oparg, STACK_LEVEL());
            break;
        case OP_POP_BLOCK:
            {
                COFrameBlock *fb = COFrameBlock_Pop(TS(frame));
                UNWIND_BLOCK(fb);
            }
            break;
        case OP_POP_TRY:
            {
                COFrameBlock *fb = COFrameBlock_Pop(TS(frame));
                UNWIND_BLOCK(fb);
            }
            break;
        case OP_BREAK_LOOP:
            status = STATUS_BREAK;
            break;
        case OP_CONTINUE_LOOP:
            oparg = NEXTARG();
            status = STATUS_CONTINUE;
            break;
        case OP_THROW:
            oparg = NEXTARG();
            if (oparg == 1) {
                o1 = POP();
            } else if (oparg == 0) {
                o1 = CO_None;
            } else {
                error("error oparg");
            }
            status = STATUS_EXCEPTION;
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
            COFrameBlock_Setup(TS(frame), opcode, oparg, STACK_LEVEL());
            break;
        case OP_END_FINALLY:
            o1 = POP();
            if (o1 != CO_None) {
                COErr_SetString(COException_SystemError, COStr_AsString(o1));
                status = STATUS_EXCEPTION;
            }
            CO_DECREF(o1);
            break;
        case OP_STORE_SUBSCRIPT:
            o1 = TOP();
            o2 = SECOND();
            o3 = THIRD();
            STACK_ADJ(-3);
            if (COList_Check(o3)) {
                err = COList_SetItem(o3, COInt_AsSsize_t(o2), o1);
            } else if (CODict_Check(o3)) {
                CODict_SetItem(o3, o2, o1);
            } else {
                error("wrong store subscript");
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            CO_DECREF(o3);
            break;
        default:
            error("unknown handle for opcode(%ld)\n", opcode);
        }

fast_end:

        while (status != STATUS_NONE && TS(frame)->f_iblock > 0) {
            COFrameBlock *fb =
                &TS(frame)->f_blockstack[TS(frame)->f_iblock - 1];
            if (fb->fb_type == OP_SETUP_LOOP && status == STATUS_CONTINUE) {
                status = STATUS_NONE;
                JUMPTO(oparg);
                break;
            }
            TS(frame)->f_iblock--;
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
        COFrameObject *tmp_frame = (COFrameObject *)TS(frame)->f_prev;
        CO_DECREF(TS(frame));
        TS(frame) = tmp_frame;
    }

    return x;
}
