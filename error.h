#ifndef ERROR_H
#define ERROR_H
/**
 * Error Handling
 *
 * @example
 *  co_try(&exbuf) {
 *     // do some stuff 
 *     co_throw(&exbuf, 1)
 *  } co_catch(&exbuf, 1) {
 *  } co_catch(&exbuf, 2) {
 *  }
 */
#include <setjmp.h>

#define co_try(b)                   \
    do {                            \
        switch (setjmp((b)->buf)) { \
        case 0:                         
#define co_throw(b,x)   longjmp((b)->buf, x)
#define co_catch(b,x)               \
    break;                          \
    case x:
#define co_end_try  }} while(0)

/* chain list of exception buffers */
struct co_exception_buf {
    struct co_exception_buf *prev;
    jmp_buf buf;
};

#endif
