#ifndef OBJECTS_TUPLEITEROBJECT_H
#define OBJECTS_TUPLEITEROBJECT_H

#include "../object.h"
#include "tupleobject.h"

typedef struct _COTupleIterObject {
    COObject_HEAD;
    long it_index;
    COTupleObject *it_seq;      /* Set to NULL when iterator is exhausted. */
} COTupleIterObject;

COTypeObject COTupleIter_Type;

#define COTupleIter_Check(co) (CO_TYPE(co) == &COTupleIter_Type)

#endif
