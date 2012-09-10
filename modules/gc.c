/**
 * Reference Cycle Garbage Collection Module
 */

#include "../cobj.h"

static COObject *
builtin_gc_collect(COObject *this, COObject *args)
{
    ssize_t n = COObject_GC_Collect();
    return COInt_FromLong(n);
}

COCFunctionObject _CO_Builtin_gc_collect = {
    COObject_HEAD_INIT(&COCFunction_Type),
    "gc_collect",
    builtin_gc_collect,
};

COObject *
module_gc_init(void)
{
    COObject *gc = CODict_New();
    CODict_SetItemString(gc, "collect", (COObject *)&_CO_Builtin_gc_collect);

    return gc;
}
