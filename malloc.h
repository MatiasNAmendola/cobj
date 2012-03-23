#ifndef MALLOC_H
#define MALLOC_H
/**
 * Object Allocator for CObject.
 */

void *co_malloc(size_t size);
void *co_realloc(void *ptr, size_t size);
#define co_free free

#endif
