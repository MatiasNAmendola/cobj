#ifndef CO_OPCODE_H
#define CO_OPCODE_H

/* Instruction opcodes for compiled code */
#define OP_PASS             0
#define OP_ADD              1 
#define OP_SUB              2
#define OP_MUL              3
#define OP_DIV              4
#define OP_MOD              5
#define OP_IS_SMALLER       6
#define OP_ASSIGN           7
#define OP_PRINT            8
#define OP_JMPZ             9
#define OP_JMP              10
#define OP_DECLARE_FUNCTION 11
#define OP_RETURN           12
#define OP_INIT_FCALL       13
#define OP_DO_FCALL         14
#define OP_RECV_PARAM       15
#define OP_PASS_PARAM       16
#define OP_EXIT             17

#endif
