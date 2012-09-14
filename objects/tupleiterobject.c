#include "../cobj.h"

static COObject *
tupleiter_repr(COTupleIterObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<tupleiter>");
    return s;
}

static void
tupleiter_dealloc(COTupleIterObject *this)
{
    if (this->it_seq) {
        COObject_Mem_FREE(this->it_seq);
    }
    COObject_GC_Free(this);
}

static int
tupleiter_traverse(COTupleIterObject *this, visitfunc visit, void *arg)
{
    CO_VISIT(this->it_seq);
    return 0;
}

static int
tupleiter_clear(COTupleIterObject *this)
{
    CO_CLEAR(this->it_seq);
    return 0;
}

static COObject *
tupleiter_next(COTupleIterObject *it)
{
    COTupleObject *seq;
    COObject *item;
    seq = it->it_seq;
    if (!seq)
        return NULL;

    if (it->it_index < COTuple_GET_SIZE(seq)) {
        item = COTuple_GET_ITEM(seq, it->it_index);
        it->it_index++;
        CO_INCREF(item);
        return item;
    }
    CO_DECREF(seq);
    it->it_seq = NULL;
    return NULL;
}

COTypeObject COTupleIter_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "tupleiter",
    sizeof(COTupleIterObject),
    0,
    0,
    0,                          /* tp_alloc                  */
    0,                          /* tp_init */
    (deallocfunc)tupleiter_dealloc,     /* tp_dealloc              */
    (reprfunc)tupleiter_repr,   /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    (traversefunc)tupleiter_traverse,   /* tp_traverse             */
    (inquiryfunc)tupleiter_clear,       /* tp_clear                */
    0,                          /* tp_call                 */
    (getiterfunc)COObject_GetSelf,      /* tp_iter                 */
    (iternextfunc)tupleiter_next,       /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface */
};
