#include "../co.h"

static COObject *
type_repr(COTypeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<type '%s'>", this->tp_name);
    return s;
}

static void
type_dealloc(COTypeObject *this)
{
    COObject_Mem_FREE(this);
}

static COObject *
type_call(COTypeObject *type, COObject *args)
{
    COObject *o;
    if (!type->tp_make) {
        COErr_Format(COException_TypeError, "cannot make '%.100s' instance", type->tp_name);
        return NULL;
    }

    o = type->tp_make(type, args);
    return o;
}

COTypeObject COType_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "type",
    sizeof(COTypeObject),
    0,
    0,
    0,                          /* tp_make */
    (deallocfunc)type_dealloc,  /* tp_dealloc */
    (reprfunc)type_repr,        /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    (binaryfunc)type_call,      /* tp_call */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

/*
 * If object should never be deallocated, use this.
 */
void
default_dealloc(COObject *this)
{
    fprintf(stderr, "deallocated, object is:\n");
    COObject_Dump(this);
    exit(0);
}
