#include "../cobj.h"

static COObject *
type_repr(COTypeObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<class '%s'>", this->tp_name);
    return s;
}

static void
type_dealloc(COTypeObject *type)
{
    /* Only heap-allocated type objects will be allocated. */
    assert(type->tp_flags & COType_FLAG_HEAP);

    CO_XDECREF(type->tp_dict);
    CO_XDECREF(type->tp_base);

    COHeapTypeObject *htp = (COHeapTypeObject *)type;
    CO_XDECREF(htp->ht_name);

    COObject_Mem_FREE(type);
}

static COObject *
type_call(COTypeObject *type, COObject *args)
{
    COObject *o;
    if (!type->tp_alloc) {
        COErr_Format(COException_TypeError, "cannot make '%.100s' instance",
                     type->tp_name);
        return NULL;
    }

    o = type->tp_alloc((COObject *)type, args);

    if (o) {
        type = CO_TYPE(o);
        if (type->tp_init && type->tp_init(o, args) < 0) {
            CO_DECREF(o);
            o = NULL;
        }
    }
    return o;
}

static COObject *
type_new(COTypeObject *type, COObject *args)
{
    COObject *x = NULL;

    assert(args != NULL && COTuple_Check(args));

    if (COTuple_GET_SIZE(args) == 1) {
        if (!COObject_ParseArgs(args, &x, NULL))
            return NULL;
        x = (COObject *)CO_TYPE(x);
        CO_INCREF(x);
        return x;
    }

    /* Check arguments: (name, dict) */
    COObject *name;
    COObject *dict;
    if (!COObject_ParseArgs(args, &name, &dict, NULL))
        return NULL;

    const ssize_t size = COVarObject_SIZE(type, 0);
    COHeapTypeObject *htp = (COHeapTypeObject *)COObject_Mem_MALLOC(size);
    if (htp == NULL)
        return COErr_NoMemory();

    memset(htp, 0, size);
    COVarObject_INIT(htp, type, 0);

    CO_INCREF(name);
    htp->ht_name = name;

    COTypeObject *tp = (COTypeObject *)htp;
    tp->tp_flags = COType_FLAG_DEFAULT | COType_FLAG_HEAP;
    tp->tp_name = COStr_AS_STRING(name);
    tp->tp_dict = CODict_New();

    // base type (now defaults to COObject_Type)
    COTypeObject *base = &COObject_Type;
    tp->tp_base = base;

    // add dict
    tp->tp_dictoffset = base->tp_basicsize;
    tp->tp_basicsize = base->tp_basicsize;
    tp->tp_basicsize += sizeof(COObject *);
    tp->tp_dict = dict;

    /* Initialize the rest. */
    if (COType_Ready((COObject *)tp) < 0)
        goto error;

    return (COObject *)tp;

error:
    return NULL;
}

COTypeObject COType_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "type",
    sizeof(COHeapTypeObject),
    0,
    0,
    (newfunc)type_new,          /* tp_alloc                  */
    0,                          /* tp_init */
    (deallocfunc)type_dealloc,  /* tp_dealloc              */
    (reprfunc)type_repr,        /* tp_repr                 */
    0,                          /* tp_print                */
    0,                          /* tp_hash                 */
    0,                          /* tp_compare              */
    0,                          /* tp_traverse             */
    0,                          /* tp_clear                */
    (binaryfunc)type_call,      /* tp_call                 */
    0,                          /* tp_iter                 */
    0,                          /* tp_iternext             */
    0,                          /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface   */
    0,                          /* tp_dict */
    offsetof(COTypeObject, tp_dict),    /* tp_dictoffset */
    0,                           /* tp_base                 */
    0,                           /* tp_methods              */
    0,                           /* tp_members              */
};

static COObject *
object_new(COTypeObject *type, COObject *args)
{
    COObject *obj;
    const ssize_t size = COVarObject_SIZE(type, 0);

    obj = (COObject *)COObject_Mem_MALLOC(size);
    if (obj == NULL)
        return COErr_NoMemory();

    memset(obj, 0, size);

    if (type->tp_flags & COType_FLAG_HEAP)
        CO_INCREF(type);

    COVarObject_INIT(obj, type, 0);

    return obj;
}

static void
object_dealloc(COObject *this)
{
    COTypeObject *type;

    /* Extract the type; we expect it to be a heap type */
    type = CO_TYPE(this);
    assert(type->tp_flags & COType_FLAG_HEAP);

    CO_DECREF(type);
    COObject_Mem_FREE(this);
}

static COObject *
object_repr(COObject *this)
{
    return COStr_FromString("<class>");
}

static int
object_init(COObject *this, COObject *args)
{
    return 0;
}

// The most base type object.
COTypeObject COObject_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "object",
    sizeof(COObject),
    0,
    0,
    (newfunc)object_new,         /* tp_alloc                  */
    (initfunc)object_init,       /* tp_init                 */
    (deallocfunc)object_dealloc, /* tp_dealloc              */
    (reprfunc)object_repr,       /* tp_repr                 */
    0,                           /* tp_print                */
    0,                           /* tp_hash                 */
    0,                           /* tp_compare              */
    0,                           /* tp_traverse             */
    0,                           /* tp_clear                */
    0,                           /* tp_call                 */
    0,                           /* tp_iter                 */
    0,                           /* tp_iternext             */
    0,                           /* tp_arithmetic_interface */
    0,                           /* tp_mapping_interface    */
    0,                           /* tp_sequence_interface   */
    0,                           /* tp_dict                 */
    0,                           /* tp_dictoffset           */
    0,                           /* tp_base                 */
    0,                           /* tp_methods              */
    0,                           /* tp_members              */
};

static int
_add_methods(COTypeObject *type, COMethodDef *methods)
{
    COObject *dict = type->tp_dict;
    for (; methods->m_name != NULL; methods++) {
        COObject *func = COCFunction_New(methods->m_name, methods->m_func);
        CODict_SetItemString(dict, methods->m_name, func);
        CO_DECREF(func);
    }

    return 0;
}

int
COType_Ready(COObject *_this)
{
    COTypeObject *this = (COTypeObject *)_this;
    if (COType_HasFeature(this, COType_FLAG_READY)) {
        return 0;
    }

    // Initialize tp_base (defaults to COObject_Type).
    COTypeObject *base = this->tp_base;
    if (base == NULL && this != &COObject_Type) {
        base = this->tp_base = &COObject_Type;
        CO_INCREF(base);
    }

    // Initialize the base class.
    if (base) {
        // COObject_Type don't has base type
        if (COType_Ready((COObject *)base) < 0) {
            goto error;
        }
    }

    // Initialize from base
#define COPYVAL(SLOT) \
        if (this->SLOT == 0) this->SLOT = base->SLOT
    if (base) {
        COPYVAL(tp_alloc);
        COPYVAL(tp_init);
        COPYVAL(tp_basicsize);
        COPYVAL(tp_itemsize);
        COPYVAL(tp_dictoffset);
        COPYVAL(tp_repr);
        COPYVAL(tp_print);
        COPYVAL(tp_dealloc);
    }

    // Initialize tp_dict.
    COObject *dict = this->tp_dict;
    if (!dict) {
        dict = CODict_New();
        this->tp_dict = dict;
    }

    // Attributes.
    if (this->tp_methods)
        _add_methods(this, this->tp_methods);

    // All done.
    this->tp_flags |= COType_FLAG_READY;
    return 0;

error:
    return -1;
}
