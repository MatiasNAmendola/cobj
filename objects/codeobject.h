#ifndef OBJECTS_CODEOBJECT_H
#define OBJECTS_CODEOBJECT_H

#include "../object.h"

typedef struct _COCodeObject {
    COObject_HEAD;
    COObject *co_code;          /* opcode array */
    COObject *co_consts;        /* list of constants */
} COCodeObject;

COTypeObject COCode_Type;
COObject *COCodeObject_New(COObject *func_name);

#endif
