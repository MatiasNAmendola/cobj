#include "../co.h"

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
    COMem_FREE(this);
}

COTypeObject COCFunction_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "cfunction",
    sizeof(COCFunctionObject),
    0,
    0,
    (deallocfunc)cfunction_dealloc,     /* tp_dealloc */
    (reprfunc)cfunction_repr,   /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};
