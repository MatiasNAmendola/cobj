#include "../co.h"

static COObject *
stack_repr(COFrameObject *this)
{
    return COStr_FromString("<stack>");
}

COTypeObject COFrame_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "stack",
    sizeof(COFrameObject),
    sizeof(COObject *),
    (reprfunc)stack_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
};

COObject *
COFrame_New(COObject *code)
{
    COCodeObject *_code = (COCodeObject *)code;
    COFrameObject *f =
        COVarObject_New(COFrameObject, &COFrame_Type, _code->co_stacksize);

    f->f_locals = CODict_New();

    f->f_stacktop = f->f_stack;
    f->f_prev = NULL;
    f->f_iblock = 0;
    return (COObject *)f;
}

void
COFrame_Destory(COObject *this)
{
    COMem_FREE(this);
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
