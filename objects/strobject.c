#include "strobject.h"
#include "typeobject.h"

static struct COObject *
str_repr(struct COObject *this)
{
    return this;
}

struct COTypeObject COStr_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "str",
    sizeof(struct COStrObject),
    0,
    str_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

