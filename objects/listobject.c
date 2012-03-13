#include "../co.h"

static COObject *
list_repr(COListObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<list>");
    return s;
}

COTypeObject COList_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "list",
    sizeof(COListObject),
    0,
    (reprfunc)list_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};
