#ifndef ARENA_H
#define ARENA_H
/**
 * A arena-like memory allocator.
 *
 * `arena_new` creates a new arena, `arena_free` frees it.
 * Once an arena has been created, it can be used to allocated memory via
 * `arena_malloc`.
 * When arena is freed, all the memory it allocated will be freed.
 *
 * `arena_malloc`, like malloc(), return the address of a block of memory
 * spanning `size` bytes, or return NULL if new memory can't be obtained.
 * Note that pointers obtained via `arena_malloc` must never be passed to the
 * system free() or realloc(), or to any of similar memory management functions.
 *
 * Objects can be registerted with the arena via `arena_addobject`, and the
 * arena will ensure that they stay alive at least until the arena is freed.
 */

#include "object.h"

struct arena;

struct arena *arena_new(void);
void *arena_malloc(struct arena *, size_t size);
void arena_free(struct arena *);
int arena_addobject(struct arena *, COObject *);

#endif
