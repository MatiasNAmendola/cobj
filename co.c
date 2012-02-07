#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "co.h"
#include "co_globals.h"
#include "co_globals_macros.h"
#include "co_hash.h"
#include "co_parser.h"
#include "co_scanner.h"
#include "co_compile.h"
#include "co_vm_execute.h"

void
coerror(const char *str)
{
    printf("syntax error: %s\n", str);
}

int
main(int argc, const char **argv)
{
    yyscan_t yyscanner;

    yylex_init(&yyscanner);

    if (argc > 1) {
        argv++;
        FILE *myin = fopen(*argv, "r");

        if (!myin) {
            exit(2);
        }
        yyset_in(myin, yyscanner);
    }

    init_compiler();
    init_executor();

    int i = coparse(yyscanner);

    co_vm_execute(CG(active_op_array));

#if CO_DEBUG
#endif

    yylex_destroy(yyscanner);
    return i;
}
