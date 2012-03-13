#include "../co.h"

static COObject *
float_repr(COFloatObject *this)
{
    char buffer[32];
    int n = snprintf(buffer, 32, "%g", this->co_fval);
    return COStr_FromStingN(buffer, n);
}

COTypeObject COFloat_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "float",
    sizeof(COFloatObject),
    0,
    (reprfunc)float_repr,           /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

COObject *
COFloat_FromString(char *s)
{
    COFloatObject *f;

    f = xmalloc(sizeof(COFloatObject));
    CO_INIT(f, &COFloat_Type);
    f->co_fval = strtof(s, NULL);
    return (COObject *)f;
}
