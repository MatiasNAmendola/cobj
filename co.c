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

int verbose = 0;
char *eval = NULL;

int
main(int argc, const char **argv)
{
    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", NULL, "print the version number and exit",
                    argparse_showversion),
        OPT_BOOLEAN('V', "verbose", &verbose,
                    "show runtime info, can be supplied multiple times to increase verbosity",
                    NULL),
        OPT_STRING('e', "eval", &eval, "code passed as string", NULL),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);

    if (verbose) {
        COTupleObject *t = COTuple_New(3);
        COTuple_SetItem(t, 0, COInt_FromLong(1));
        COTuple_SetItem(t, 1, COInt_FromLong(2));
        COTuple_SetItem(t, 2, COInt_FromLong(3));
        COObject_dump(t);
        return 0;
    }


    /* compilation */
    co_scanner_startup();
    if (eval) {
        co_scanner_setcode(eval);
    } else {
        int fd = 0;
        if (argc > 0) {
            fd = open(*argv, O_RDONLY);
            if (fd < 0) {
                error("open %s failed", *argv);
            }
        }
        co_scanner_openfile(fd);
    }

    init_compiler();
    coparse(&compiler_globals);
    co_scanner_shutdown();

    COFunctionObject *func = (COFunctionObject *)COFunctionObject_New(NULL);
    func->opline_array = CG(active_opline_array);
    co_hash_init(&func->upvalues, 1, NULL);

    /* vm execution */
    co_vm_init();
    co_vm_execute(func);
    co_vm_shutdown();
    return 0;
}
