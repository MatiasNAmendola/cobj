#include "../cobj.h"

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
    0,
    0,                          /* tp_new */
    default_dealloc,            /* tp_dealloc */
    none_repr,                  /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject _CO_None = COObject_HEAD_INIT(&CONone_Type);
