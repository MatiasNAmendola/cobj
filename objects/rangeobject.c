#include "../cobj.h"

static COObject *
range_repr(CORangeObject *this)
{
    COObject *s;
    ssize_t istep;
    istep = COInt_AsSsize_t(this->step);
    if (istep == 1) {
        s = COStr_FromFormat("range(%d, %d)", COInt_AsLong(this->start),
                             COInt_AsLong(this->stop));
    } else {
        s = COStr_FromFormat("range(%d, %d, %d)", COInt_AsLong(this->start),
                             COInt_AsLong(this->stop),
                             COInt_AsLong(this->step));
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

/* Return length of items in range(lo, hi, step). step != 0 is required.
 * The result always fits in an unsigned long.
 */
static unsigned long
get_len_of_range(long lo, long hi, long step)
{
    assert(step != 0);

    /*
     * If step > 0 and lo >= hi, or step < 0 and lo <= hi, the range is empty.
     */
    if (step > 0 && lo < hi)
        return 1UL + (hi - 1UL - lo) / step;
    else if (step < 0 && lo > hi)
        return 1UL + (lo - 1UL - hi) / (0UL - step);
    else
        return 0UL;
}

/* Initialize a rangeiter object. If the length of the rangeiter object is not
 * representable as a C long, OverflowError is raised. */
static COObject *
fast_range_iter(long start, long stop, long step)
{
    CORangeIterObject *it = COObject_NEW(CORangeIterObject, &CORangeIter_Type);
    unsigned long ulen;
    if (!it)
        return NULL;

    ulen = get_len_of_range(start, stop, step);

    if (ulen > (unsigned long)LONG_MAX) {
        CO_DECREF(it);
        COErr_SetString(COException_OverflowError,
                        "range too large to represent as a range_iterator");
        return NULL;
    }

    it->start = start;
    it->step = step;
    it->len = (long)ulen;
    it->index = 0;
    return (COObject *)it;
}

static COObject *
range_iter(CORangeObject *this)
{
    return fast_range_iter(COInt_AsLong(this->start),
                           COInt_AsLong(this->stop), COInt_AsLong(this->step));
}

static COObject *
compute_range_length(COObject *start, COObject *stop, COObject *step)
{
    // simply case
    return COInt_FromLong((COInt_AsLong(stop) - COInt_AsLong(start)) /
                          COInt_AsLong(step));
}

static CORangeObject *
make_range_object(COTypeObject *type, COObject *start, COObject *stop,
                  COObject *step)
{
    CORangeObject *obj = NULL;
    COObject *length;
    length = compute_range_length(start, stop, step);
    if (!length)
        return NULL;

    obj = COObject_NEW(CORangeObject, type);
    if (!obj) {
        CO_DECREF(length);
        return NULL;
    }

    obj->start = start;
    obj->stop = stop;
    obj->step = step;
    obj->length = length;
    return obj;
}

/**
 * range(10)
 * range(10, 20)
 * range(10, 20, 2)
 */
static COObject *
range_new(COTypeObject *type, COObject *args)
{
    CORangeObject *obj;
    COObject *start = NULL, *stop = NULL, *step = NULL;

    if (COTuple_Size(args) < 1) {
        goto err;
    } else if (COTuple_Size(args) == 1) {
        if (!COObject_ParseArgs(args, &stop, NULL))
            return NULL;
        start = COInt_FromLong(0);
        if (!start) {
            goto err;
        }
        step = COInt_FromLong(1);
        if (!step) {
            goto err;
        }
    } else if (COTuple_Size(args) == 2 || COTuple_Size(args) == 3) {
        if (!COObject_ParseArgs(args, &start, &stop, &step, NULL))
            return NULL;
        if (!step) {
            step = COInt_FromLong(1);
        }
        if (!step)
            goto err;
    } else {
        COErr_SetString(COException_TypeError,
                        "range expected at most 3 arguments");
        goto err;
    }

    obj = make_range_object(type, start, stop, step);
    if (!obj)
        goto err;

    return (COObject *)obj;
err:
    CO_XDECREF(start);
    CO_XDECREF(stop);
    CO_XDECREF(step);
    return NULL;
}

COTypeObject CORange_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "range",
    sizeof(CORangeObject),
    0,
    0,
    (newfunc)range_new,         /* tp_new     */
    (deallocfunc)range_dealloc, /* tp_dealloc */
    (reprfunc)range_repr,       /* tp_repr    */
    0,                          /* tp_print   */
    0,                          /* tp_hash              */
    0,                          /* tp_compare           */
    0,                          /* tp_traverse          */
    0,                          /* tp_clear             */
    0,                          /* tp_call              */
    (getiterfunc)range_iter,    /* tp_iter              */
    0,                          /* tp_iternext          */
    0,                          /* tp_len */
    0,                          /* tp_arithmetic_interface     */
    0,                          /* tp_mapping_interface */
};
