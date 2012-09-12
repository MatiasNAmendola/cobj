#include "cobj.h"

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
        GIVE_NAME(OP_BINARY_SUBSCRIPT);
        GIVE_NAME(OP_UNARY_NEGATE);
        GIVE_NAME(OP_UNARY_INVERT);
        GIVE_NAME(OP_CMP);
        GIVE_NAME(OP_STORE_NAME);
        GIVE_NAME(OP_JMPZ);
        GIVE_NAME(OP_JMP);
        GIVE_NAME(OP_JMPX);
        GIVE_NAME(OP_MAKE_FUNCTION);
        GIVE_NAME(OP_MAKE_CLASS);
        GIVE_NAME(OP_RETURN);
        GIVE_NAME(OP_CALL_FUNCTION);
        GIVE_NAME(OP_SETUP_TRY);
        GIVE_NAME(OP_THROW);
        GIVE_NAME(OP_LOAD_NAME);
        GIVE_NAME(OP_LOAD_CONST);
        GIVE_NAME(OP_BUILD_TUPLE);
        GIVE_NAME(OP_BUILD_LIST);
        GIVE_NAME(OP_DICT_BUILD);
        GIVE_NAME(OP_DICT_ADD);
        GIVE_NAME(OP_SETUP_LOOP);
        GIVE_NAME(OP_POP_BLOCK);
        GIVE_NAME(OP_BREAK_LOOP);
        GIVE_NAME(OP_CONTINUE_LOOP);
        GIVE_NAME(OP_POP_TRY);
        GIVE_NAME(OP_DUP_TOP);
        GIVE_NAME(OP_POP_TOP);
        GIVE_NAME(OP_END_TRY);
        GIVE_NAME(OP_SETUP_FINALLY);
        GIVE_NAME(OP_END_FINALLY);
        GIVE_NAME(OP_STORE_SUBSCRIPT);
        GIVE_NAME(OP_LOAD_LOCAL);
        GIVE_NAME(OP_LOAD_UPVAL);
        GIVE_NAME(OP_STORE_UPVAL);
        GIVE_NAME(OP_STORE_LOCAL);
        GIVE_NAME(OP_GET_ITER);
        GIVE_NAME(OP_FOR_ITER);
        GIVE_NAME(OP_GET_ATTR);
        GIVE_NAME(OP_SET_ATTR);
        GIVE_NAME(OP_DEL_ATTR);
    }
    error("unknown opcode: %d\n", opcode);
    return NULL;
}
