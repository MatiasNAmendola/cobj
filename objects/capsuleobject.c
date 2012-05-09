#include "../co.h"

static COObject *
capsule_repr(COCapsuleObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<capsule>");
    return s;
}

static void
capsule_dealloc(COCapsuleObject *this)
{
    if (this->destructor) {
        this->destructor(this->pointer);
    }
    COMem_FREE(this);
}

COTypeObject COCapsule_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "capsule",
    sizeof(COCapsuleObject),
    0,
    (deallocfunc)capsule_dealloc,       /* tp_dealloc */
    (reprfunc)capsule_repr,     /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject *
COCapsule_New(void *pointer, COCapsule_Destructor destructor)
{
    COCapsuleObject *capsule;
    capsule = COObject_NEW(COCapsuleObject, &COCapsule_Type);
    capsule->pointer = pointer;
    capsule->destructor = destructor;
    return (COObject *)capsule;
}
