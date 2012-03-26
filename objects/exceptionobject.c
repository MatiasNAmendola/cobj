#include "../co.h"

static COObject *
exception_repr(COExceptionObject *this)
{
    return COStr_FromString("<Exception>");
}

COTypeObject COException_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "Exception",
    sizeof(COExceptionObject),
    0,
    (reprfunc)exception_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};
