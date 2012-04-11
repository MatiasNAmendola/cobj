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
#define OP_CMP                      9
#define OP_UNARY_NEGATE             10
#define OP_UNARY_INVERT             11
#define OP_ASSIGN                   12
#define OP_PRINT                    13
#define OP_JMPZ                     14
#define OP_JMP                      15
#define OP_JMPX                     16
#define OP_DECLARE_FUNCTION         17
#define OP_RETURN                   18
#define OP_CALL_FUNCTION            19
#define OP_TRY                      20
#define OP_THROW                    21
#define OP_CATCH                    22
#define OP_LOAD_NAME                23
#define OP_LOAD_CONST               24
#define OP_TUPLE_BUILD              25
#define OP_LIST_BUILD               26
#define OP_LIST_ADD                 27
#define OP_DICT_BUILD               28
#define OP_DICT_ADD                 29

/* OP_CMP's opargs */
enum Cmp_OpArg {
    Cmp_LT = 0,
    Cmp_LE = 1,
    Cmp_EQ = 2,
    Cmp_NE = 3,
    Cmp_GT = 4,
    Cmp_GE = 5,
};

#endif
