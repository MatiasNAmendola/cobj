#ifndef MALLOC_H
#define MALLOC_H
/**
 * COObject Memory Allocation Interfaces.
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

#endif
