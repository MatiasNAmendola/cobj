#include "cobj.h"

void *
_COObject_Mem_DebugMalloc(size_t n)
{
    // TODO DEBUG
    return ((size_t) (n) > SIZE_MAX ? NULL : malloc((n) ? (n) : 1));
}

void *
_COObject_Mem_DebugRelloc(void *p, size_t n)
{
    // TODO DEBUG
    return ((size_t) (n) > SIZE_MAX ? NULL : realloc((p), (n) ? (n) : 1));
}

void
_COObject_Mem_DebugFree(void *p)
{
    // TODO DEBUG
    free(p);
}
