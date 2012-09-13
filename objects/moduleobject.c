#include "../cobj.h"

static COObject *
module_repr(COModuleObject *this)
{
    COObject *s;
    COObject *name = COObject_GetAttrString((COObject *)this, "__name__");
    if (!name)
        return NULL;
    s = COStr_FromFormat("<module '%s'>", COStr_AS_STRING(name));
    return s;
}

static void
module_dealloc(COModuleObject *this)
{
    CO_DECREF(this->md_dict);
    COObject_GC_Free(this);
}

static int
module_traverse(COModuleObject *this, visitfunc visit, void *arg)
{
    CO_VISIT(this->md_dict);
    return 0;
}

static int
module_clear(COModuleObject *this)
{
    CO_CLEAR(this->md_dict);
    return 0;
}

COTypeObject COModule_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "module",
    sizeof(COModuleObject),
    0,
    COType_FLAG_GC,
    0,                          /* tp_new                  */
    0,                          /* tp_init */
    (deallocfunc)module_dealloc,        /* tp_dealloc              */
    (reprfunc)module_repr,      /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    (traversefunc)module_traverse,      /* tp_traverse             */
    (inquiryfunc)module_clear,  /* tp_clear                */
    0,                          /* tp_call                 */
    0,                          /* tp_iter                 */
    0,                          /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface   */
    0,
    offsetof(COModuleObject, md_dict),
    0,
    0
};

COObject *
COModule_New(COObject *name)
{
    COModuleObject *this;
    this = COObject_GC_NEW(COModuleObject, &COModule_Type);
    if (!this)
        return NULL;
    this->md_dict = CODict_New();
    if (!this->md_dict)
        goto fail;
    if (CODict_SetItemString(this->md_dict, "__name__", name) != 0)
        goto fail;

    COObject_GC_TRACK(this);
    return (COObject *)this;

fail:
    CO_DECREF(this);
    return NULL;
}

COObject *
COModule_GetDict(COObject *this)
{
    return ((COModuleObject *)this)->md_dict;
}
