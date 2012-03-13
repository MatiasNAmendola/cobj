#include "co.h"
#include "hash.h"
#include "parser.h"
#include "scanner.h"
#include "compile.h"
#include "vm_execute.h"
#include "error.h"
#include "argparse/argparse.h"
#include "object.h"
#include "objects/strobject.h"
#include "objects/boolobject.h"
#include "objects/floatobject.h"

static const char *const usagestr[] = {
    "co [options] [file] [args]",
    NULL,
};

int
argparse_showversion(struct argparse *this,
                     const struct argparse_option *option)
{
    printf("CObject 0.01\n");
    exit(1);
}

int
main(int argc, const char **argv)
{
    int verbose = 0;
    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", NULL, "print the version number and exit",
                    argparse_showversion),
        OPT_BOOLEAN('V', "verbose", &verbose,
                    "show runtime info, can be supplied multiple times to increase verbosity",
                    NULL),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);

    if (verbose) {
        coobject_print(CO_None);
        coobject_print(CO_True);
        coobject_print(CO_False);
        coobject_print((struct COObject *)&COType_Type);
        coobject_print((struct COObject *)CO_TYPE(&COType_Type));
        coobject_print(COInt_FromString("123456", 0));
        coobject_print(COFloat_FromString("1.3456"));
    }

    int fd = 0;
    if (argc > 0) {
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
