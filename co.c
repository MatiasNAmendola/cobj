#include "co.h"

/* Change whenever the bytecode emmited by the compiler may no longer be
 * understood by old code evaluator.
 */
#define CODEDUMP_MAGIC  (314 << 16 | 'c' << 8 | 'o')

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
int compile = 0;
char *eval = NULL;
char *compile_outfile = NULL;

void
cli_completion(const char *buf, linenoiseCompletions *lc)
{
    // TODO
    if (buf[0] == 'p') {
        linenoiseAddCompletion(lc, "print");
    }
}

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
        OPT_BOOLEAN('c', "compile", &compile,
                    "compile only, write compiled code to file", NULL),
        OPT_STRING('o', "outfile", &compile_outfile,
                   "compile output file, defaults to `co.out`", NULL),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);

    /* Init */
    COInt_Init();
    threadstate_current = COThreadState_New();
    TS(frame) = COFrame_New();

    /* test only */
    if (verbose) {
        COObject *d = CODict_New();
        CODict_SetItem(d, COInt_FromLong(0), COInt_FromLong(100));
        CODict_SetItem(d, COInt_FromLong(1), COInt_FromLong(200));
        CODict_SetItem(d, COInt_FromLong(2), d);

        COObject *d_s = COObject_serialize(d);
        d = COObject_unserialize(d_s);
        COObject_dump(d);
        COObject_dump(CODict_GetItem(d, COInt_FromLong(0)));
        COObject_dump(CODict_GetItem(d, COInt_FromLong(1)));
        COObject_dump(CODict_GetItem(d, COInt_FromLong(2)));

        return 0;
    }

    /* compilation */
    co_scanner_startup();
    if (eval) {
        co_scanner_setcode(eval);
    } else {
        FILE *f = stdin;
        const char *f_name = "<stdin>";
        if (argc > 0) {
            f = fopen(*argv, "rb");
            if (f == NULL) {
                error("open %s failed", *argv);
            }
            f_name = *argv;
            // detect if it's a code cache
            COObject *first_object;
            first_object = COObject_unserializeFromFile(f);
            if (first_object && COInt_Check(first_object)
                && COInt_AsLong(first_object) == CODEDUMP_MAGIC) {
                COObject *code = COObject_unserializeFromFile(f);
                if (!code) {
                    // TODO invalid code dump
                    return -1;
                }
                co_vm_eval(code);
                return 0;
            }
        }
        fseek(f, 0, SEEK_SET);

        if (isatty((int)fileno(f))) {
            // TODO, current each statement is executed separately
            char *code;
            linenoiseSetCompletionCallback(cli_completion);
            while ((code = linenoise("co> ")) != NULL) {
                co_scanner_setcode(code);
                co_vm_eval(co_compile());
            }
        } else {
            co_scanner_setfile(COFile_FromFile(f, (char *)f_name, "r", fclose));
        }
    }

    COCodeObject *co;
    co = co_compile();
    co_scanner_shutdown();

    if (compile) {
        FILE *f;
        if (compile_outfile) {
            f = fopen(compile_outfile, "w");
        } else {
            f = fopen("co.out", "w");
        }
        COObject_serializeToFile(COInt_FromLong(CODEDUMP_MAGIC), f);
        COObject_serializeToFile((COObject *)co, f);
        return 0;
    }

    co_vm_eval(co);
    return 0;
}
