#include "debug.h"
#include "compile.h"

void
co_print_opcode(co_op_array *op_array)
{
    static const char* opcode_names[] = {
        "OP_PASS",
        "OP_ADD", 
        "OP_SUB",
        "OP_MUL",
        "OP_DIV",
        "OP_MOD",
        "OP_IS_SMALLER",
        "OP_ASSIGN",
        "OP_PRINT",
        "OP_JMPZ",
        "OP_JMP",
        "OP_DECLARE_FUNCTION",
        "OP_RETURN",
        "OP_INIT_FCALL",
        "OP_DO_FCALL",
        "OP_RECV_PARAM",
        "OP_PASS_PARAM",
        "OP_EXIT",
    };
    for (int i = 0; i < op_array->last; i++) {
        co_op *op = &op_array->ops[i];
        printf("opcode[%d]: %s\n", i, opcode_names[op->opcode]);
    }
}
