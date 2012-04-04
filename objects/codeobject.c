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
    0,                          /* tp_int_interface */
};

COObject *
COCode_New(COObject *code, COObject *consts, COObject *names, int argcount)
{
    COCodeObject *co = COObject_New(COCodeObject, &COCode_Type);

    co->co_code = code;
    co->co_consts = consts;
    co->co_names = names;
    co->co_argcount = argcount;

    return (COObject *)co;
}
