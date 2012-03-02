#ifndef ERROR_H
#define ERROR_H
/**
 * Error Handling
 *
 * @example
 *  co_try(&exbuf) {
 *      // throw in current block
 *      co_throw(&exbuf, 1)
 *      // throw in sub function 
 *      call_func();
 *      // nested try
 *      try(&exbuf) {
 *         co_throw(&exbuf, 3);
 *      } co_catch(&exbuf, 3) {
 *          // do if code is 3 
 *      }
 *  } co_catch(&exbuf, 1) {
 *      // do, if code is 1
 *  } co_catch(&exbuf, 2) {
 *      // do, if code is 2
 *  } co_end_try
 */
#include <setjmp.h>

/* chain list of exception buffers */
struct co_exception_buf {
    struct co_exception_buf *prev;
    jmp_buf buf;
};

#define co_try(b)                       \
    do {                                \
        struct co_exception_buf ex_buf; \
        ex_buf.prev = (b);              \
        (b) = &ex_buf;                  \
        switch (setjmp((b)->buf)) {     \
        case 0:                         
#define co_throw(b,x)   longjmp((b)->buf, x)
#define co_catch(b,x)               \
    break;                          \
    case x:
#define co_end_try(b)               \
        (b) = (b)->prev;            \
        }} while(0)

#endif
