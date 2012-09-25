#include "cobj.h"

COObject *
COObject_Repr(COObject *o)
{
    if (!o)
        return COStr_FromString("<NULL>");
    return CO_TYPE(o)->tp_repr(o);
}

void
COObject_Dump(COObject *o)
{
    if (o == NULL) {
        fprintf(stderr, "NULL\n");
    } else {
        fprintf(stderr, "object(%p):\n", (void *)o);
        fprintf(stderr, "    type: %s\n",
                CO_TYPE(o) == NULL ? "NULL" : CO_TYPE(o)->tp_name);
        fprintf(stderr, "    refcnt: %d\n", o->co_refcnt);
        COStrObject *s = (COStrObject *)COObject_Repr(o);
        fprintf(stderr, "    repr: %s\n", s->co_sval);
        CO_DECREF(s);
    }
}

long
_CO_HashPointer(void *p)
{
    long x;
    size_t y = (size_t) p;
    /* bottom 3 or 4 bits are likely to be 0; rotate y by 4 to avoid
       excessive hash collisions for dicts and sets */
    y = (y >> 4) | (y << (8 * sizeof(void *) - 4));
    x = (long)y;
    if (x == -1)
        x = -2;
    return x;
}

long
COObject_Hash(COObject *o)
{
    COTypeObject *tp = o->co_type;

    if (tp->tp_hash != NULL) {
        return tp->tp_hash(o);
    }

    if (tp->tp_compare == NULL) {
        /* imply it's unique in world, so use this address as hash value */
        return _CO_HashPointer(o);
    }

    COErr_Format(COException_TypeError, "unhashable type: '%.200s'",
                 tp->tp_name);
    return -1;
}

COObject *
COObject_New(COTypeObject *tp)
{
    COObject *o;
    o = (COObject *)COObject_Mem_MALLOC(tp->tp_basicsize);
    if (o == NULL) {
        return COErr_NoMemory();
    }

    return COObject_INIT(o, tp);
}

COObject *
COVarObject_New(COTypeObject *tp, ssize_t n)
{
    COVarObject *o;
    const size_t size = COVarObject_SIZE(tp, n);
    o = (COVarObject *)COObject_Mem_MALLOC(size);
    if (o == NULL) {
        return COErr_NoMemory();
    }
    return (COObject *)COVarObject_INIT(o, tp, n);
}

int
COObject_Print(COObject *o, FILE *fp)
{
    if (!o) {
        fprintf(fp, "<null>");
        return 0;
    }
    if (CO_REFCNT(o) <= 0) {
        fprintf(fp, "<refcnt %ld at %p>", (long)CO_REFCNT(o), (void *)o);
        return 0;
    } else if (CO_TYPE(o)->tp_print == NULL) {
        COObject *s = CO_TYPE(o)->tp_repr(o);
        if (!s)
            return -1;
        int ret = 0;
        ret = COObject_Print(s, fp);
        CO_DECREF(s);
        return ret;
    } else {
        return CO_TYPE(o)->tp_print(o, fp);
    }
}

/*
 * COObject Compare
 *
 * Every object's type have a tp_compare function slot, it's not NULL, it
 * gets called with two objects and Cmp_Op, and should return an object as follows:
 *  - NULL if an exception occurred
 *  - False object is comparison is false
 *  - True object is comparison is true
 */
static COObject *
do_compare(COObject *v, COObject *w, int op)
{
    static char *opstrings[] = { "<", "<=", "==", "!=", ">", ">=" };

    comparefunc f;
    COObject *x = NULL;

    if (v->co_type == w->co_type && (f = v->co_type->tp_compare) != NULL) {
        x = (*f) (v, w, op);
        return x;
    }

    /* Default comparision. */
    switch (op) {
    case Cmp_EQ:
        x = (v == w) ? CO_True : CO_False;
        CO_INCREF(x);
        break;
    case Cmp_NE:
        x = (v != w) ? CO_True : CO_False;
        CO_INCREF(x);
        break;
    default:
        COErr_Format(COException_NotImplementedError,
                     "undefined comparison: %.100s() %s %.100s()",
                     v->co_type->tp_name, opstrings[op], w->co_type->tp_name);
    }

    return x;
}

