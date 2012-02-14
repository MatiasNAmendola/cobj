#include "co.h"
#include "co_hash.h"
#include "co_parser.h"
#include "co_scanner.h"
#include "co_compile.h"
#include "co_vm_execute.h"

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
    yyparse(yyscanner);
    yylex_destroy(yyscanner);

    co_vm_init();
    co_vm_execute(CG(active_op_array));
    return 0;
}
