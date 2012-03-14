#ifndef OBJECTS_TUPLEOBJECT_H
#define OBJECTS_TUPLEOBJECT_H
/**
 * Tuple object interface
 * 
 * It's immutable. C code can change the tuple items, and use tuples as
 * general-purpose arrays of object references.
 */

#include "../object.h"

typedef struct _COTupleObject {
    COVarObject_HEAD;
    COObject **co_item;
} COTupleObject;

COTypeObject COTuple_Type;

COObject *COTuple_New(size_t size);
size_t COTuple_Size(COObject *this);
COObject *COTuple_GetItem(COObject *this, size_t index);
int COTuple_SetItem(COObject *this, size_t index, COObject *item);

/* Macros, trading safety for speed */
#define COTuple_GET_ITEM(co, i)     (((COTupleObject *)(co))->co_item[i])
// this *only* to be used to fill in brand new tuples 
#define COTuple_SET_ITEM(co, i, v)  (((COTupleObject *)(co))->co_item[i] = v)

#endif
