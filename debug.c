#include "co.h"

void
co_print_opcode(struct co_opline_array *opline_array)
{
    static const char *opcode_names[] = {
        "OP_PASS",
        "OP_ADD",
        "OP_SUB",
        "OP_MUL",
        "OP_DIV",
        "OP_MOD",
        "OP_SL",
        "OP_SR",
        "OP_IS_SMALLER",
        "OP_IS_SMALLER_OR_EQUAL",
        "OP_IS_EQUAL",
        "OP_IS_NOT_EQUAL",
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
        "OP_TRY",
        "OP_THROW",
        "OP_CATCH",
        "OP_BIND_NAME",
    };
    for (int i = 0; i < opline_array->last; i++) {
        struct co_opline *op = &opline_array->ops[i];
        printf("opcode[%d]: %s, %d, %d, %d\n", i, opcode_names[op->opcode],
               op->op1.type, op->op2.type, op->result.type);
    }
}
