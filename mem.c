#include "co.h"

void *
_COMem_DebugMalloc(size_t n)
{
    // TODO DEBUG
    return ((size_t) (n) > SIZE_MAX ? NULL : malloc((n) ? (n) : 1));
}

void *
_COMem_DebugRelloc(void *p, size_t n)
{
    // TODO DEBUG
    return ((size_t) (n) > SIZE_MAX ? NULL : realloc((p), (n) ? (n) : 1));
}

void
_COMem_DebugFree(void *p)
{
    // TODO DEBUG
    free(p);
}
