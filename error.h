#ifndef ERROR_H
#define ERROR_H
/**
 * Error Handling
 *
 * @example
 */
#include <setjmp.h>

/**
 * C Exception Emulator
 *
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

/* ! C Exception Emulator */

void COErr_Print(void);
COObject *COErr_Occurred(void);
void COErr_Restore(COObject *type, COObject *value, COObject *traceback);
void COErr_Fetch(COObject **type, COObject **value, COObject **traceback);
void COErr_SetObject(COObject *exception, COObject *value);
void COErr_Format(COObject *exception, const char *fmt, ...);
void COErr_SetString(COObject *exception, const char *str);

/* Convenience functions */
#define COErr_BadInternalCall() _COErr_BadInternalCall(__FILE__, __LINE__)
void _COErr_BadInternalCall(const char *filename, int lineno);
void COErr_NoMemory(void);
/* ! Convenience function */
#endif
