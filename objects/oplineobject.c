#include "../co.h"

static COObject *
opline_repr(COOplineObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<opline>");
    return s;
}

COTypeObject COOpline_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "opline",
    sizeof(COOplineObject),
    0,
    (reprfunc)opline_repr,      /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};

COObject *
COOpline_New(void)
{
    COOplineObject *co = COObject_New(COOplineObject, &COOpline_Type);

    return (COObject *)co;
}
