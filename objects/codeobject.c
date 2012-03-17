#include "../co.h"

static COObject *
code_repr(COCodeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<code>");
    return s;
}

COTypeObject COCode_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "code",
    sizeof(COCodeObject),
    0,
    (reprfunc)code_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};

COObject *
COCode_New(COObject *code, COObject *consts, COObject *names)
{
    COCodeObject *co = xmalloc(sizeof(COCodeObject));
    memset(co, 0, sizeof(COCodeObject));
    COObject_Init(co, &COCode_Type);

    co->co_consts = consts;
    co->co_names = names;

    return (COObject *)co;
}
