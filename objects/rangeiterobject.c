#include "../cobj.h"

static COObject *
rangeiter_repr(CORangeIterObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<rangeiter>");
    return s;
}

static COObject *
rangeiter_next(CORangeIterObject *this)
{
    if (this->index < this->len)
        /* Cast to unsigned to avoid possible signed overflow in intermediate
         * calculations. */
        return COInt_FromLong((long)
                              (this->start +
                               (unsigned long)(this->index++) * this->step));
    return NULL;
}

COTypeObject CORangeIter_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "rangeiter",
    sizeof(CORangeIterObject),
    0,
    0,
    0,                          /* tp_new               */
    0,                          /* tp_init */
    (deallocfunc)COObject_Mem_FREE,     /* tp_dealloc           */
    (reprfunc)rangeiter_repr,   /* tp_repr              */
    0,                          /* tp_print             */
    0,                          /* tp_hash              */
    0,                          /* tp_compare           */
    0,                          /* tp_traverse          */
    0,                          /* tp_clear             */
    0,                          /* tp_call              */
    (getiterfunc)COObject_GetSelf,      /* tp_iter              */
    (iternextfunc)rangeiter_next,       /* tp_iternext          */
    0,                          /* tp_arithmetic_interface     */
    0,                          /* tp_mapping_interface */
    0,                          /* tp_sequence_interface */
};
