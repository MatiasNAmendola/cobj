#include "functionobject.h"

static struct COObject *anonymous_func_name = NULL;

static struct COObject *
function_repr(struct COFunctionObject *this)
{
    struct COObject *s;
    s = COStr_FromFormat("<function '%s'>", COStr_AsString(this->func_name));
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
COFunctionObject_New(struct COObject *func_name)
{
    struct COFunctionObject *func;
    func = xmalloc(sizeof(struct COFunctionObject));
    CO_INIT(func, &COFunction_Type);
    if (!func_name) {
        func_name = anonymous_func_name ? anonymous_func_name : (anonymous_func_name = COStr_FromString("anonymous"));
    }
    func->func_name = func_name;
    return (struct COObject *)func;
}
