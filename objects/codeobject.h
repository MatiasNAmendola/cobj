#ifndef OBJECTS_CODEOBJECT_H
#define OBJECTS_CODEOBJECT_H
/**
 * Code object interface
 */

#include "../object.h"

typedef struct _COCodeObject {
    COObject_HEAD;
    COObject *co_code;          /* bytecodes */
    COObject *co_consts;        /* tuple of constants */
    COObject *co_names;         /* list of names */
} COCodeObject;

COTypeObject COCode_Type;

#endif
