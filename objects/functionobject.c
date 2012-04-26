#include "../co.h"

static COObject *anonymous_func_name = NULL;

static COObject *
function_repr(COFunctionObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<function '%s'>", COStr_AsString(this->func_name));
    return s;
}

static void
function_dealloc(COFunctionObject *this)
{
    CO_XDECREF(this->func_name);
    CO_XDECREF(this->func_upvalues);
    CO_XDECREF(this->func_code);
    COMem_FREE(this);
}

COTypeObject COFunction_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "function",
    sizeof(COFunctionObject),
    0,
    (deallocfunc)function_dealloc,  /* tp_dealloc */
    (reprfunc)function_repr,        /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_hash */
    0,                              /* tp_compare */
    0,                              /* tp_int_interface */
};

COObject *
COFunction_New(COObject *func_code)
{
    COFunctionObject *func = COObject_New(COFunctionObject, &COFunction_Type);
    if (((COCodeObject *)func_code)->co_name) {
        func->func_name = ((COCodeObject *)func_code)->co_name;
    } else {
        func->func_name =
            anonymous_func_name ? anonymous_func_name : (anonymous_func_name =
                                                         COStr_FromString
                                                         ("anonymous"));
    }
    func->func_upvalues = CODict_New();
    func->func_code = func_code;
    return (COObject *)func;
}
