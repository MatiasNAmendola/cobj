#include "../cobj.h"

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
    CO_DECREF(this->func_name);
    CO_DECREF(this->func_code);
    CO_XDECREF(this->func_defaults);
    CO_XDECREF(this->func_upvalues);
    COObject_Mem_FREE(this);
}

COTypeObject COFunction_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "function",
    sizeof(COFunctionObject),
    0,
    0,
    0,                          /* tp_new                  */
    0,                          /* tp_init */
    (deallocfunc)function_dealloc,      /* tp_dealloc              */
    (reprfunc)function_repr,    /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    0,                          /* tp_traverse             */
    0,                          /* tp_clear                */
    0,                          /* tp_call                 */
    0,                          /* tp_iter                 */
    0,                          /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface   */
};

COObject *
COFunction_New(COObject *func_code)
{
    COFunctionObject *func = COObject_NEW(COFunctionObject, &COFunction_Type);
    COObject *func_name =
        anonymous_func_name ? anonymous_func_name : (anonymous_func_name =
                                                     COStr_FromString
                                                     ("anonymous"));
    func->func_name = func_name;
    CO_XINCREF(func_name);
    func->func_code = func_code;
    CO_XINCREF(func_code);

    func->func_defaults = NULL;
    ssize_t nupvals = COTuple_GET_SIZE(((COCodeObject *)func_code)->co_upvals);
    func->func_upvalues = NULL;
    if (nupvals) {
        func->func_upvalues = COTuple_New(nupvals);
    }
    return (COObject *)func;
}

void
COFunction_SetCode(COObject *func, COObject *code)
{
    COFunctionObject *this = (COFunctionObject *)func;
    CO_XDECREF(this->func_code);
    this->func_code = code;
    CO_XINCREF(code);
}
