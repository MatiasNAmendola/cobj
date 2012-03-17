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
        COObject *t = COList_New(3);
        COList_SetItem(t, 0, (COObject *)COInt_FromLong(1));
        COList_SetItem(t, 1, (COObject *)COInt_FromLong(2));
        COList_SetItem(t, 2, (COObject *)COInt_FromLong(3));
        t = COList_AsTuple(t);
        COObject_dump(t);

        COObject *d = CODict_New();
        CODict_SetItem(d, COStr_FromString("key1"), t);
        CODict_SetItem(d, COStr_FromString("key2"), d);
        COObject_dump(d);
        COObject *r = CODict_GetItem(d, COStr_FromString("key1"));
        COObject_dump(r);
        printf("size: %ld\n", CODict_Size(d));
        CODict_DelItem(d, COStr_FromString("key1"));
        printf("delete one, %p, size: %ld\n",
                CODict_GetItem(d, COStr_FromString("key1")), CODict_Size(d));
        CODict_Clear(d);
        printf("size: %ld\n", CODict_Size(d));
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

    co_vm_execute(co);
    return 0;
}
