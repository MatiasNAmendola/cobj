#include "../co.h"

static COObject *
float_repr(COFloatObject *this)
{
    char buffer[32];
    int n = snprintf(buffer, 32, "%g", this->co_fval);
    return COStr_FromStringN(buffer, n);
}

COTypeObject COFloat_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "float",
    sizeof(COFloatObject),
    0,
    (reprfunc)float_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_richcompare */
    0,                          /* tp_int_interface */
};

COObject *
COFloat_FromString(char *s)
{
    COFloatObject *f;

    f = COObject_New(COFloatObject, &COFloat_Type);
    f->co_fval = strtof(s, NULL);
    return (COObject *)f;
}
