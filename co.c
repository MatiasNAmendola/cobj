#include "co.h"

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
        COObject_dump(CO_None);
        COObject_dump(CO_True);
        COObject_dump(CO_False);
        COObject_dump((COObject *)&COType_Type);
        COObject_dump((COObject *)CO_TYPE(&COType_Type));
        COObject_dump(COInt_FromString("123456", 0));
        COObject_dump(COFloat_FromString("1.3456"));
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
