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
    COObject_Mem_FREE(this);
}

COTypeObject COFloat_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "float",
    sizeof(COFloatObject),
    0,
    0,
    0,                          /* tp_new */
    (deallocfunc)float_dealloc, /* tp_dealloc */
    (reprfunc)float_repr,       /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject *
COFloat_FromString(char *s)
{
    COFloatObject *f;

    f = COObject_NEW(COFloatObject, &COFloat_Type);
    f->co_fval = strtof(s, NULL);
    return (COObject *)f;
}
