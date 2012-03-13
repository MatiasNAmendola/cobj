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

    return s;
}

struct COTypeObject COBool_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "bool",
    sizeof(long),
    0,
    bool_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
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
    COObject_HEAD_INIT(&COBool_Type),
    0,
};

COIntObject _CO_True = {
    COObject_HEAD_INIT(&COBool_Type),
    1,
};
