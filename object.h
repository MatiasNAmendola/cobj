#ifndef OBJECT_H
#define OBJECT_H
/*
 * Object interface (Object, Type, Value)
 *
 * Objects are structures allocated on the heap. Objects are never allocated
 * statically or on the stack (program initialized objects are exceptions);
 * they must only be accessed through special macros and functions only to
 * ensure they are properly garbage-collected.
 *
 * An object has a 'reference count' that is increased or decreased when a
 * pointer to the object is copied or deleted; when the reference count reaches
 * zero there are no references to the object left and it can be removed from
 * the heap.
 *
 * An object has a 'type' that determines what it represents and what kind of
 * data it contains. An object's type is fixed when it's created. Types
 * themselves are represented as objects, and type's 'type' is type object.
 *
 * Objects do not float around in memory; once allocated an object keeps the
 * same size and address. Objects that must hold variable-size data can contain
 * pointers to the variable-size parts of the object. Not all objects of the
 * same type have the same size; but the size cannot change after allocation.
 * (These restrictions are made so a reference to an object can be simply a
 * pointer -- moving an object would required updating all pointers, and
 * changing an object's size would require moving it if there was another object
 * right next to it.)
 *
 * Objects are always accessed through pointers of the type 'struct COObject *'.
 * It's a structure that only contains the reference count and the type pointer.
 * The actual memory allocated for an object contains other data that can only
 * be accessed after casting the pointer to a pointer to longer structure type.
 * This longer type must start with the reference count and type fields; the
 * macro COObject_HEAD should be used for this (to accommodate for future
 * changes). The implementation of a particular object type can cast the object
 * pointer to the proper type and back.
 */

#include "co.h"

// initial segment of every object
#define COObject_HEAD struct COObject ob_base
#define COObject_HEAD_INIT(type)    \
    { 0, 0, 1, type }


struct COObject {
    /* 
     * `ob_next` and `ob_prev` is for a doubly-linked list of all live heap
     * objects.
     */
    struct COObject *_ob_next;
    struct COObject *_ob_prev;
    unsigned int co_refcnt;
    struct COTypeObject *ob_type;
};

struct COVarObject {
    COObject_HEAD;
    size_t ob_size;         /* number of objects */
};

typedef struct COObject *(*reprfunc)(struct COObject *);
typedef struct COObject *(*getattrfunc)(struct COObject *, char *);
typedef int (*setattrfunc)(struct COObject *, char *, struct COObject *);

struct COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;

    /* Methods */
    reprfunc tp_print;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
};

#endif
