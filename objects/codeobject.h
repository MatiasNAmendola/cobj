#ifndef OBJECTS_CODEOBJECT_H
#define OBJECTS_CODEOBJECT_H
/**
 * Code object interface
 */

#include "../object.h"

typedef struct _COCodeObject {
    COObject_HEAD;
    COObject *co_name;
    COObject *co_code;          /* byte object, bytecode */
    COObject *co_consts;        /* tuple, constants */
    COObject *co_names;         /* tuple, names in code */
    int co_argcount;            /* arguments count */
    int co_stacksize;           /* stack size needed */
    int co_nlocals;             /* number of local variables */
} COCodeObject;

COTypeObject COCode_Type;

#define COCode_Check(co) (CO_TYPE(co) == &COCode_Type)

COObject *COCode_New(COObject *name, COObject *code, COObject *consts,
                     COObject *names, int argcount, int stacksize);
#endif
