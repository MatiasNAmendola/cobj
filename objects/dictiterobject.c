#include "../cobj.h"

static COObject *
dictiter_repr(CODictIterObject *this)
{
    return COStr_FromFormat("<dictiter>");
}

static void
dictiter_dealloc(CODictIterObject *this)
{
    CO_XDECREF(this->it_dict);
    COObject_GC_Free(this);
}

static int
dictiter_traverse(CODictIterObject *this, visitfunc visit, void *arg)
{
    CO_VISIT(this->it_dict);
    return 0;
}

static int
dictiter_clear(CODictIterObject *this)
{
    CO_CLEAR(this->it_dict);
    return 0;
}

static COObject *
dictiter_next(CODictIterObject *it)
{
    if (!it->it_dict)
        return NULL;

    if (CODict_Next((COObject *)it->it_dict, &it->key, &it->val) == 0) {
        CO_INCREF(it->key);
        return it->key;
    }
    CO_DECREF(it->it_dict);
    it->it_dict = NULL;
    return NULL;
}

COTypeObject CODictIter_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "dictiter",
    sizeof(CODictIterObject),
    0,
    0,
    0,                               /* tp_new                  */
    (deallocfunc)dictiter_dealloc,   /* tp_dealloc              */
    (reprfunc)dictiter_repr,         /* tp_repr                 */
    0,                               /* tp_print                */
    0,                               /* tp_hash                 */
    0,                               /* tp_compare              */
    (traversefunc)dictiter_traverse, /* tp_traverse             */
    (inquiryfunc)dictiter_clear,     /* tp_clear                */
    0,                               /* tp_call                 */
    (getiterfunc)COObject_GetSelf,   /* tp_iter                 */
    (iternextfunc)dictiter_next,     /* tp_iternext             */
    0,                               /* tp_arithmetic_interface */
    0,                               /* tp_mapping_interface    */
};
