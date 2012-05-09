#ifndef OPCODES_H
#define OPCODES_H

/* Instruction opcodes */

#define OP_NOP                      0
#define OP_BINARY_ADD               1
#define OP_BINARY_SUB               2
#define OP_BINARY_MUL               3
#define OP_BINARY_DIV               4
#define OP_BINARY_MOD               5
#define OP_BINARY_POW               6
#define OP_BINARY_SL                7
#define OP_BINARY_SR                8
#define OP_BINARY_SUBSCRIPT         9
#define OP_CMP                      10
#define OP_UNARY_NEGATE             11
#define OP_UNARY_INVERT             12
#define OP_ASSIGN                   13
#define OP_PRINT                    14
#define OP_JMPZ                     15
#define OP_JMP                      16
#define OP_JMPX                     17
#define OP_DECLARE_FUNCTION         18
#define OP_RETURN                   19
#define OP_CALL_FUNCTION            20
#define OP_SETUP_TRY                21
#define OP_THROW                    22
#define OP_LOAD_NAME                23
#define OP_LOAD_CONST               24
#define OP_BUILD_TUPLE              25
#define OP_BUILD_LIST               26
#define OP_DICT_BUILD               27
#define OP_DICT_ADD                 28
#define OP_SETUP_LOOP               29
#define OP_POP_BLOCK                30
#define OP_BREAK_LOOP               31
#define OP_CONTINUE_LOOP            32
#define OP_POP_TRY                  33
#define OP_DUP_TOP                  34
#define OP_POP_TOP                  35
#define OP_END_TRY                  36
#define OP_SETUP_FINALLY            37
#define OP_END_FINALLY              38
#define OP_STORE_SUBSCRIPT          39

/* OP_CMP's opargs */
enum Cmp_OpArg {
    Cmp_LT = 0,
    Cmp_LE = 1,
    Cmp_EQ = 2,
    Cmp_NE = 3,
    Cmp_GT = 4,
    Cmp_GE = 5,
    Cmp_EXC_MATCH = 6,
};

char *opcode_name(unsigned char opcode);
#endif
