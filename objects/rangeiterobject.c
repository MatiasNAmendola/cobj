#include "../co.h"

static COObject *
rangeiter_repr(CORangeIterObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<rangeiter>");
    return s;
}

COTypeObject CORangeIter_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "rangeiter",
    sizeof(CORangeIterObject),
    0,
    0,
    0,                              /* tp_make              */
    (deallocfunc)COObject_Mem_FREE, /* tp_dealloc           */
    (reprfunc)rangeiter_repr,       /* tp_repr              */
    0,                              /* tp_print             */
    0,                              /* tp_hash              */
    0,                              /* tp_compare           */
    0,                              /* tp_traverse          */
    0,                              /* tp_clear             */
    0,                              /* tp_call              */
    (getiterfunc)COObject_GetSelf, /* tp_iter              */
    0,                             /* tp_iternext          */
    0,                             /* tp_int_interface     */
    0,                             /* tp_mapping_interface */
};
