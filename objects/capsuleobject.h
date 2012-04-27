#ifndef OBJECTS_CAPSULEOBJECT_H
#define OBJECTS_CAPSULEOBJECT_H
/**
 * Capsule object interface
 */

#include "../object.h"

typedef void (*COCapsule_Destructor) (COObject *);

typedef struct _COCapsuleObject {
    COObject_HEAD;
    void *pointer;
    COCapsule_Destructor destructor;
} COCapsuleObject;

COTypeObject COCapsule_Type;

#define COCapsule_Check(co) (CO_TYPE(co) == &COCapsule_Type)

COObject *COCapsule_New(void *pointer, COCapsule_Destructor destructor);
#endif
