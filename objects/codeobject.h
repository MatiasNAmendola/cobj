#ifndef OBJECTS_CODEOBJECT_H
#define OBJECTS_CODEOBJECT_H
/**
 * Code object interface
 */

#include "../object.h"

typedef struct _COCodeObject {
    COObject_HEAD;
    uint co_numoftmpvars;
    COObject *co_oplines;       /* tuple of oplines */
} COCodeObject;

COTypeObject COCode_Type;

#define COCode_Check(co) (CO_TYPE(co) == &COCode_Type)

COObject *COCode_New(COObject *oplines, size_t numoftmpvars);
#endif
