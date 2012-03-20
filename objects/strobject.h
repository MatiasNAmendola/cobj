#ifndef OBJECTS_STROBJECT_H
#define OBJECTS_STROBJECT_H
/**
 * String (str, bytes) object interface
 * 
 * The COStrObject represents a character string. An extra zero byte is reserved
 * at the end to ensure it is zero-terminated, but a size is present so strings
 * with null bytes in them can be represented.
 * 
 * This is immutable object type.
 */
#include "../object.h"

typedef struct _COStrObject {
    COVarObject_HEAD;
    long co_shash;
    char co_sval[1];

    /* Invariants:
     *  - co_sval contains space for 'co_size + 1' chars
     *  - co_sval[co_size] == '\0'
     *  - co_shash is the hash of string or -1 if not computed yet
     */
} COStrObject;

COTypeObject COStr_Type;

#define COStr_Check(co) (CO_TYPE(co) == &COStr_Type)

char *COStr_AsString(COObject *co);
COObject *COStr_FromString(const char *s);
COObject *COStr_FromStringN(const char *s, size_t len);
COObject *COStr_FromFormat(const char *fmt, ...);
void COStr_Concat(COObject **pv, COObject *s);
COObject *COStr_Repr(COObject *s, int smartquotes);

#endif
