#ifndef OBJECTS_EXCEPTIONOBJECT_H
#define OBJECTS_EXCEPTIONOBJECT_H
/**
 * Exception object.
 */

#include "../object.h"

typedef struct _COExceptionObject {
    COObject_HEAD;
    COObject *message;
} COExceptionObject;

COTypeObject COException_Type;

#define COException_Check(co) (CO_TYPE(co) == &COException_Type)

/* Predefined exceptions
 *
 * Hierarchy:
 *  Exception
 *    - AttributeError
 *    - ImportError
 *    - IndexError
 *    - KeyError
 *    - MemoryError
 *    - NameError
 *    - OverflowError
 *    - SystemError
 *    - TypeError
 *    - UndefinedError
 *    - ValueError
 */
COObject *COException;
COObject *COException_AttributeError;
COObject *COException_ImportError;
COObject *COException_IndexError;
COObject *COException_KeyError;
COObject *COException_MemoryError;
COObject *COException_NameError;
COObject *COException_OverflowError;
COObject *COException_SystemError;
COObject *COException_TypeError;
COObject *COException_UndefinedError;
COObject *COException_ValueError;

#endif
