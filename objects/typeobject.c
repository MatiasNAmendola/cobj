#include "../co.h"

static COObject *
type_repr(struct COTypeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<type '%s'>", this->tp_name);
    return s;
}

struct COTypeObject COType_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "type",
    sizeof(struct COTypeObject),
    0,
    (reprfunc)type_repr,            /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};
