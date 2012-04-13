#include "co.h"

COObject *
COObject_repr(COObject *o)
{
    return CO_TYPE(o)->tp_repr(o);
}

void
COObject_dump(COObject *o)
{
    if (o == NULL) {
        fprintf(stderr, "NULL\n");
    } else {
        fprintf(stderr, "object:\n");
        fprintf(stderr, "    type: %s\n",
                CO_TYPE(o) == NULL ? "NULL" : CO_TYPE(o)->tp_name);
        COStrObject *s = (COStrObject *)COObject_repr(o);
        fprintf(stderr, "    repr: %s\n", s->co_sval);
        fprintf(stderr, "    refcnt: %d\n", o->co_refcnt);
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
_COObject_New(COTypeObject *tp)
{
    COObject *o;
    o = (COObject *)COMem_MALLOC(tp->tp_basicsize);
    if (o == NULL) {
        COErr_NoMemory();
        return NULL;
    }

    return COObject_Init(o, tp);
}

COObject *
_COVarObject_New(COTypeObject *tp, ssize_t n)
{
    COVarObject *o;
    const size_t size = COObject_VAR_SIZE(tp, n);
    o = (COVarObject *)COMem_MALLOC(size);
    if (o == NULL) {
        COErr_NoMemory();
        return NULL;
    }
    return (COObject *)COVarObject_Init(o, tp, n);
}

void
COObject_print(COObject *o)
{
    COStrObject *s = (COStrObject *)CO_TYPE(o)->tp_repr(o);
    printf("%s\n", s->co_sval);
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
do_compare(COObject *a, COObject *b, int op)
{
    static char *opstrings[] = { "<", "<=", "==", "!=", ">", ">=" };

    comparefunc f;
    COObject *x;

    if (a->co_type == b->co_type && (f = a->co_type->tp_compare) != NULL) {
        x = (*f) (a, b, op);
    } else {
        COErr_Format(COException_UndefinedError,
                     "undefined comparison: %.100s() %s %.100s()",
                     a->co_type->tp_name, opstrings[op], b->co_type->tp_name);
        return NULL;
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
