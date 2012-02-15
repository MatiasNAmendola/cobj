#include "co.h"
#include "co_hash.h"
#include "co_parser.h"
#include "co_scanner.h"
#include "co_compile.h"
#include "co_vm_execute.h"

int
main(int argc, const char **argv)
{
    if (argc > 1) {
        argv++;
        int fd;
        fd = open(*argv, O_RDONLY);
        if (fd < 0) {
            die("open %s failed", *argv);
        }

        /* compilation */
        co_scanner_startup();
        co_scanner_openfile(fd);
        init_compiler();
        coparse(&compiler_globals);
        co_scanner_shutdown();

        /* vm execution */
        co_vm_init();
        co_vm_execute(CG(active_op_array));
    } else {
        printf("no file to execute\n");
    }

    return 0;
}
