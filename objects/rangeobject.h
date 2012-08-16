#ifndef OBJECTS_RANGEOBJECT_H
#define OBJECTS_RANGEOBJECT_H

#include "../object.h"

typedef struct _CORangeObject {
    COObject_HEAD;
    COObject *start;
    COObject *stop;
    COObject *step;
    COObject *length;
} CORangeObject;

COTypeObject CORange_Type;

#define CORange_Check(co) (CO_TYPE(co) == &CORange_Type)

#endif
