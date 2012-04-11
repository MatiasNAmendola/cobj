#ifndef OBJECTS_LISTOBJECT_H
#define OBJECTS_LISTOBJECT_H

#include "../object.h"

typedef struct _COListObject {
    COObject_HEAD;
    /* 
     * Vector of pointers to list elements.
     *  - list[0] is co_item[0]
     *  - 0 <= co_size <= allocated_size
     *  - co_item == NULL implies co_size == allocated == 0
     */
    ssize_t co_size;
    COObject **co_item;
    ssize_t allocated;
} COListObject;

COTypeObject COList_Type;

#define COList_Check(co) (CO_TYPE(co) == &COList_Type)

COObject *COList_New(ssize_t size);
ssize_t COList_Size(COObject *this);
COObject *COList_GetItem(COObject *this, ssize_t index);
int COList_SetItem(COObject *this, ssize_t index, COObject *item);
int COList_DelItem(COObject *this, ssize_t index);
int COList_Insert(COObject *this, int index, COObject *item);
int COList_Append(COObject *this, COObject *item);
COObject *COList_AsTuple(COObject *this);
COObject *COList_GetSlice(COObject *this, int ilow, int ihigh);

#endif
