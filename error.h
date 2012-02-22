#ifndef ERROR_H
#define ERROR_H
/**
 * Error Handling
 */
#include <setjmp.h>

#define CO_TRY(b,a) if (setjmp((b)->buf) == 0) { a }
#define CO_THROW(b,c)              \
    {                              \
        (b)->status = c;           \
        longjmp((b)->buf, 1);      \
    }


/* chain list of exception buffers */
struct co_exception_buf {
    struct co_exception_buf *prev;
    jmp_buf buf;
    volatile int status; /* error code */
};

#endif
