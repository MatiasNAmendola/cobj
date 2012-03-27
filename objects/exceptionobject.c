#include "../co.h"

static COObject *
exception_repr(COExceptionObject *this)
{
    return COStr_FromString("<Exception>");
}

COTypeObject COException_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "Exception",
    sizeof(COExceptionObject),
    0,
    (reprfunc)exception_repr,   /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};

COObject *COException = (COObject *)&COException_Type;

#define SimpleExtendsException(EXCBASE, EXCNAME, EXCDOC)    \
    COTypeObject _COException_ ## EXCNAME = {               \
        COObject_HEAD_INIT(&COType_Type),                   \
        # EXCNAME,                                          \
        sizeof(COExceptionObject),                          \
        0,                                                  \
        (reprfunc)exception_repr,                           \
        0,                                                  \
        0,                                                  \
        0,                                                  \
    };                                                      \
    COObject *COException_ ## EXCNAME = (COObject *)&_COException_ ## EXCNAME

SimpleExtendsException(COException, SystemError, "test doc");
SimpleExtendsException(COException, NameError, "test doc");
