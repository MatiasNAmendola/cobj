#ifndef OBJECTS_BYTEARRAYOBJECT_H
#define OBJECTS_BYTEARRAYOBJECT_H
/**
 * Bytearray object interface
 * 
 * The COBytesObject represents a mutable array of bytes.
 * A byte is from 0 to 255.
 */
#include "../object.h"

typedef struct _COBytesObject {
    COVarObject_HEAD;
    size_t co_alloc;            /* bytes allocaed */
    char *co_bytes;
} COBytesObject;

COTypeObject COBytes_Type;

char *COBytes_AsString(COObject *co);
COObject *COBytes_FromString(const char *bytes);
COObject *COBytes_FromStringN(const char *bytes, size_t len);
void COBytes_Concat(COObject **pv, COObject *bytes);
int COBytes_Resize(COObject *this, size_t newsize);

#endif
