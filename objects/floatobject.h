#ifndef OBJECTS_FLOATOBJECT_H
#define OBJECTS_FLOATOBJECT_H

#include "../object.h"

struct COFloatObject {
    COObject_HEAD;
    double co_fval;
};

struct COTypeObject COFloat_Type;

COObject *COFloat_FromString(char *s);

#endif
