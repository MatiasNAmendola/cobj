#include "../co.h"

static COObject *
range_repr(CORangeObject *this)
{
    COObject *s;
    ssize_t istep;
    istep = COInt_AsSsize_t(this->step);
    if (istep == 1) {
        // TODO
        s = COStr_FromFormat("<range>");
    } else {
        // TODO
        s = COStr_FromFormat("<range>");
    }
    return s;
}

static void
range_dealloc(CORangeObject *this)
{
    CO_DECREF(this->start);
    CO_DECREF(this->stop);
    CO_DECREF(this->step);
    CO_DECREF(this->length);
    COObject_Mem_FREE(this);
}

static void
range_iter(CORangeObject *this)
{

}

static COObject *
range_make(COTypeObject *type, COObject *args)
{
    COObject *x = NULL;
    if (!COObject_ParseArgs(args, &x, NULL))
        return NULL;
    if (!x)
        return COStr_FromString("");
    return COObject_Str(x);
}

COTypeObject CORange_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "range",
    sizeof(CORangeObject),
    0,
    0,
    0,                          /* tp_make              */
    (deallocfunc)range_dealloc, /* tp_dealloc           */
    (reprfunc)range_repr,       /* tp_repr              */
    0,                          /* tp_print             */
    0,                          /* tp_hash              */
    0,                          /* tp_compare           */
    0,                          /* tp_traverse          */
    0,                          /* tp_clear             */
    0,                          /* tp_call              */
    (getiterfunc)range_iter,    /* tp_iter              */
    0,                          /* tp_iternext          */
    0,                          /* tp_int_interface     */
    0,                          /* tp_mapping_interface */
};
