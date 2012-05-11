#ifndef MEM_H
#define MEM_H
/**
 * COObject Memory Allocator.
 */

#ifdef COMEM_DEBUG
# define COObject_Mem_MALLOC   _COObject_Mem_DebugMalloc
# define COObject_Mem_REALLOC  _COObject_Mem_DebugRelloc
# define COObject_Mem_FREE     _COObject_Mem_DebugFree
#else
/* NULL indicates error, and some systems would return NULL for malloc(0), so we
 * return one extra byte when size is 0.
 */
# define COObject_Mem_MALLOC(n)        malloc((n))
# define COObject_Mem_REALLOC(p, n)    realloc((p), (n))
# define COObject_Mem_FREE             free
#endif

void *_COObject_Mem_DebugMalloc(size_t n);
void *_COObject_Mem_DebugRelloc(void *p, size_t n);
void _COObject_Mem_DebugFree(void *p);

#endif
