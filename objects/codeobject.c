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

static void
code_dealloc(COCodeObject *this)
{
    CO_DECREF(this->co_name);
    CO_DECREF(this->co_code);
    CO_DECREF(this->co_consts);
    CO_DECREF(this->co_names);
    COObject_Mem_FREE(this);
}

COTypeObject COCode_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "code",
    sizeof(COCodeObject),
    0,
    0,
    0,                          /* tp_make */
    (deallocfunc)code_dealloc,  /* tp_dealloc */
    (reprfunc)code_repr,        /* tp_repr */
    0,                          /* tp_print */
    (hashfunc)code_hash,        /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject *
COCode_New(COObject *name, COObject *code, COObject *consts,
           COObject *names, COObject *localnames, int argcount, int stacksize,
           int nlocals)
{
    COCodeObject *co = COObject_NEW(COCodeObject, &COCode_Type);

    co->co_name = name;
    CO_INCREF(name);
    co->co_code = code;
    CO_INCREF(code);
    co->co_consts = consts;
    CO_INCREF(consts);
    co->co_names = names;
    CO_INCREF(names);
    co->co_localnames = localnames;
    co->co_argcount = argcount;
    co->co_stacksize = stacksize;
    co->co_nlocals = nlocals;

    return (COObject *)co;
}
