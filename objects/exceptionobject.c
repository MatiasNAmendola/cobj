#include "../cobj.h"

static COObject *
exception_repr(COExceptionObject *this)
{
    return COStr_FromString("<Exception>");
}

static void
exception_dealloc(COExceptionObject *this)
{
    CO_XDECREF(this->message);
    COObject_Mem_FREE(this);
}

COTypeObject COException_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "Exception",
    sizeof(COExceptionObject),
    0,
    0,
    0,                          /* tp_new */
    (deallocfunc)exception_dealloc,     /* tp_dealloc */
    (reprfunc)exception_repr,   /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_iter */
    0,                          /* tp_iternext */
    0,                          /* tp_len */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface */
};

COObject *COException = (COObject *)&COException_Type;

#define SimpleExtendsException(EXCBASE, EXCNAME)            \
    COTypeObject _COException_ ## EXCNAME = {               \
        COObject_HEAD_INIT(&COType_Type),                   \
        # EXCNAME,                                          \
        sizeof(COExceptionObject),                          \
        0,                                                  \
        0,                                                  \
        0,                                  /* tp_new */                \
        (deallocfunc)exception_dealloc,     /* tp_dealloc */            \
        (reprfunc)exception_repr,           /* tp_repr */               \
        0,                                  /* tp_print */              \
        0,                                  /* tp_hash */               \
        0,                                  /* tp_compare */            \
        0,                                  /* tp_traverse */           \
        0,                                  /* tp_clear */              \
        0,                                  /* tp_call */               \
        0,                                  /* tp_iter */               \
        0,                                  /* tp_iternext */           \
        0,                                  /* tp_len */                \
        0,                                  /* tp_arithmetic_interface */      \
        0,                                  /* tp_mapping_interface */  \
    };                                                      \
    COObject *COException_ ## EXCNAME = (COObject *)&_COException_ ## EXCNAME

SimpleExtendsException(COException, SystemError);
SimpleExtendsException(COException, NameError);
SimpleExtendsException(COException, TypeError);
SimpleExtendsException(COException, ValueError);
SimpleExtendsException(COException, OverflowError);
SimpleExtendsException(COException, MemoryError);
SimpleExtendsException(COException, UndefinedError);
SimpleExtendsException(COException, IndexError);
SimpleExtendsException(COException, KeyError);
