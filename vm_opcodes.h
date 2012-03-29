#ifndef VM_OPCODES_H
#define VM_OPCODES_H

/* Instruction opcodes */

#define OP_NOP                      0
#define OP_ADD                      1
#define OP_SUB                      2
#define OP_MUL                      3
#define OP_DIV                      4
#define OP_MOD                      5
#define OP_POW                      6
#define OP_SL                       7
#define OP_SR                       8
#define OP_IS_SMALLER               9
#define OP_IS_SMALLER_OR_EQUAL      10
#define OP_IS_EQUAL                 11
#define OP_IS_NOT_EQUAL             12
#define OP_ASSIGN                   13
#define OP_PRINT                    14
#define OP_JMPZ                     15
#define OP_JMP                      16
#define OP_DECLARE_FUNCTION         17
#define OP_RETURN                   18
#define OP_DO_FCALL                 19
#define OP_RECV_PARAM               20
#define OP_PASS_PARAM               21
#define OP_TRY                      22
#define OP_THROW                    23
#define OP_CATCH                    24
#define OP_LOAD_NAME                25
#define OP_LOAD_CONST               26
#define OP_TUPLE_BUILD              27
#define OP_LIST_BUILD               28
#define OP_LIST_ADD                 29
#define OP_POP_TOP                  30
#define OP_DICT_BUILD               31
#define OP_DICT_ADD                 32

#endif
