#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H
/**
 * Function Object
 *
 * Function objects are created by the execution of the "func" statement or
 * expression. Code object is purely syntactic object, nothing more than a
 * compiled version of some source code lines. There is one code object per
 * source code "fragment", but each code object can be referenced by zero or
 * many function objects.
 */

#include "../object.h"
#include "../compile.h"

typedef struct _COFunctionObject {
    COObject_HEAD;
    COObject *func_name;
    COObject *func_code;        /* code object */
    COObject *func_upvalues;    /* tuple of upvalue objects */
} COFunctionObject;

COTypeObject COFunction_Type;

#define COFunction_Check(co) (CO_TYPE(co) == &COFunction_Type)

COObject *COFunction_New(COObject *func_code);
void COFunction_SetCode(COObject *func, COObject *code);

#endif
