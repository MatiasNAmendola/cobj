#include "../cobj.h"

static long
float_hash(COFloatObject *this)
{
    // TODO
    return 1;
}

/*
 * Macro and helper that convert COObject obj to a C double and store the value
 * in dbl. If convension to double raises an exception, obj is set to NULL, and
 * the function invoking this macro returns NULL. If obj is not of float or int
 * type, COException_NotImplementedError raised.
 */
#define CONVERT_TO_DOUBLE(obj, dbl)                 \
    if (COFloat_Check(obj))                         \
        dbl = COFloat_AS_DOUBLE(obj);               \
    else if (convert_to_double(&(obj), &(dbl)) < 0) \
        return obj

static int
convert_to_double(COObject **v, double *dbl)
{
    COObject *obj = *v;

    if (COInt_Check(obj)) {
        *dbl = COInt_AsDouble(obj);
        if (COErr_Occurred()) {
            *v = NULL;
            return -1;
        }
    } else {
        COErr_SetNone(COException_NotImplementedError);
        *v = NULL;
        return -1;
    }
    return 0;
}

static COObject *
float_repr(COFloatObject *this)
{
    char buffer[32];
    int n = snprintf(buffer, 32, "%g", this->co_fval);
    return COStr_FromStringN(buffer, n);
}

static void
float_dealloc(COFloatObject *this)
{
    COObject_Mem_FREE(this);
}

static COObject *
float_compare(COObject *v, COObject *w, int op)
{
    double i, j;
    int r = 0;

    if (COFloat_Check(v) && COFloat_Check(w)) {
        i = COFloat_AS_DOUBLE(v);
        j = COFloat_AS_DOUBLE(w);
        goto compare;
    } else {
        goto notimplemented;
    }

compare:
    switch (op) {
    case Cmp_EQ:
        r = i == j;
        break;
    case Cmp_NE:
        r = i != j;
        break;
    case Cmp_LE:
        r = i <= j;
        break;
    case Cmp_GE:
        r = i >= j;
        break;
    case Cmp_LT:
        r = i < j;
        break;
    case Cmp_GT:
        r = i > j;
        break;
    }
    return COBool_FromLong(r);

notimplemented:
    COErr_SetNone(COException_NotImplementedError);
    return NULL;
}

static COObject *
float_add(COObject *v, COObject *w)
{
    double a, b;
    CONVERT_TO_DOUBLE(v, a);
    CONVERT_TO_DOUBLE(w, b);
    a = a + b;
    return COFloat_FromDouble(a);
}

static COObject *
float_pow(COObject *v, COObject *w)
{
    // TODO
    double a, b;
}

static COAritmeticInterface arithmetic_interface = {
    (binaryfunc)float_add,
    /*(binaryfunc)float_sub,*/
    /*(binaryfunc)float_mul,*/
    /*(binaryfunc)float_div,*/
    /*(binaryfunc)float_mod,*/
    0,
    0,
    0,
    0,
    (binaryfunc)float_pow,
    0,
    0,
    0,
    0,
    /*(binaryfunc)float_lshift,*/
    /*(binaryfunc)float_rshift,*/
    /*(unaryfunc)float_neg,*/
    /*(unaryfunc)float_invert,*/
};

COTypeObject COFloat_Type = {
    COVarObject_HEAD_INIT(&COType_Type, 0),
    "float",
    sizeof(COFloatObject),
    0,
    0,
    0,                          /* tp_alloc                */
    0,                          /* tp_init                 */
    (deallocfunc)float_dealloc, /* tp_dealloc              */
    (reprfunc)float_repr,       /* tp_repr                 */
    0,                          /* tp_print                */
    (hashfunc)float_hash,       /* tp_hash                 */
    (comparefunc)float_compare, /* tp_compare              */
    0,                          /* tp_traverse             */
    0,                          /* tp_clear                */
    0,                          /* tp_call                 */
    0,                          /* tp_iter                 */
    0,                          /* tp_iternext             */
    &arithmetic_interface,      /* tp_arithmetic_interface */
    0,                          /* tp_mapping_interface    */
    0,                          /* tp_sequence_interface   */
};

COObject *
COFloat_FromDouble(double dbl)
{
    COFloatObject *f;

    f = COObject_NEW(COFloatObject, &COFloat_Type);
    f->co_fval = dbl;
    return (COObject *)f;
}

COObject *
COFloat_FromString(char *s)
{
    COFloatObject *f;

    f = COObject_NEW(COFloatObject, &COFloat_Type);
    f->co_fval = strtof(s, NULL);
    return (COObject *)f;
}
