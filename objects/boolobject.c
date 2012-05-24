#include "../co.h"

static COObject *False_str = NULL;
static COObject *True_str = NULL;

static COObject *
bool_repr(COObject *this)
{
    COObject *s;

    if (this == CO_True) {
        s = True_str ? True_str : (True_str = COStr_FromString("True"));
    } else {
        s = False_str ? False_str : (False_str = COStr_FromString("False"));
    }

    CO_INCREF(s);
    return s;
}

COTypeObject COBool_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "bool",
    sizeof(long),
    0,
    0,
    0,                          /* tp_new */
    default_dealloc,            /* tp_dealloc */
    bool_repr,                  /* tp_repr */
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
COBool_FromLong(long ok)
{
    COObject *result;

    if (ok)
        result = CO_True;
    else
        result = CO_False;

    return result;
}

COIntObject _CO_False = {
    COVarObject_HEAD_INIT(&COBool_Type, 0),
    {0}
};

COIntObject _CO_True = {
    COVarObject_HEAD_INIT(&COBool_Type, 1),
    {1},
};
