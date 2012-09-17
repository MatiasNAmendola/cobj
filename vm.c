#include "cobj.h"

/* status code for eval main loop */
enum status_code {
    STATUS_NONE = 0x0001,       /* None */
    STATUS_EXCEPTION = 0x0002,  /* Exception occurred */
    STATUS_BREAK = 0x0003,
    STATUS_CONTINUE = 0x0004,
};

COObject *
vm_getglobal(COFrameObject *frame, COObject *name)
{
    COObject *co;

    co = CODict_GetItem(frame->f_globals, name);
    if (co) {
        return co;
    }
    // at last
    co = CODict_GetItem(frame->f_builtins, name);
    if (co) {
        return co;
    }
    return NULL;
}

static inline COObject *
vm_cmp(int op, COObject *o1, COObject *o2)
{
    int ret = 0;
    COObject *x;
    switch (op) {
    case Cmp_IS:
        ret = (o1 == o2);
        break;
    case Cmp_IS_NOT:
        ret = (o1 != o2);
        break;
    case Cmp_IN:
        ret = COSequence_Contains(o1, o2);
        if (ret < 0)
            return NULL;
        break;
    case Cmp_NOT_IN:
        ret = COSequence_Contains(o1, o2);
        if (ret < 0)
            return NULL;
        ret = !ret;
        break;
    case Cmp_EXC_MATCH:
        if (!COTuple_Check(o1)) {
            assert(0);
        }
        int i = 0;
        for (; i < COTuple_GET_SIZE(o1); i++) {
            if (COObject_CompareBool(COTuple_GET_ITEM(o1, i), o2, Cmp_EQ)) {
                x = CO_True;
                CO_INCREF(x);
                return x;
            }
        }
        x = CO_False;
        CO_INCREF(x);
        return x;
    default:
        return COObject_Compare(o2, o1, op);
    }

    x = ret ? CO_True : CO_False;
    CO_INCREF(x);
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
#define SETGLOBAL(n, v) CODict_SetItem(frame->f_globals, n, v)
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
#define STACK_LEVEL()   ((int)(stack_top - frame->f_stack))
#define UNWIND_BLOCK(b) \
    do { \
        while (STACK_LEVEL() > (b)->fb_level) { \
            COObject *o = POP(); \
            CO_XDECREF(o); \
        } \
    } while (0)

    COObject *code;
    COObject *names;
    COObject *consts;
    COObject *localnames;

    COObject **fastlocals;
    COObject **stack_top;       /* Stack top, points to next free slot in stack */

    unsigned char *next_code;
    unsigned char *first_code;
    unsigned char opcode;       /* Current opcode */
    int oparg = 0;              /* Current opcode argument, if any */
    COObject *x = NULL;         /* Result object -- NULL if error */
    COObject *o1 = NULL, *o2 = NULL, *o3 = NULL;        /* Temporary objects popped of stack */
    int status = STATUS_NONE;   /* VM status */
    int err = 0;                /* C function error code */

    COFrameObject *frame = (COFrameObject *)COFrame_New(NULL, func, globals);

new_frame:                     /* reentry point when function call/return */
    func = frame->f_func;
    code = ((COFunctionObject *)func)->func_code;
    stack_top = frame->f_stacktop;
    names = ((COCodeObject *)code)->co_names;
    localnames = ((COCodeObject *)code)->co_localnames;
    consts = ((COCodeObject *)code)->co_consts;
    first_code =
        (unsigned char *)COBytes_AsString(((COCodeObject *)code)->co_code);
    next_code = first_code + frame->f_lasti;
    fastlocals = frame->f_extraplus;

    for (;;) {
        opcode = NEXTOP();
        switch (opcode) {
        case OP_BINARY_ADD:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Add(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SUB:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Sub(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_MUL:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Mul(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_DIV:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Div(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_MOD:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Mod(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SL:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Lshift(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SR:
            o1 = POP();
            o2 = TOP();
            x = COArithmetic_Rshift(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            SET_TOP(x);
            break;
        case OP_BINARY_SUBSCRIPT:
            o1 = POP();
            o2 = TOP();
            x = COMapping_GetItem(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            CO_INCREF(x);
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
            x = COArithmetic_Neg(o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            SET_TOP(x);
            break;
        case OP_UNARY_INVERT:
            o1 = TOP();
            x = COArithmetic_Invert(o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
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
            x = vm_getglobal(frame, o1);
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
            o1 = COTuple_GET_ITEM(((COFunctionObject *)func)->func_upvalues,
                                  oparg);
            PUSH(o1);
            break;
        case OP_LOAD_CONST:
            oparg = NEXTARG();
            x = GETITEM(consts, oparg);
            CO_INCREF(x);
            PUSH(x);
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
            SETGLOBAL(o1, o2);
            CO_DECREF(o2);
            break;
        case OP_STORE_UPVAL:
            oparg = NEXTARG();
            o2 = POP();
            COTuple_SET_ITEM(((COFunctionObject *)func)->func_upvalues,
                             oparg, o2);
            break;
        case OP_STORE_LOCAL:
            oparg = NEXTARG();
            o1 = POP();
            SETLOCAL(oparg, o1);
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
        case OP_MAKE_FUNCTION:
            oparg = NEXTARG();
            o1 = POP();
            COCodeObject *c = (COCodeObject *)o1;
            x = COFunction_New((COObject *)c);
            // defaults
            COObject *defaults = NULL;
            if (oparg > 0) {
                defaults = COTuple_New(oparg);
                int i = oparg;
                while (i--) {
                    COTuple_SetItem(defaults, oparg - (i + 1), POP());
                }
            }
            // load code object
            ((COFunctionObject *)x)->func_defaults = defaults;
            // upvalues
            for (int i = 0; i < CO_SIZE(c->co_upvals); i++) {
                COObject *name = COTuple_GET_ITEM(c->co_upvals, i);
                COObject *upvalue = vm_getglobal(frame, name);
                if (!upvalue) {
                    // find in current & preivous stacks
                    COFrameObject *currentframe = frame;
                    do {
                        COObject *mylocalnames =
                            ((COCodeObject *)currentframe->f_code)->co_localnames;
                        for (int j = 0; j < COTuple_GET_SIZE(mylocalnames); j++) {
                            if (COObject_CompareBool
                                (COTuple_GET_ITEM(mylocalnames, j), name,
                                 Cmp_EQ)) {
                                upvalue = currentframe->f_extraplus[j];
                                break;
                            }
                        }
                        currentframe = (COFrameObject *)currentframe->f_prev;
                    } while (currentframe);
                    // find in upvalues
                    if (!upvalue) {
                        for (int j = 0;
                             j <
                             COTuple_GET_SIZE(((COCodeObject *)
                                               code)->co_upvals); j++) {

                            if (COObject_CompareBool
                                (COTuple_GET_ITEM
                                 (((COCodeObject *)code)->co_upvals, j), name,
                                 Cmp_EQ)) {
                                upvalue = COTuple_GET_ITEM(((COFunctionObject *)
                                                            func)->
                                                           func_upvalues, j);
                                break;
                            }
                        }
                    }
                }
                COTuple_SET_ITEM(((COFunctionObject *)x)->func_upvalues, i,
                                 upvalue);
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

            /* Always dispath CFunction first, because these are presumed to be
             * the most frequently called objects.
             */
            if (COCFunction_Check(o1)) {
                COCFunction cfunc = COCFunction_GET_FUNCTION(o1);
                x = cfunc(args);
                if (COErr_Occurred()) {
                    status = STATUS_EXCEPTION;
                    goto fast_end;
                }
                CO_DECREF(o1);
                CO_DECREF(args);
                PUSH(x);
            } else if (COFunction_Check(o1)) {
                // Load defaults arguments
                COObject *defaults = ((COFunctionObject *)o1)->func_defaults;
                int defaults_argcount = 0;
                if (defaults) {
                    defaults_argcount = COTuple_GET_SIZE(defaults);
                }
                // Store frame.
                frame->f_stacktop = stack_top;
                frame->f_lasti = (int)(next_code - first_code);

                // Set up new frame.
                frame =
                    (COFrameObject *)COFrame_New((COObject *)frame, o1,
                                                 globals);

                code = ((COFunctionObject *)o1)->func_code;
                if (defaults_argcount == 0
                    && COTuple_GET_SIZE(args) !=
                    ((COCodeObject *)code)->co_argcount) {
                    COErr_Format(COException_ValueError,
                                 "takes exactly %d arguments (%d given)",
                                 ((COCodeObject *)code)->co_argcount,
                                 COList_GET_SIZE(args));
                } else if (COTuple_GET_SIZE(args) <
                           ((COCodeObject *)code)->co_argcount -
                           defaults_argcount) {
                    COErr_Format(COException_ValueError,
                                 "takes at least %d arguments (%d given)",
                                 ((COCodeObject *)code)->co_argcount -
                                 defaults_argcount, COList_GET_SIZE(args));
                    status = STATUS_EXCEPTION;
                } else if (COTuple_GET_SIZE(args) >
                           ((COCodeObject *)code)->co_argcount) {
                    COErr_Format(COException_ValueError,
                                 "takes at most %d arguments (%d given)",
                                 ((COCodeObject *)code)->co_argcount -
                                 defaults_argcount, COList_GET_SIZE(args));
                    status = STATUS_EXCEPTION;
                }
                if (status != STATUS_NONE) {
                    goto fast_end;
                }
                int i = 0;
                int j = 0;
                for (; i < COList_GET_SIZE(args); i++) {
                    x = COTuple_GET_ITEM(args, i);
                    CO_INCREF(x);
                    frame->f_extraplus[i] = x;
                }
                j = i;
                i = 0;
                for (; i < defaults_argcount; i++) {
                    x = COTuple_GET_ITEM(defaults, i);
                    CO_INCREF(x);
                    frame->f_extraplus[j + i] = x;
                }
                CO_DECREF(args);

                CO_DECREF(o1);
                goto new_frame;
            } else {
                x = COObject_Call(o1, args);
                CO_DECREF(args);
                CO_DECREF(o1);
                if (!x) {
                    status = STATUS_EXCEPTION;
                    goto fast_end;
                }
                PUSH(x);
            }
            break;
        case OP_RETURN:
            o1 = POP();
            frame->f_stacktop = stack_top;
            frame->f_lasti = (int)(next_code - first_code);
            COFrameObject *old_frame = (COFrameObject *)frame;
            frame = (COFrameObject *)old_frame->f_prev;
            CO_DECREF(old_frame);
            if (!frame) {
                CO_DECREF(o1);
                goto vm_exit;
            }
            // init function return
            *(frame->f_stacktop++) = o1;
            goto new_frame;
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
        case OP_STORE_SUBSCRIPT:
            o1 = TOP();
            o2 = SECOND();
            o3 = THIRD();
            STACK_ADJ(-3);
            if (COMapping_SetItem(o3, o2, o1) != 0) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            CO_DECREF(o3);
            break;
        case OP_GET_ITER:
            o1 = TOP();
            x = COObject_GetIter(o1);
            CO_DECREF(o1);
            SET_TOP(x);
            break;
        case OP_FOR_ITER:
            oparg = NEXTARG();
            o1 = TOP();
            x = COObject_IterNext(o1);
            if (x) {
                PUSH(x);
                break;
            }
            o1 = POP();
            CO_DECREF(o1);
            JUMPTO(oparg);
            break;
        case OP_GET_ATTR:
            o1 = POP();
            o2 = TOP();
            x = COObject_GetAttr(o2, o1);
            if (!x) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            CO_INCREF(x);
            SET_TOP(x);
            break;
        case OP_SET_ATTR:
            o1 = TOP();
            o2 = SECOND();
            o3 = THIRD();
            STACK_ADJ(-3);
            if (COObject_SetAttr(o3, o2, o1) != 0) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            CO_DECREF(o1);
            CO_DECREF(o2);
            CO_DECREF(o3);
            break;
        case OP_MAKE_CLASS:
            {
                o1 = POP();
                o2 = TOP();
                COObject *dict = CODict_New();
                x = COObject_Call(o1, COTuple_Pack(1, dict));
                if (!x) {
                    status = STATUS_EXCEPTION;
                    goto fast_end;
                }
                COObject *args = COTuple_Pack(2, o2, dict);
                x = COObject_Call((COObject *)&COType_Type, args);
                CO_DECREF(o1);
                CO_DECREF(o2);
                SET_TOP(x);
            }
            break;
        case OP_IMPORT_NAME:
            oparg = NEXTARG();
            o1 = GETITEM(names, oparg);
            o2 = module_import(o1);
            if (!o2) {
                status = STATUS_EXCEPTION;
                goto fast_end;
            }
            PUSH(o2);
            break;
        case OP_IMPORT_FROM:
            oparg = NEXTARG();
            o1 = TOP();
            o2 = GETITEM(names, oparg);
            o2 = COObject_GetAttr(o1, o2);
            PUSH(o2);
            break;
        default:
            error("unknown handle for opcode(%ld)\n", opcode);
        }

fast_end:

        while (status != STATUS_NONE && frame->f_iblock > 0) {
            COFrameBlock *fb =
                &frame->f_blockstack[frame->f_iblock - 1];
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
    while (frame) {
        COFrameObject *tmp_frame = (COFrameObject *)frame->f_prev;
        CO_DECREF(frame);
        frame = tmp_frame;
    }

    return x;
}
