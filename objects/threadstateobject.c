#include "../cobj.h"

static COObject *
threadstate_repr(COThreadStateObject *this)
{
    return COStr_FromString("<threadstate>");
}

static void
threadstate_dealloc(COThreadStateObject *this)
{
    CO_DECREF(this->modules);
    CO_DECREF(this->module_search_path);
    COObject_GC_Free(this);
}

static int
threadstate_traverse(COThreadStateObject *this, visitfunc visit, void *arg)
{
    CO_VISIT(this->modules);
    CO_VISIT(this->module_search_path);
    return 0;
}

static int
threadstate_clear(COThreadStateObject *this)
{
    CO_CLEAR(this->modules);
    CO_CLEAR(this->module_search_path);
    return 0;
}


COTypeObject COThreadState_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "threadstate",
    sizeof(COThreadStateObject),
    0,
    COType_FLAG_GC,
    0,                                  /* tp_alloc                */
    0,                                  /* tp_init                 */
    (deallocfunc)threadstate_dealloc,   /* tp_dealloc              */
    (reprfunc)threadstate_repr,         /* tp_repr                 */
    0,                                  /* tp_print                */
    0,                                  /* tp_hash                 */
    0,                                  /* tp_compare              */
    (traversefunc)threadstate_traverse, /* tp_traverse             */
    (inquiryfunc)threadstate_clear,     /* tp_clear                */
    0,                                  /* tp_call                 */
    0,                                  /* tp_iter                 */
    0,                                  /* tp_iternext             */
    0,                                  /* tp_arithmetic_interface */
    0,                                  /* tp_mapping_interface    */
    0,                                  /* tp_sequence_interface   */
};

COThreadStateObject *
COThreadState_New(void)
{
    COThreadStateObject *ts = COObject_GC_NEW(COThreadStateObject, &COThreadState_Type);
    if (!ts)
        return NULL;

    ts->modules = CODict_New();
    ts->module_search_path = COList_New(0);
    COObject_GC_TRACK(ts);
    return ts;
}
