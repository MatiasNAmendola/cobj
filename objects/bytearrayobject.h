#ifndef OBJECTS_BYTEARRAYOBJECT_H
#define OBJECTS_BYTEARRAYOBJECT_H
/**
 * Bytearray object interface
 * 
 * The COByteArrayObject represents a mutable array of bytes.
 * A byte is from 0 to 255.
 */
#include "../object.h"

typedef struct _COByteArrayObject {
    COVarObject_HEAD;
    size_t co_alloc;            /* bytes allocaed */
    char *co_bytes;
} COByteArrayObject;

COTypeObject COByteArray_Type;

char *COByteArray_AsString(COObject *co);
COObject *COByteArray_FromString(const char *bytes);
COObject *COByteArray_FromStringN(const char *bytes, size_t len);
void COByteArray_Concat(COObject **pv, COObject *bytes);
int COByteArray_Resize(COObject *this, size_t newsize);

#endif
