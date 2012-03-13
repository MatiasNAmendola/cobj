#include "boolobject.h"
#include "strobject.h"

static struct COObject *False_str = NULL;
static struct COObject *True_str = NULL;

static struct COObject *
bool_repr(struct COObject *this)
{
    struct COObject *s;

    if (this == CO_True) {
        s = True_str ? True_str : (True_str = COStrObject_FromString("True"));
    } else {
        s = False_str ? False_str : (False_str = COStrObject_FromString("False"));
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

struct COObject *
COBool_FromLong(long ok)
{
    struct COObject *result;

    if (ok)
        result = CO_True;
    else
        result = CO_False;

    return result;
}

struct COIntObject _CO_False = {
    COObject_HEAD_INIT(&COBool_Type),
    0,
};

struct COIntObject _CO_True = {
    COObject_HEAD_INIT(&COBool_Type),
    1,
};
