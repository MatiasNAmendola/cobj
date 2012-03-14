#include "../co.h"

static COObject *
frame_repr(COFrameObject *this)
{
    return COStr_FromString("<frame>");
}

COTypeObject COFrame_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "frame",
    sizeof(COFrameObject),
    0,
    (reprfunc)frame_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};

COFrameObject *
COFrame_New()
{
}
