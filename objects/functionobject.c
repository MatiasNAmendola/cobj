#include "functionobject.h"

static struct COObject *
function_repr(struct COObject *this)
{
    return this;
}

struct COTypeObject COFunction_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "function",
    sizeof(struct COFunctionObject),
    0,
    function_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};
