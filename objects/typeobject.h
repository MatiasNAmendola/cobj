#ifndef OBJECTS_TYPEOBJECT_H
#define OBJECTS_TYPEOBJECT_H
/**
 * Type object.
 */

#include "../object.h"

/* Object Methods */
typedef COObject *(*newfunc)(COObject *, COObject *);
typedef int (*initfunc)(COObject *, COObject *, COObject *);
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
    lenfunc sq_length;
    binaryintfunc sq_contains;
} COSequenceInterface;

typedef struct {
    binaryfunc mp_subscript;
    ternaryintfunc mp_ass_subscript;
} COMappingInterface;
/* ! Object Interfaces */

/* Structures used to define methods and members of object in C. */

typedef struct _COMethodDef {
    const char *m_name;
    COCFunction m_func;         /* The C Function that implements it. */
    int m_flags;
} COMethodDef;

typedef struct _COMemberDef {
    const char *m_name;
    int m_type;
    int m_offset;
    int m_flags;
} COMemberDef;

struct _COTypeObject {
    COObject_HEAD;
    char *tp_name;
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;
    int tp_flags;               /* Flags to define optional/expanded features */

    /* Basic methods.  */
    newfunc tp_new;             /* Create object. */
    initfunc tp_init;           /* Initiate object. */
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

    /* Standard interfaces. */
    COAritmeticInterface *tp_arithmetic_interface;
    COMappingInterface *tp_mapping_interface;
    COSequenceInterface *tp_sequence_interface;

    /* Object-Oriented Stuff. */
    COObject *tp_dict;
    ssize_t tp_dictoffset;
    COMethodDef *tp_methods;
    COMemberDef *tp_members;
};

COTypeObject COType_Type;

/* The *real* layout of a type object when allocated on the heap.
 *
 * Why:
 *  When allocated on heap, we need to manage referencens between type object
 *  with its slots objects, like tp_name string from string object, etc.
 */
typedef struct _COHeapTypeObject {
    COTypeObject ht_type;
    COObject *ht_name;
} COHeapTypeObject;

#define COType_Check(co) (CO_TYPE(co) == &COType_Type)

#define COType_FLAG_DEFAULT (1L<<0)
#define COType_FLAG_GC      (1L<<1)
#define COType_FLAG_READY   (1L<<2)
#define COType_FLAG_HEAP    (1L<<3)

#define COType_HasFeature(t, f) ((((COTypeObject *)t)->tp_flags & (f)) != 0)

int COType_Ready(COObject *);
void default_dealloc(COObject *this);

#endif
