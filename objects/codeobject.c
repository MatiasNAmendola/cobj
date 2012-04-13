#include "../co.h"

static COObject *
code_repr(COCodeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<code>");
    return s;
}

static long
code_hash(COCodeObject *co)
{
    long h, h0, h1, h2, h3;
    h0 = COObject_Hash(co->co_name);
    if (h0 == -1)
        return -1;
    h1 = COObject_Hash(co->co_code);
    if (h1 == -1)
        return -1;
    h2 = COObject_Hash(co->co_consts);
    if (h2 == -1)
        return -1;
    h3 = COObject_Hash(co->co_names);
    if (h3 == -1)
        return -1;
    h = h0 ^ h1 ^ h2 ^ h3 ^ co->co_argcount ^ co->co_stacksize;
    if (h == -1)
        h = -2;
    return h;
}

COTypeObject COCode_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "code",
    sizeof(COCodeObject),
    0,
    (reprfunc)code_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    (hashfunc)code_hash,        /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
};

COObject *
COCode_New(COObject *name, COObject *code, COObject *consts, COObject *names,
           int argcount, int stacksize)
{
    COCodeObject *co = COObject_New(COCodeObject, &COCode_Type);

    co->co_name = name;
    co->co_code = code;
    co->co_consts = consts;
    co->co_names = names;
    co->co_argcount = argcount;
    co->co_stacksize = stacksize;

    return (COObject *)co;
}
