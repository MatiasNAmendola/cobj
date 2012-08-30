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
/* ! Object Methods */

/* Object Interfaces */
typedef struct {
    binaryfunc int_add;
    binaryfunc int_sub;
    binaryfunc int_mul;
    binaryfunc int_div;
    binaryfunc int_mod;
    binaryfunc int_lshift;
    binaryfunc int_rshift;
    unaryfunc int_neg;
    unaryfunc int_invert;
} COIntInterface;

typedef struct {
    lenfunc sq_length;
    binaryfunc sq_concat;
} COSequenceInterface;

typedef struct {
    lenfunc mp_length;
    binaryfunc mp_subscript;
} COMappingInterface;
/* ! Object Interfaces */

struct _COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;
    int tp_flags;               /* Flags to define optional/expanded features */

    /* Basic Methods  */
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

    /* Interfaces for standard types */
    COIntInterface *tp_int_interface;           /* Implement int object interface. */
    COMappingInterface *tp_mapping_interface;   /* Implement map object interface. (dict) */
};

COTypeObject COType_Type;

#define COType_Check(co) (CO_TYPE(co) == &COType_Type)

#define COType_FLAG_GC  (1<<0)
#define COType_HasFeature(t, f) (((t)->tp_flags & (f)) != 0)

void default_dealloc(COObject *this);

#endif
