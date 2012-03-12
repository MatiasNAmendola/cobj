#include "typeobject.h"

static struct COObject *
type_repr(struct COObject *this)
{
    // TODO
    return NULL;
}

struct COTypeObject COType_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "type",
    sizeof(struct COTypeObject),
    0,
    type_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

