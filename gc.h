#ifndef GC_H
#define GC_H
/**
 * Garbage Collection
 *
 * (Based on manualgc (http://code.google.com/p/manualgc/) and Python gc
 * module.)
 *
 * Currently, gc uses reference counting to keep track of allocated objects on
 * heap. Each object has a reference count which indicates how many objects are
 * referring to it. When this reference count reaches zero the object is freed.
 *
 * TODO Add mechanism to detect reference cycle.
 *
 * Author: Yecheng Fu <cofyc.jackson@gmail.com> 
 */



#endif
