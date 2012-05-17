#include "../co.h"

static COObject *
frame_repr(COFrameObject *this)
{
    return COStr_FromString("<frame>");
}

static void
frame_dealloc(COFrameObject *this)
{
    CO_XDECREF(this->f_prev);
    CO_XDECREF(this->f_func);
    CO_XDECREF(this->f_locals);
    CO_XDECREF(this->f_builtins);

    /* free stack */
    COObject **p;
    if (this->f_stacktop != NULL) {
        for (p = this->f_stack; p < this->f_stacktop; p++) {
            CO_XDECREF(*p);
        }
    }

    COObject_Mem_FREE(this);
}

COTypeObject COFrame_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "frame",
    sizeof(COFrameObject),
    sizeof(COObject *),
    0,
    (deallocfunc)frame_dealloc, /* tp_dealloc */
    (reprfunc)frame_repr,       /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

static COObject *builtins = NULL;

int
COFrame_Init(void)
{
    if (builtins == NULL) {
        builtins = CODict_New();
        CODict_SetItemString(builtins, "print", (COObject *)&_CO_Builtin_print);
        CODict_SetItemString(builtins, "type", (COObject *)&COType_Type);
    }
    return 0;
}

void
COFrame_Fini(void)
{
    CO_XDECREF(builtins);
}

COObject *
COFrame_New(COObject *prev, COObject *func, COObject *locals)
{
    COFrameObject *f = COVarObject_NEW(COFrameObject, &COFrame_Type,
                                       ((COCodeObject *)((COFunctionObject *)
                                                         func)->func_code)->
                                       co_stacksize);

    f->f_lasti = 0;
    f->f_prev = prev;
    CO_XINCREF(prev);
    f->f_func = func;
    CO_XINCREF(func);

    if (locals) {
        f->f_locals = locals;
        CO_INCREF(locals);
    } else {
        f->f_locals = CODict_New();
    }
    f->f_builtins = builtins;
    CO_INCREF(builtins);

    f->f_stacktop = f->f_stack;
    f->f_iblock = 0;
    return (COObject *)f;
}

void
COFrameBlock_Setup(COFrameObject *f, int type, int handler, int level)
{
    COFrameBlock *fb;
    if (((COFrameObject *)f)->f_iblock >= FRAME_MAXBLOCKS)
        error("block stack overflow");
    fb = &f->f_blockstack[f->f_iblock++];
    fb->fb_type = type;
    fb->fb_level = level;
    fb->fb_handler = handler;
}

COFrameBlock *
COFrameBlock_Pop(COFrameObject *f)
{
    COFrameBlock *fb;
    if (f->f_iblock <= 0)
        error("block stack underflow");
    fb = &f->f_blockstack[--f->f_iblock];
    return fb;
}
