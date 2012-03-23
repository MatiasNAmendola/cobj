#include "co.h"

void *
co_malloc(size_t size)
{
    void *ret = malloc(size);

    if (!ret) {
        error("Out of memory, malloc failed");
    }

    return ret;
}

void *
co_realloc(void *ptr, size_t size)
{
    void *ret = realloc(ptr, size);

    if (!ret) {
        error("Out of memory, realloc failed");
    }

    return ret;
}
