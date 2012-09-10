#ifndef OBJECTS_TYPEOBJECT_H
#define OBJECTS_TYPEOBJECT_H
/**
 * Type object.
 */

#include "../object.h"

/* Object Methods */
typedef COObject *(*newfunc)(COTypeObject *, COObject *);
typedef void (*deallocfunc) (COObject *);
typedef COObject *(*reprfunc)(COObject *);
typedef int (*printfunc) (COObject *, FILE *);
typedef long (*hashfunc) (COObject *);
typedef COObject *(*comparefunc)(COObject *, COObject *, int);
typedef int (*visitfunc) (COObject *, void *);
typedef int (*traversefunc) (COObject *, visitfunc, void *);
typedef int (*inquiryfunc) (COObject *);
typedef COObject *(*unaryfunc)(COObject *);
typedef COObject *(*binaryfunc)(COObject *, COObject *);
typedef COObject *(*ternaryfunc)(COObject *, COObject *, COObject *);
typedef ssize_t(*lenfunc) (COObject *);
typedef COObject *(*getiterfunc)(COObject *);
typedef COObject *(*iternextfunc)(COObject *);
typedef int (*binaryintfunc) (COObject *, COObject *);
typedef int (*ternaryintfunc) (COObject *, COObject *, COObject *);
/* ! Object Methods */

/* Object Interfaces */
typedef struct {
    binaryfunc arith_add;
    binaryfunc arith_sub;
    binaryfunc arith_mul;
    binaryfunc arith_div;
    binaryfunc arith_mod;
    binaryfunc arith_lshift;
    binaryfunc arith_rshift;
    unaryfunc arith_neg;
    unaryfunc arith_invert;
} COAritmeticInterface;

typedef struct {
    binaryintfunc sq_contains;
} COSequenceInterface;

typedef struct {
    binaryfunc mp_subscript;
    ternaryintfunc mp_ass_subscript;
} COMappingInterface;
/* ! Object Interfaces */

struct _COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;
    int tp_flags;               /* Flags to define optional/expanded features */

    /* Basic methods.  */
    newfunc tp_new;             /* Create object. */
    deallocfunc tp_dealloc;     /* Destory object. */
    reprfunc tp_repr;           /* Represent object. */
    printfunc tp_print;         /* Print object onto stdout, or any file. */
    hashfunc tp_hash;           /* Hash object. */
    comparefunc tp_compare;     /* Compare object. */
    traversefunc tp_traverse;   /* Traverse object. */
    inquiryfunc tp_clear;       /* Clear object. */
    binaryfunc tp_call;         /* Call object. */
    getiterfunc tp_iter;        /* Get iterator of object. */
    iternextfunc tp_iternext;   /* Iter to next object. */
    lenfunc tp_len;             /* Get length of object. */

    /* Standard interfaces. */
    COAritmeticInterface *tp_arithmetic_interface;
    COMappingInterface *tp_mapping_interface;
    COSequenceInterface *tp_sequence_interface;
};

COTypeObject COType_Type;

#define COType_Check(co) (CO_TYPE(co) == &COType_Type)

#define COType_FLAG_GC  (1<<0)
#define COType_HasFeature(t, f) (((t)->tp_flags & (f)) != 0)

void default_dealloc(COObject *this);

#endif
