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

int flag_verbose = 0;
int flag_compile = 0;
char *eval = NULL;

int
main(int argc, const char **argv)
{
    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", NULL, "print the version number and exit",
                    argparse_showversion),
        OPT_BOOLEAN('V', "verbose", &flag_verbose,
                    "show runtime info, can be supplied multiple times to increase verbosity",
                    NULL),
        OPT_BOOLEAN('c', "compile", &flag_compile, "compile only", NULL),
        OPT_STRING('e', "eval", &eval, "code passed as string", NULL),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);

    if (flag_verbose) {
        COObject *co = COList_New(0);
        COList_Append(co, COInt_FromLong(1));
        COList_Append(co, COInt_FromLong(2));
        COList_Append(co, COInt_FromLong(3));
        COObject *s = COObject_serialize(COList_AsTuple(co));
        COObject_dump(s);
        COObject_dump(COObject_unserialize(s));
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

    COCodeObject *co;
    co = co_compile();
    co_scanner_shutdown();

    if (flag_compile) {
        COObject_dump((COObject *)co);
        COObject *co_compiled_str = COObject_serialize((COObject *)co);
        co = COObject_unserialize(co_compiled_str);
        co_vm_execute(co);
        return 0;
    }

    co_vm_execute(co);
    return 0;
}
