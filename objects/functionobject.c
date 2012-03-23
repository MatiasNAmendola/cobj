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
    0,                          /* tp_hash */
};

COObject *
COFunction_New(COObject *func_name)
{
    COFunctionObject *func = COObject_New(COFunctionObject, &COFunction_Type);
    if (!func_name) {
        func_name =
            anonymous_func_name ? anonymous_func_name : (anonymous_func_name =
                                                         COStr_FromString
                                                         ("anonymous"));
    }
    func->func_name = func_name;
    func->func_upvalues = CODict_New();
    func->func_code = NULL;
    return (COObject *)func;
}
