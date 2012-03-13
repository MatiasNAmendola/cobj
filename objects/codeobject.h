#ifndef OBJECTS_CODEOBJECT_H
#define OBJECTS_CODEOBJECT_H

#include "../object.h"

typedef struct _COCodeObject {
    COObject_HEAD;
} COCodeObject;

COTypeObject COCode_Type;
COObject *COCodeObject_New(COObject *func_name);

#endif
