#include "../cobj.h"

static COObject *
cfunction_repr(COCFunctionObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<cfunction '%s'>", this->c_name);
    return s;
}

static void
cfunction_dealloc(COCFunctionObject *this)
{
    COObject_Mem_FREE(this);
}

COTypeObject COCFunction_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "cfunction",
    sizeof(COCFunctionObject),
    0,
    0,
    0,                          /* tp_new */
    (deallocfunc)cfunction_dealloc,     /* tp_dealloc */
    (reprfunc)cfunction_repr,   /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,                          /* tp_len */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface */
};
