#include "co.h"

char *
opcode_name(unsigned char opcode)
{
#define GIVE_NAME(type) \
    case (type):        \
        return #type

    switch (opcode) {
        GIVE_NAME(OP_NOP);
        GIVE_NAME(OP_BINARY_ADD);
        GIVE_NAME(OP_BINARY_SUB);
        GIVE_NAME(OP_BINARY_MUL);
        GIVE_NAME(OP_BINARY_DIV);
        GIVE_NAME(OP_BINARY_MOD);
        GIVE_NAME(OP_BINARY_POW);
        GIVE_NAME(OP_BINARY_SL);
        GIVE_NAME(OP_BINARY_SR);
        GIVE_NAME(OP_UNARY_NEGATE);
        GIVE_NAME(OP_UNARY_INVERT);
        GIVE_NAME(OP_CMP);
        GIVE_NAME(OP_ASSIGN);
        GIVE_NAME(OP_PRINT);
        GIVE_NAME(OP_JMPZ);
        GIVE_NAME(OP_JMP);
        GIVE_NAME(OP_JMPX);
        GIVE_NAME(OP_DECLARE_FUNCTION);
        GIVE_NAME(OP_RETURN);
        GIVE_NAME(OP_CALL_FUNCTION);
        GIVE_NAME(OP_TRY);
        GIVE_NAME(OP_THROW);
        GIVE_NAME(OP_CATCH);
        GIVE_NAME(OP_LOAD_NAME);
        GIVE_NAME(OP_LOAD_CONST);
        GIVE_NAME(OP_TUPLE_BUILD);
        GIVE_NAME(OP_LIST_BUILD);
        GIVE_NAME(OP_LIST_ADD);
        GIVE_NAME(OP_DICT_BUILD);
        GIVE_NAME(OP_DICT_ADD);
    }
    error("unknow opcode: %d\n", opcode);
    return NULL;
}
