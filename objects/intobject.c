#include "intobject.h"

static struct COObject *
int_repr(struct COIntObject *this)
{
    long n = this->co_ival;
    return NULL;
}

struct COTypeObject COInt_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "int",
    sizeof(struct COIntObject),
    0,
    (reprfunc)int_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};
