#include "co.h"
#include "hash.h"
#include "parser.h"
#include "scanner.h"
#include "compile.h"
#include "vm_execute.h"
#include "error.h"

int
main(int argc, const char **argv)
{
    int fd = 0;
    if (argc > 1) {
        argv++;
        fd = open(*argv, O_RDONLY);
        if (fd < 0) {
            coerror("open %s failed", *argv);
        }
    }

    /* compilation */
    co_scanner_startup();
    co_scanner_openfile(fd);
    init_compiler();
    coparse(&compiler_globals);
    co_scanner_shutdown();

    /* vm execution */
    co_try(&EG(exception_buf)) {
        co_vm_init();
        co_vm_execute(CG(active_op_array));
    } co_catch(&EG(exception_buf), 1) {
        printf("NameError: name 'unknown' is not defined\n");
    } co_end_try;
    return 0;
}
