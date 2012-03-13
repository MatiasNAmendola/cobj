#ifndef OBJECTS_LISTOBJECT_H
#define OBJECTS_LISTOBJECT_H

#include "../object.h"

typedef struct _COListObject {
    COVarObject_HEAD;
    /* 
     * Vector of pointers to list elements.
     *  - list[0] is co_item[0]
     *  - 0 <= co_size <= allocated_size
     *  - co_item == NULL implies co_size == allocated == 0
     */
    COObject **co_item;
    size_t allocated;
} COListObject;

COTypeObject COList_Type;

#endif
