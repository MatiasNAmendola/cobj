#ifndef OBJECTS_FLOATOBJECT_H
#define OBJECTS_FLOATOBJECT_H

#include "../object.h"

typedef struct _COFloatObject {
    COObject_HEAD;
    double co_fval;
} COFloatObject;

COTypeObject COFloat_Type;

COObject *COFloat_FromString(char *s);

#endif