COObject *
COObject_Compare(COObject *a, COObject *b, int op)
{
    assert(Cmp_LT <= op && op <= Cmp_GE);
    COObject *x;

    if (!a || !b) {
        COErr_BadInternalCall();
        return NULL;
    }

    x = do_compare(a, b, op);
    return x;
}

/*
 * -1 for error, 0 for falce, 1 for true
 */
int
COObject_CompareBool(COObject *a, COObject *b, int op)
{
    COObject *x;
    int ok;
    /* Quick result when objects are the same.
       Guarantees that identity implies equality. */
    if (a == b) {
        if (op == Cmp_EQ)
            return 1;
        else if (op == Cmp_NE)
            return 0;
    }
    x = COObject_Compare(a, b, op);
    if (!x)
        return -1;
    if (x == CO_True)
        ok = 1;
    else
        ok = 0;
    CO_DECREF(x);
    return ok;
}

/*
 * Return -1 if an error occured. 0 for false, 1 for true.
 */
int
COObject_IsTrue(COObject *o)
{
    ssize_t size;
    if (o == CO_True)
        return 1;
    if (o == CO_False)
        return 0;
    if (o == CO_None)
        return 0;
    if (COInt_Check(o)) {
        size = CO_SIZE(o);
    } else {
        // default
        return 1;
    }
    return (size == 0) ? 0 : 1;
}

void
_CO_NegativeRefCnt(const char *fname, int lineno, COObject *co)
{
    char buf[256];
    snprintf(buf, sizeof(buf),
             "%s:%i object at %p has negative ref count: %d", fname, lineno,
             (void *)co, co->co_refcnt);
    fprintf(stderr, "%s\n", buf);
    exit(-1);
}

int
COObject_ParseArgs(COObject *args, ...)
{
    va_list va;

    if (args == NULL || !COTuple_Check(args)) {
        COErr_BadInternalCall();
        return 0;
    }

    va_start(va, args);

    int i;
    COObject **o;

    i = 0;
    while (true) {
        o = va_arg(va, COObject **);
        if (!o)
            break;
        if (i >= COTuple_GET_SIZE(args)) {
            *o = NULL;
            break;
        } else {
            *o = COTuple_GET_ITEM(args, i);
        }
        i++;
    }

    va_end(va);

    return 1;
}

COObject *
COObject_Str(COObject *o)
{
    if (!o)
        return COStr_FromString("<NULL>");

    if (COStr_Check(o)) {
        CO_INCREF(o);
        return o;
    }

    return COObject_Repr(o);
}

COObject *
COObject_Call(COObject *this, COObject *args)
{
    binaryfunc call;
    if ((call = this->co_type->tp_call) != NULL) {
        COObject *result;
        result = call(this, args);
        if (!result && !COErr_Occurred())
            COErr_SetString(COException_SystemError,
                            "NULL result without error in COObject_Call");
        return result;
    }
    COErr_Format(COException_TypeError, "'%.200s' object is not callable",
                 this->co_type->tp_name);
    return NULL;
}

COObject *
COObject_GetIter(COObject *o)
{
    COTypeObject *tp = o->co_type;
    getiterfunc f = NULL;
    f = tp->tp_iter;
    if (f == NULL) {
        COErr_BadInternalCall();
        return NULL;
    } else {
        COObject *res = (*f) (o);
        return res;
    }
}

COObject *
COObject_GetSelf(COObject *o)
{
    CO_INCREF(o);
    return o;
}

