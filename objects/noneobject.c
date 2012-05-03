#include "../co.h"

static COObject *
none_repr(COObject *o)
{
    return COStr_FromString("None");
}

COTypeObject CONone_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "None",
    sizeof(long),
    0,
    default_dealloc,            /* tp_dealloc */
    none_repr,                  /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject _CO_None = COObject_HEAD_INIT(&CONone_Type);
