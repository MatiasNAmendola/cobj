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
COFrame_New(void)
{
    ssize_t stacksize = 256;
    COFrameObject *f = COVarObject_New(COFrameObject, &COFrame_Type, stacksize);

    f->f_stacktop = f->f_stack;
    f->f_prev = NULL;
    return (COObject *)f;
}

void
COFrame_Destory(COObject *this)
{
    COMem_FREE(this);
}
