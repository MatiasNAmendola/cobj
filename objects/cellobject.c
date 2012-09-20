#include "../cobj.h"

static COObject *
cell_repr(COCellObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<cell>");
    return s;
}

static void
cell_dealloc(COCellObject *this)
{
    if (this->co_ref) {
        COObject_Mem_FREE(this->co_ref);
    }
    COObject_GC_Free(this);
}

static int
cell_traverse(COCellObject *this, visitfunc visit, void *arg)
{
    CO_VISIT(this->co_ref);
    return 0;
}

static int
cell_clear(COCellObject *this)
{
    CO_CLEAR(this->co_ref);
    return 0;
}

COTypeObject COCell_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "cell",
    sizeof(COCellObject),
    0,
    0,
    0,                          /* tp_alloc                */
    0,                          /* tp_init                 */
    (deallocfunc)cell_dealloc,  /* tp_dealloc              */
    (reprfunc)cell_repr,        /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    (traversefunc)cell_traverse,        /* tp_traverse             */
    (inquiryfunc)cell_clear,    /* tp_clear                */
    0,                          /* tp_call                 */
    0,                          /* tp_iter                 */
    0,                          /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface   */
};

COObject *
COCell_New(COObject *o)
{
    COCellObject *cell;
    cell = COObject_GC_NEW(COCellObject, &COCell_Type);
    if (!cell)
        return NULL;

    cell->co_ref = o;
    CO_XINCREF(o);
    COObject_GC_TRACK(cell);
    return (COObject *)cell;
}

COObject *
COCell_Get(COObject *this)
{
    if (!COCell_Check(this)) {
        COErr_BadInternalCall();
        return NULL;
    }
    CO_XINCREF(((COCellObject *)this)->co_ref);
    return COCell_GET(this);
}

int
COCell_Set(COObject *this, COObject *o)
{
    COObject *oldobj;
    if (!COCell_Check(this)) {
        COErr_BadInternalCall();
        return -1;
    }
    oldobj = COCell_GET(this);
    CO_XINCREF(o);
    COCell_SET(this, o);
    CO_XDECREF(oldobj);
    return 0;
}
