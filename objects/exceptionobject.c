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
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "Exception",
    sizeof(COExceptionObject),
    0,
    0,
    0,                          /* tp_alloc                  */
    0,                          /* tp_init */
    (deallocfunc)exception_dealloc,     /* tp_dealloc              */
    (reprfunc)exception_repr,   /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    0,                          /* tp_traverse             */
    0,                          /* tp_clear                */
    0,                          /* tp_call                 */
    0,                          /* tp_iter                 */
    0,                          /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface   */
};

COObject *COException = (COObject *)&COException_Type;

#define SimpleExtendsException(EXCBASE, EXCNAME)            \
    COTypeObject _COException_ ## EXCNAME = {               \
        COVarObject_HEAD_INIT(&COType_Type, 0),                   \
        # EXCNAME,                                          \
        sizeof(COExceptionObject),                          \
        0,                                                  \
        0,                                                  \
        0,                                  /* tp_alloc */                \
        0,                                  /* tp_init */               \
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
        0,                                  /* tp_arithmetic_interface */      \
        0,                                  /* tp_mapping_interface */  \
    };                                                      \
    COObject *COException_ ## EXCNAME = (COObject *)&_COException_ ## EXCNAME

SimpleExtendsException(COException, AttributeError);
SimpleExtendsException(COException, ImportError);
SimpleExtendsException(COException, IndexError);
SimpleExtendsException(COException, KeyError);
SimpleExtendsException(COException, MemoryError);
SimpleExtendsException(COException, NameError);
SimpleExtendsException(COException, OverflowError);
SimpleExtendsException(COException, SystemError);
SimpleExtendsException(COException, TypeError);
SimpleExtendsException(COException, UndefinedError);
SimpleExtendsException(COException, ValueError);
