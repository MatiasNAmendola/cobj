#include "../cobj.h"

static COObject *
none_repr(COObject *o)
{
    return COStr_FromString("None");
}

COTypeObject CONone_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "None",
    sizeof(long),
    0,
    0,
    0,                          /* tp_new */
    0,                          /* tp_init */
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
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface */
    0,                          /* tp_sequence_interface */
};

COObject _CO_None = COObject_HEAD_INIT(&CONone_Type);
