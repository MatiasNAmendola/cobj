#ifndef OBJECTS_LISTITEROBJECT_H
#define OBJECTS_LISTITEROBJECT_H

#include "../object.h"

typedef struct _COListIterObject {
    COObject_HEAD;
    long it_index;
    COListObject *it_seq;   /* Set to NULL when iterator is exhausted. */
} COListIterObject;

COTypeObject COListIter_Type;

#define COListIter_Check(co) (CO_TYPE(co) == &COListIter_Type)

#endif
