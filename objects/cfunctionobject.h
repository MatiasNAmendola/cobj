#ifndef OBJECTS_CFUNCTIONOBJECT_H
#define OBJECTS_CFUNCTIONOBJECT_H
/**
 * C Function Object
 */

#include "../object.h"

typedef COObject *(*COCFunction)(COObject *, COObject *);

typedef struct _COCFunctionObject {
    COObject_HEAD;
    const char *c_name;
    COCFunction c_func;
} COCFunctionObject;

COTypeObject COCFunction_Type;

#define COCFunction_Check(co) (CO_TYPE(co) == &COCFunction_Type)

/* Macros for direct access to common values. */
#define COCFunction_GET_FUNCTION(func) \
    (((COCFunctionObject *)func)->c_func)

COObject *COCFunction_New(const char *name, COCFunction func);

#endif
