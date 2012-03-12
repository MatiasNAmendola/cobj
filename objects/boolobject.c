#include "boolobject.h"
#include "typeobject.h"

static struct COObject *
bool_repr(struct COObject *this)
{
    return NULL;
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

struct COIntObject _CO_False = {
    COObject_HEAD_INIT(&COBool_Type),
    0,
};

struct COIntObject _CO_True = {
    COObject_HEAD_INIT(&COBool_Type),
    1,
};
