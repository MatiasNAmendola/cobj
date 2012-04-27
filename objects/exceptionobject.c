#include "../co.h"

static COObject *
exception_repr(COExceptionObject *this)
{
    return COStr_FromString("<Exception>");
}

static void
exception_dealloc(COExceptionObject *this)
{
    CO_XDECREF(this->message);
    COMem_FREE(this);
}

COTypeObject COException_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "Exception",
    sizeof(COExceptionObject),
    0,
    (deallocfunc)exception_dealloc,     /* tp_dealloc */
    (reprfunc)exception_repr,   /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
};

COObject *COException = (COObject *)&COException_Type;

#define SimpleExtendsException(EXCBASE, EXCNAME)            \
    COTypeObject _COException_ ## EXCNAME = {               \
        COObject_HEAD_INIT(&COType_Type),                   \
        # EXCNAME,                                          \
        sizeof(COExceptionObject),                          \
        0,                                                  \
        (deallocfunc)exception_dealloc,                     \
        (reprfunc)exception_repr,                           \
        0,                                                  \
        0,                                                  \
        0,                                                  \
    };                                                      \
    COObject *COException_ ## EXCNAME = (COObject *)&_COException_ ## EXCNAME

SimpleExtendsException(COException, SystemError);
SimpleExtendsException(COException, NameError);
SimpleExtendsException(COException, TypeError);
SimpleExtendsException(COException, ValueError);
SimpleExtendsException(COException, OverflowError);
SimpleExtendsException(COException, MemoryError);
SimpleExtendsException(COException, UndefinedError);
