#include "../co.h"

static COObject *None_str = NULL;

static COObject *
none_repr(COObject *o)
{
    return None_str ? None_str : (None_str = COStr_FromString("None"));
}

COTypeObject CONone_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "none",
    sizeof(long),
    0,
    none_repr,                  /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_richcompare */
    0,                          /* tp_int_interface */
};

COObject _CO_None = COObject_HEAD_INIT(&CONone_Type);
