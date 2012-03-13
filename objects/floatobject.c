#include "floatobject.h"

static struct COObject *
float_repr(struct COFloatObject *this)
{
    char buffer[32];
    int n = snprintf(buffer, 32, "%g", this->co_fval);
    return COStr_FromStingN(buffer, n);
}

struct COTypeObject COFloat_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "float",
    sizeof(struct COFloatObject),
    0,
    (reprfunc)float_repr,           /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

struct COObject *
COFloat_FromString(char *s)
{
    struct COFloatObject *f;

    f = xmalloc(sizeof(struct COFloatObject));
    CO_INIT(f, &COFloat_Type);
    f->co_fval = strtof(s, NULL);
    return (struct COObject *)f;
}
