#ifndef OBJECTS_FRAMEOBJECT_H
#define OBJECTS_FRAMEOBJECT_H

#include "../object.h"

typedef struct _COFrameObject {
    COObject_HEAD;
} COFrameObject;

COTypeObject COFrame_Type;
COObject *COFrameObject_New(COObject *func_name);

#endif

