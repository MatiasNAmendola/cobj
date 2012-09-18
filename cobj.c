#include "cobj.h"
#include "version.h"

static const char *const usagestr[] = {
    "cobj [options] [file] [args]",
    NULL,
};

int
argparse_showversion(struct argparse *this,
                     const struct argparse_option *option)
{
    printf("CObj %s\n", cobj_version);
    exit(1);
}

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
eval_wrapper(COObject *code, COObject *globals)
{
    COObject *ret;

    COObject *func = COFunction_New(code);
    ret = vm_eval(func, globals);
    if (!ret) {
        if (COErr_Occurred()) {
            COErr_Print();
            return NULL;
        }
    }
    CO_DECREF(func);
    CO_DECREF(globals);
    return ret;
}

int
run_file(FILE *fp, const char *filename, COObject *globals)
{
    COObject *ret;
    struct arena *arena = arena_new();
    scanner_init(arena);

    COObject *f_filename = COStr_FromString(filename);
    COObject *mode = COStr_FromString("r");
    COObject *f = COFile_FromFile(fp, f_filename, mode);
    CO_DECREF(f_filename);
    CO_DECREF(mode);

    COObject *source = COFile_Read(f, -1);

    scanner_setcode(COBytes_AsString(source));
    COObject *code = compile(arena);
    ret = eval_wrapper(code, globals);
    CO_DECREF(source);
    CO_DECREF(f);
    CO_DECREF(code);
    COGlobalState_Delete(COGlobalState_Current);
    arena_free(arena);
    return ret ? 1 : 0;
}

int
run_string(const char *str, COObject *globals)
{
    COObject *ret;
    struct arena *arena = arena_new();
    scanner_init(arena);
    scanner_setcode((char *)str);
    COObject *code = compile(arena);
    ret = eval_wrapper(code, globals);
    CO_DECREF(code);
    COGlobalState_Delete(COGlobalState_Current);
    arena_free(arena);
    return ret ? 1 : 0;
}


int
main(int argc, const char **argv)
{
    struct argparse argparse;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", NULL, "print the version number and exit",
                    argparse_showversion),
        OPT_STRING('e', "eval", &eval, "code passed as string", NULL),
        OPT_END(),
    };
    argparse_init(&argparse, options, usagestr, 0);
    argc = argparse_parse(&argparse, argc, argv);

    /* Initialize */
    COInt_Init();
    COFrame_Init();
    COObject_GC_Init();
    COThreadStateObject *ts = COThreadState_New();
    COGlobalState_Current = COGlobalState_New(ts);
    COObject *globals = CODict_New();

    /* Run */
    int ret = 0;
    if (eval) {
        ret = run_string(eval, globals);
    } else {
        FILE *f = stdin;
        const char *f_name = "<stdin>";
        if (argc > 0) {
            f = fopen(*argv, "rb");
            if (f == NULL) {
                error("open %s failed", *argv);
            }
            f_name = *argv;
        }

        if (isatty((int)fileno(f))) {
            char *eval;
            linenoiseSetCompletionCallback(cli_completion);
            char *home = getenv("HOME");
            char history[PATH_MAX];
            char *history_path = NULL;
            if (home) {
                history_path =
                    mksnpath(history, sizeof(history), "%s/.co_history", home);
                linenoiseHistoryLoad(history_path);
            }
            printf("CObj %s\n", cobj_version);
            /* Read-Eval-Print Loop */
            struct arena *arena;
            arena = arena_new();
            scanner_init(arena);
            COObject *ret;
            COObject *globals = CODict_New();
            while ((eval = linenoise(">>> ")) != NULL) {
                scanner_setcode(eval);
                COObject *code = compile(arena);
                COObject *func = COFunction_New(code);
                ret = vm_eval(func, globals);
                if (!ret) {
                    if (COErr_Occurred()) {
                        COErr_Print();
                    }
                } else if (ret != CO_None) {
                    COObject_Print(ret, stdout);
                }
                CO_DECREF(code);
                CO_DECREF(func);

                linenoiseHistoryAdd(eval);
                if (history_path) {
                    linenoiseHistorySave(history_path);
                }
                free(eval);
            }
            arena_free(arena);
        } else {
            ret = run_file(f, f_name, globals);
        }
    }

    /* Finialize */
    COFrame_Fini();
    COObject_GC_Collect();
}
