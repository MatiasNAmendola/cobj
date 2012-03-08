#ifndef VM_OPCODES_H
#define VM_OPCODES_H

/* Instruction opcodes */

#define OP_PASS                     0
#define OP_ADD                      1
#define OP_SUB                      2
#define OP_MUL                      3
#define OP_DIV                      4
#define OP_MOD                      5
#define OP_IS_SMALLER               6
#define OP_IS_SMALLER_OR_EQUAL      7
#define OP_IS_EQUAL                 8
#define OP_IS_NOT_EQUAL             9
#define OP_ASSIGN                   10
#define OP_PRINT                    11
#define OP_JMPZ                     12
#define OP_JMP                      13
#define OP_DECLARE_FUNCTION         14
#define OP_RETURN                   15
#define OP_INIT_FCALL               16
#define OP_DO_FCALL                 17
#define OP_RECV_PARAM               18
#define OP_PASS_PARAM               19
#define OP_EXIT                     20
#define OP_TRY                      21
#define OP_THROW                    22
#define OP_CATCH                    23
#define OP_BIND_NAME                24

#endif
