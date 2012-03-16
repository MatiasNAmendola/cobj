#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H

#include "../object.h"
#include "../compile.h"

typedef struct _COFunctionObject {
    COObject_HEAD;
    COObject *func_name;
    COObject *upvalues;         /* dict of upvalues */
    COObject *co_code;          /* (new) string of bytecodes */
    struct co_opline_array *opline_array;
} COFunctionObject;

COTypeObject COFunction_Type;
COObject *COFunctionObject_New(COObject *func_name);

#endif
