#include "../cobj.h"

static COObject *
listiter_repr(COListIterObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<listiter>");
    return s;
}

static void
listiter_dealloc(COListIterObject *this)
{
    if (this->it_seq) {
        COObject_Mem_FREE(this->it_seq);
    }
    COObject_GC_Free(this);
}

static int
listiter_traverse(COListIterObject *this, visitfunc visit, void *arg)
{
    CO_VISIT(this->it_seq);
    return 0;
}

static int
listiter_clear(COListIterObject *this)
{
    CO_CLEAR(this->it_seq);
    return 0;
}

static COObject *
listiter_next(COListIterObject *it)
{
    COListObject *seq;
    COObject *item;
    seq = it->it_seq;
    if (!seq)
        return NULL;

    if (it->it_index < COList_GET_SIZE(seq)) {
        item = COList_GET_ITEM(seq, it->it_index);
        it->it_index++;
        CO_INCREF(item);
        return item;
    }
    CO_DECREF(seq);
    it->it_seq = NULL;
    return NULL;
}

COTypeObject COListIter_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "listiter",
    sizeof(COListIterObject),
    0,
    0,
    0,                          /* tp_new */
    (deallocfunc)listiter_dealloc,      /* tp_dealloc */
    (reprfunc)listiter_repr,    /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    (traversefunc)listiter_traverse,    /* tp_traverse */
    (inquiryfunc)listiter_clear,        /* tp_clear */
    0,                          /* tp_call */
    (getiterfunc)COObject_GetSelf,      /* tp_iter */
    (iternextfunc)listiter_next,        /* tp_iternext */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface */
};
