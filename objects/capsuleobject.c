#include "../co.h"

static COObject *
capsule_repr(COCapsuleObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<capsule>");
    return s;
}

COTypeObject COCapsule_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "capsule",
    sizeof(COCapsuleObject),
    0,
    (reprfunc)capsule_repr,     /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_int_interface */
};

COObject *
COCapsule_New(void *pointer)
{
    COCapsuleObject *capsule;
    capsule = COObject_New(COCapsuleObject, &COCapsule_Type);
    capsule->pointer = pointer;
    return (COObject *)capsule;
}
