#include "../co.h"

static COObject *anonymous_func_name = NULL;

static COObject *
function_repr(COFunctionObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<function '%s'>", COStr_AsString(this->func_name));
    return s;
}

COTypeObject COFunction_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "function",
    sizeof(COFunctionObject),
    0,
    (reprfunc)function_repr,    /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};

COObject *
COFunctionObject_New(COObject *func_name)
{
    COFunctionObject *func;
    func = xmalloc(sizeof(COFunctionObject));
    CO_INIT(func, &COFunction_Type);
    if (!func_name) {
        func_name =
            anonymous_func_name ? anonymous_func_name : (anonymous_func_name =
                                                         COStr_FromString
                                                         ("anonymous"));
    }
    func->func_name = func_name;
    return (COObject *)func;
}
