#ifndef OBJECTS_MODULEOBJECT_H
#define OBJECTS_MODULEOBJECT_H
/**
 * Module Object.
 */

#include "../object.h"

typedef struct _COModuleObject {
    COObject_HEAD;
    COObject *md_dict;
} COModuleObject;

COTypeObject COModule_Type;

#define COModule_Check(co) (CO_TYPE(co) == &COModule_Type)

COObject * COModule_New(COObject *name);
COObject *COModule_GetDict(COObject *this);
#endif
