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
int flag_compile = 0;
char *eval = NULL;

void
cli_completion(const char *buf, linenoiseCompletions *lc)
{
    static const char *words[] = {
        "print",
        "if",
        "else",
        "elif",
        "try",
        "throw",
        "catch",
        "finally",
        "func",
        "while",
        "end",
        "None",
        "True",
        "False",
    };

    for (int i = 0; i < ARRAY_SIZE(words); i++) {
        if (!prefixcmp(words[i], buf)) {
            linenoiseAddCompletion(lc, (char *)words[i]);
        }
    }
}

COObject *
eval_wrapper(COObject *co)
{
    COObject *ret;

    COObject *func = COFunction_New(co);
    ret = vm_eval(func);
    if (!ret) {
        if (COErr_Occurred()) {
            COErr_Print();
            return NULL;
        }
    }
    return ret;
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
        OPT_BOOLEAN('c', "compile", &flag_compile, "compile only", NULL),
        OPT_STRING('e', "eval", &eval, "code passed as string", NULL),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr);
    argc = argparse_parse(&argparse, argc, argv);

    /* Init */
    COInt_Init();
    threadstate_current = COThreadState_New();

    /* test only */
    if (verbose) {
        return 0;
    }

    /* compilation */
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
            first_object = unmarshal_fromfile(f);
            if (first_object && COInt_Check(first_object)
                && COInt_AsLong(first_object) == CODEDUMP_MAGIC) {
                COObject *code = unmarshal_fromfile(f);
                if (!code) {
                    return -1;
                }
                return eval_wrapper(code) ? 0 : -1;
            }
        }
        fseek(f, 0, SEEK_SET);

        if (isatty((int)fileno(f))) {
            char *code;
            linenoiseSetCompletionCallback(cli_completion);
            char *home = getenv("HOME");
            char history[PATH_MAX];
            char *history_path = NULL;
            if (home) {
                history_path =
                    mksnpath(history, sizeof(history), "%s/.co_history", home);
                linenoiseHistoryLoad(history_path);
            }
            printf("COObject 0.1\n");
            /* Read-Eval-Print Loop */
            while ((code = linenoise(">>> ")) != NULL) {
                co_scanner_setcode(code);
                eval_wrapper(compile());
                linenoiseHistoryAdd(code);
                if (history_path) {
                    linenoiseHistorySave(history_path);
                }
                free(code);
            }
        } else {
            co_scanner_setfile(COFile_FromFile(f, (char *)f_name, "r", fclose));
        }
    }

    COObject *code = compile();
    if (flag_compile) {
        FILE *f;
        f = fopen("a.out", "wb");
        marshal_tofile(COInt_FromLong(CODEDUMP_MAGIC), f);
        marshal_tofile(code, f);
        return 0;
    }

    return eval_wrapper(code) ? 0 : -1;
}
