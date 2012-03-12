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

static const char * const usagestr[] = {
    "co [options] [file] [args]",
    NULL,
};

int
argparse_showversion(struct argparse *this, const struct argparse_option *option)
{
    printf("CObject 0.01\n");
    exit(1);
}

int
main(int argc, const char **argv)
{

    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", NULL, "print the version number and exit", argparse_showversion),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);

    struct COStrObject *str;
    /*str = (struct COStrObject *)COStrObject_FromString("hello world");*/
    str = CO_TYPE(CO_True)->tp_repr(CO_True);
    printf("len: %ld, str: %s\n", str->co_len, str->co_str);
    str = CO_TYPE(CO_False)->tp_repr(CO_False);
    printf("len: %ld, str: %s\n", str->co_len, str->co_str);
    str = CO_TYPE(&COType_Type)->tp_repr(&COType_Type);
    printf("len: %ld, str: %s\n", str->co_len, str->co_str);

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
