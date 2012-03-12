#include "typeobject.h"
#include "strobject.h"

static struct COObject *
type_repr(struct COTypeObject *this)
{
    struct COObject *s;
    s = COStrObject_FromString(this->tp_name);
    return s;
}

struct COTypeObject COType_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "type",
    sizeof(struct COTypeObject),
    0,
    (reprfunc)type_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};
