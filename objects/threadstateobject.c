#include "../cobj.h"

static COObject *
threadstate_repr(COThreadStateObject *this)
{
    return COStr_FromString("<threadstate>");
}

static void
threadstate_dealloc(COThreadStateObject *this)
{
    COObject_Mem_FREE(this);
}

COTypeObject COThreadState_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "threadstate",
    sizeof(COThreadStateObject),
    0,
    0,
    0,                                /* tp_alloc                */
    0,                                /* tp_init                 */
    (deallocfunc)threadstate_dealloc, /* tp_dealloc              */
    (reprfunc)threadstate_repr,       /* tp_repr                 */
    0,                                /* tp_print                */
    0,                                /* tp_hash                 */
    0,                                /* tp_compare              */
    0,                                /* tp_traverse             */
    0,                                /* tp_clear                */
    0,                                /* tp_call                 */
    0,                                /* tp_iter                 */
    0,                                /* tp_iternext             */
    0,                                /* tp_arithmetic_interface */
    0,                                /* tp_mapping_interface    */
    0,                                /* tp_sequence_interface   */
};

COObject *
COThreadState_New(void)
{
    COThreadStateObject *ts = COObject_NEW(COThreadStateObject, &COThreadState_Type);
    if (!ts)
        return NULL;

    ts->modules = CODict_New();
    ts->module_search_path = COList_New(0);
    return (COObject *)ts;
}
