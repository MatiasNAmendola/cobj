#ifndef OBJECTS_RANGEITEROBJECT_H
#define OBJECTS_RANGEITEROBJECT_H

#include "../object.h"

typedef struct _CORangeIterObject {
    COObject_HEAD;
    long index;
    long start;
    long step;
    long len;
} CORangeIterObject;

COTypeObject CORangeIter_Type;

#define CORangeIter_Check(co) (CO_TYPE(co) == &CORangeIter_Type)

#endif