COObject *
COObject_GetAttrString(COObject *o, const char *name)
{
    COObject *attr_name = COStr_FromString(name);
    COObject *v = COObject_GetAttr(o, attr_name);
    CO_DECREF(attr_name);
    return v;
}

/* Helper to get a pointer to an object's __dict__ slot, if any. */

COObject **
_COObject_GetDictPtr(COObject *o)
{
    ssize_t dictoffset;
    COTypeObject *tp = CO_TYPE(o);

    dictoffset = tp->tp_dictoffset;
    if (dictoffset == 0)
        return NULL;

    return (COObject **)((char *)o + dictoffset);
}

COObject *
COObject_GetAttr(COObject *o, COObject *attr)
{
    COObject *res = NULL;
    COTypeObject *tp = CO_TYPE(o);
    COObject **dictptr;
    COObject *dict;

    if (!COStr_Check(attr)) {
        COErr_Format(COException_TypeError,
                     "attribute name must be string, not '%.200s'",
                     CO_TYPE(attr)->tp_name);
        return NULL;
    }
    // for static allocated Type objects, such as COInt_Type, etc
    if (!tp->tp_dict) {
        if (COType_Ready((COObject *)tp) < 0)
            return NULL;
    }

    dictptr = _COObject_GetDictPtr(o);
    if (dictptr) {
        dict = *dictptr;
        if (dict) {
            res = CODict_GetItem(dict, attr);
            if (res) {
                CO_INCREF(res);
                return res;
            }
        }
    }

    if (tp->tp_dict) {
        res = CODict_GetItem(tp->tp_dict, attr);
        if (res) {
            CO_INCREF(res);
            return res;
        }
    }

    if (COType_Check(o)) {
        COErr_Format(COException_AttributeError,
                     "'%.200s' object '%.200s' has no attribute '%s'",
                     CO_TYPE(o)->tp_name, ((COTypeObject *)o)->tp_name,
                     COStr_AS_STRING(attr));
    } else {
        COErr_Format(COException_AttributeError,
                     "'%.200s' object has no attribute '%s'",
                     CO_TYPE(o)->tp_name, COStr_AS_STRING(attr));
    }

    return NULL;
}

int
COObject_SetAttr(COObject *o, COObject *attr, COObject *v)
{
    int err;
    COObject **dictptr;
    COObject *dict;
    dictptr = _COObject_GetDictPtr(o);
    if (dictptr) {
        dict = *dictptr;
        if (!dict) {
            dict = CODict_New();
            *dictptr = dict;
            if (!dict)
                return -1;
        }
        if (v == NULL) {
            err = CODict_DelItem(dict, attr);
        } else {
            err = CODict_SetItem(dict, attr, v);
        }
        return err;
    }

    COErr_Format(COException_AttributeError,
                 "'%.200s' object has no attribute '%s'", CO_TYPE(o)->tp_name,
                 COStr_AS_STRING(attr));

    return -1;
}

/* Sequence Methods. */

/*
 * Get length of given object.
 */
ssize_t
COSequence_Length(COObject *seq)
{
    COSequenceInterface *si = CO_TYPE(seq)->tp_sequence_interface;
    if (si && si->sq_length)
        return si->sq_length(seq);

    COErr_Format(COException_TypeError,
                 "'%.200s' object does not implement __len__",
                 CO_TYPE(seq)->tp_name);
    return 0;
}

/*
 * Returns 1 or 0 on success, -1 on failure.
 */
int
COSequence_Contains(COObject *seq, COObject *o)
{
    COSequenceInterface *si = CO_TYPE(seq)->tp_sequence_interface;
    if (si && si->sq_contains)
        return si->sq_contains(seq, o);

    COErr_Format(COException_TypeError,
                 "'%.200s' object does not support `in` operator",
                 CO_TYPE(seq)->tp_name);
    return -1;
}

/* ! Sequence Methods. */

/* Mapping Methods. */

