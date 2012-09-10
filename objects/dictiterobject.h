#ifndef OBJECTS_DICTITEROBJECT_H
#define OBJECTS_DICTITEROBJECT_H

#include "../object.h"

typedef struct _CODictIterObject {
    COObject_HEAD;
    CODictObject *it_dict;       /* Set to NULL when iterator is exhausted. */
    COObject *key;
    COObject *val;
} CODictIterObject;

COTypeObject CODictIter_Type;

#define CODictIter_Check(co) (CO_TYPE(co) == &CODictIter_Type)

#endif
