#include "functionobject.h"
#include "strobject.h"

static struct COObject *
function_repr(struct COFunctionObject *this)
{
    struct COObject *s;
    s = COStrObject_FromFormat("<function '%s'>", COStr_AsString(this->func_name));
    return s;
}

struct COTypeObject COFunction_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "function",
    sizeof(struct COFunctionObject),
    0,
    (reprfunc)function_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

struct COObject *
COObject_New(struct COObject *func_name)
{
    struct COFunctionObject *func;
    func = xmalloc(sizeof(struct COFunctionObject));
    CO_INIT(func, &COFunction_Type);
    func->func_name = func_name;
    return (struct COObject *)func;
}