COObject *
COMapping_GetItem(COObject *o, COObject *key)
{
    COMappingInterface *mi;
    mi = o->co_type->tp_mapping_interface;
    if (mi && mi->mp_subscript)
        return mi->mp_subscript(o, key);

    COErr_Format(COException_TypeError,
                 "'%.200s' object is not subscriptable", CO_TYPE(o)->tp_name);
    return NULL;
}

int
COMapping_SetItem(COObject *o, COObject *key, COObject *value)
{
    COMappingInterface *mi;
    mi = o->co_type->tp_mapping_interface;
    if (mi && mi->mp_ass_subscript)
        return mi->mp_ass_subscript(o, key, value);

    COErr_Format(COException_TypeError,
                 "'%.200s' object does not support subscriptable assignment",
                 CO_TYPE(o)->tp_name);
    return -1;
}

/* ! Mapping Methods. */

/* Arithmetic methods. */

/*
 * Calling scheme used for binary operations:
 * 
 * Order operations are tried until either a valid result or error:
 *
 *   b.op(a, b) - only when a->co_type != b->co_type && b->co_type is subclass
 *   of a->co_type
 *   v.op(a, b)
 *   w.op(a, b)
 */
#define ARITHMETIC_BINARY_FUNC(func, op, op_name)  \
    COObject * \
    func(COObject *v, COObject *w) \
    { \
        COObject *x; \
        binaryfunc slotv; \
        binaryfunc slotw; \
\
        if (v->co_type->tp_arithmetic_interface != NULL) \
            slotv = v->co_type->tp_arithmetic_interface->op; \
        if (w->co_type != v->co_type && \
            w->co_type->tp_arithmetic_interface != NULL) { \
            slotw = w->co_type->tp_arithmetic_interface->op; \
            if (slotw == slotv) \
                slotw = NULL; \
        } \
\
        if (slotv) { \
            x = slotv(v, w); \
            if (x) \
                return x; \
        } \
        if (slotw) { \
            x = slotw(v, w); \
            if (x) \
                return x; \
        } \
\
        COErr_Format(COException_NotImplementedError, \
                     "undefined arithmetic operation: %.100s() %s %.100s()", \
                     v->co_type->tp_name, op_name, w->co_type->tp_name); \
        return NULL; \
    }

ARITHMETIC_BINARY_FUNC(COArithmetic_Add, arith_add, "+")
ARITHMETIC_BINARY_FUNC(COArithmetic_Sub, arith_sub, "-")
ARITHMETIC_BINARY_FUNC(COArithmetic_Mul, arith_mul, "*")
ARITHMETIC_BINARY_FUNC(COArithmetic_Div, arith_div, "/")
ARITHMETIC_BINARY_FUNC(COArithmetic_Mod, arith_mod, "%")
ARITHMETIC_BINARY_FUNC(COArithmetic_Pow, arith_pow, "**")
ARITHMETIC_BINARY_FUNC(COArithmetic_Lshift, arith_lshift, "<<")
ARITHMETIC_BINARY_FUNC(COArithmetic_Rshift, arith_rshift, ">>")

COObject *
COArithmetic_Neg(COObject *o)
{
    COAritmeticInterface *ai;
    ai = o->co_type->tp_arithmetic_interface;
    if (ai && ai->arith_neg)
        return ai->arith_neg(o);

    COErr_Format(COException_TypeError,
                 "bad operand type for unary -: '%.200s'", CO_TYPE(o)->tp_name);
    return NULL;
}

COObject *
COArithmetic_Invert(COObject *o)
{
    COAritmeticInterface *ai;
    ai = o->co_type->tp_arithmetic_interface;
    if (ai && ai->arith_invert)
        return ai->arith_invert(o);

    COErr_Format(COException_TypeError,
                 "bad operand type for unary ~: '%.200s'", CO_TYPE(o)->tp_name);
    return NULL;
}

/* ! Arithmetic methods. */
