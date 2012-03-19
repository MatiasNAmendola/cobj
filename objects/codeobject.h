#ifndef OBJECTS_CODEOBJECT_H
#define OBJECTS_CODEOBJECT_H
/**
 * Code object interface
 */

#include "../object.h"

typedef struct _COCodeObject {
    COObject_HEAD;
    COObject *co_consts;        /* tuple of constants */
    COObject *co_names;         /* tuple of names */
    COObject *co_oplines;       /* tuple of oplines */
    uint    co_numoftmpvars;       
} COCodeObject;

COTypeObject COCode_Type;

COObject *COCode_New(COObject *oplines, COObject *consts, COObject *names);
#endif
