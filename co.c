#include "co.h"
#include "hash.h"
#include "parser.h"
#include "scanner.h"
#include "compile.h"
#include "vm_execute.h"
#include "error.h"
#include "argparse/argparse.h"

static const char * const usagestr[] = {
    "co [options] [file] [args]",
    NULL,
};
static struct argparse_option options[] = {
    OPT_HELP(),
    OPT_END(),
};

int
main(int argc, const char **argv)
{

    struct argparse argparse;
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);
    int fd = 0;
    if (argc > 1) {
        argv++;
        fd = open(*argv, O_RDONLY);
        if (fd < 0) {
            error("open %s failed", *argv);
        }
    }

    /* compilation */
    co_scanner_startup();
    co_scanner_openfile(fd);
    init_compiler();
    coparse(&compiler_globals);
    co_scanner_shutdown();

    /* vm execution */
    co_vm_init();
    co_vm_execute(CG(active_opline_array));
    return 0;
}
