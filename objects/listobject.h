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

COObject *COList_New(size_t size);
size_t COList_Size(COListObject *this);
COObject *COList_GetItem(COListObject *this, size_t index);
int COList_SetItem(COListObject *this, size_t index, COObject *item);
int COList_Insert(COListObject *this, size_t index, COObject *item);
int COList_Append(COListObject *this, COObject *item);

#endif
