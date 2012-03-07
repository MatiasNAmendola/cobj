#ifndef COMPILE_H
#define COMPILE_H

#include "co.h"
#include "hash.h"
#include "stack.h"

/** cval type **/
#define CVAL_IS_NONE        1
#define CVAL_IS_INT         2
#define CVAL_IS_FLOAT       3
#define CVAL_IS_BOOL        4
#define CVAL_IS_STRING      5
#define CVAL_IS_FUNCTION    6

/** struct cnode type **/
#define IS_CONST        (1<<0)
#define IS_TMP_VAR      (1<<1)
#define IS_VAR          (1<<2)
#define IS_UNUSED       (1<<3)
#define SET_UNUSED(op)  (op).type = IS_UNUSED

#define CG(v)   compiler_globals.v

/**
 * Function
 */
struct Function {
    struct co_opline_array *op_array;
    int numparams;              /* number of positional parameters */
};

/* 
 * value container
 */
struct cval {
    union {
        long ival;              /* int value */
        double fval;            /* float value */
        struct {
            char *val;
            int len;
        } str;                  /* string value */
        struct Function *func;
    } u;
    uchar type;
};

/*
 * op node
 */
struct cnode {
    int type;
    union {
        struct cval val;
        uint var;
        uint opline_num;
    } u;
};

/*
 * op line
 */
struct co_opline {
    uchar opcode;
    struct cnode result;
    struct cnode op1;
    struct cnode op2;
};

/*
 * op array
 */
struct co_opline_array {
    struct co_opline *ops;
    uint last, size;
    uint t;                     // number of temp variables
};

/* temp variable */
union temp_variable {
    struct cval tmp_var;
};

/* compiler */
void init_compiler();
void init_op_array(struct co_opline_array *op_array, int ops_size);

/* parser-driven code generators */
void co_binary_op(uchar opcode, struct cnode *result, const struct cnode *op1,
                  const struct cnode *op2);
void co_print(const struct cnode *op);
void co_assign(struct cnode *result, struct cnode *variable, const struct cnode *op);
void co_if_cond(const struct cnode *cond, struct cnode *closing_bracket_token);
void co_if_after_statement(struct cnode *closing_bracket_token);
void co_if_end(const struct cnode *closing_bracket_token);
void co_while_cond(const struct cnode *cond, struct cnode *while_token,
                   struct cnode *closing_bracket_token);
void co_while_end(const struct cnode *while_token, const struct cnode *closing_bracket_token);
void co_begin_function_declaration(struct cnode *function_token);
void co_end_function_declaration(const struct cnode *function_token);
void co_begin_function_call(struct cnode *function_name);
void co_end_function_call(struct cnode *function_name, struct cnode *result);
void co_pass_param(struct cnode *param);
void co_recv_param(struct cnode *param);
void co_end_compilation();

/* opcode */
struct co_opline *get_next_op(struct co_opline_array *op_array);

// compiler globals
struct co_compiler_globals {
    struct co_opline_array *active_op_array;
} co_compiler_globals;

// parser
int coparse();
void coerror(const char *err, ...);

// scanner
int colex(struct cnode *colval);
int co_scanner_lex(struct cnode *yylval);
void co_scanner_startup(void);
void co_scanner_shutdown(void);
int co_scanner_openfile(int fd);

// compiler
struct co_compiler_globals compiler_globals;

#endif
