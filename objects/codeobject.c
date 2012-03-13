#include "../co.h"

static COObject *
code_repr(COCodeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<code>");
    return s;
}

COTypeObject COCode_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "code",
    sizeof(COCodeObject),
    0,
    (reprfunc) code_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};
