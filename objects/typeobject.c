#include "../co.h"

static COObject *
type_repr(COTypeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<type '%s'>", this->tp_name);
    return s;
}

COTypeObject COType_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "type",
    sizeof(COTypeObject),
    0,
    (reprfunc)type_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_richcompare */
    0,                          /* tp_int_interface */
};
