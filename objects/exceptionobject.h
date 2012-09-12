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
 *    - SystemError
 *    - NameError
 *    - TypeError
 *    - ValueError
 *    - OverflowError
 *    - MemoryError
 *    - UndefinedError
 *    - IndexError
 *    - KeyError
 *    - AttributeError
 */
COObject *COException;
COObject *COException_SystemError;
COObject *COException_NameError;
COObject *COException_TypeError;
COObject *COException_ValueError;
COObject *COException_OverflowError;
COObject *COException_MemoryError;
COObject *COException_UndefinedError;
COObject *COException_IndexError;
COObject *COException_KeyError;
COObject *COException_AttributeError;

#endif
