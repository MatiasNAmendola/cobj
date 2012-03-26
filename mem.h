#ifndef MEM_H
#define MEM_H
/**
 * COObject Memory Allocator.
 */

#ifdef COMEM_DEBUG
# define COMem_MALLOC   _COMem_DebugMalloc
# define COMem_REALLOC  _COMem_DebugRelloc
# define COMem_FREE     _COMem_DebugFree
#else
/* NULL indicates error, and some systems would return NULL for malloc(0), so we
 * return one extra byte when size is 0.
 */
# define COMem_MALLOC(n)   ((size_t)(n) > SIZE_MAX ? NULL : malloc((n) ? (n) : 1))
# define COMem_REALLOC(p, n)  ((size_t)(n) > SIZE_MAX ? NULL : realloc((p), (n) ? (n) : 1))
# define COMem_FREE     free
#endif

void *_COMem_DebugMalloc(size_t n);
void *_COMem_DebugRelloc(void *p, size_t n);
void _COMem_DebugFree(void *p);

#endif
