#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H
/**
 * Function Object
 *
 * Running program itself is function `main` which is created by co with program
 * statements as code object. `main` can be refered by `this` keyword, just as
 * `this` in user-defined functions.
 *
 * Function has code object, code object can consists of functions.
 */

#include "../object.h"
#include "../compile.h"

typedef struct _COFunctionObject {
    COObject_HEAD;
    COObject *func_name;
    COObject *func_upvalues;    /* dict of upvalues */
    COObject *func_code;        /* code object */
} COFunctionObject;

COTypeObject COFunction_Type;

#define COFunction_Check(co) (CO_TYPE(co) == &COFunction_Type)

COObject *COFunction_New(COObject *func_name, COObject *func_code,
                         COObject *func_upvalues);

#endif
