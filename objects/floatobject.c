#include "../co.h"

static COObject *
float_repr(COFloatObject *this)
{
    char buffer[32];
    int n = snprintf(buffer, 32, "%g", this->co_fval);
    return COStr_FromStringN(buffer, n);
}

static void
float_dealloc(COFloatObject *this)
{
    COMem_FREE(this);
}

COTypeObject COFloat_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "float",
    sizeof(COFloatObject),
    0,
    (deallocfunc)float_dealloc, /* tp_dealloc */
    (reprfunc)float_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject *
COFloat_FromString(char *s)
{
    COFloatObject *f;

    f = COObject_New(COFloatObject, &COFloat_Type);
    f->co_fval = strtof(s, NULL);
    return (COObject *)f;
}
