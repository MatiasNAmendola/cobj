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

/** cnode type **/
#define IS_CONST        (1<<0)
#define IS_TMP_VAR      (1<<1)
#define IS_VAR          (1<<2)
#define IS_UNUSED       (1<<3)
#define SET_UNUSED(op)  (op).op_type = IS_UNUSED

#define CG(v)   compiler_globals.v
#define EG(v)   executor_globals.v

typedef struct _cval cval;
typedef struct _cnode cnode;
typedef struct _co_op co_op;
typedef struct _co_op_array co_op_array;
typedef union _temp_variable temp_variable;

/**
 * Function
 */
typedef struct Function {
    co_op_array *op_array;
    int numparams;              /* number of positional parameters */
} Function;

/* c value container */
struct _cval {
    union {
        long ival;              /* int value */
        double fval;            /* float value */
        struct {
            char *val;
            int len;
        } str;                  /* string value */
        Function *func;
    } u;
    uchar type;
};

/* c node */
struct _cnode {
    int op_type;
    union {
        cval val;
        uint var;
        uint opline_num;
    } u;
};

/* c op for opcode */
struct _co_op {
    uchar opcode;
    cnode result;
    cnode op1;
    cnode op2;
};

/* c op array */
struct _co_op_array {
    uchar type;
    co_op *ops;
    co_op *start_op;
    uint last, size;
    uint t;                     // number of temp variables
};

/* temp variable */
union _temp_variable {
    cval tmp_var;
};

/* compiler */
void init_compiler();
void init_op_array(co_op_array *op_array, int ops_size);

/* parser-driven code generators */
void co_binary_op(uchar opcode, cnode *result, const cnode *op1, const cnode *op2);
void co_print(const cnode *op);
void co_assign(cnode *result, cnode *variable, const cnode *op);
void co_if_cond(const cnode *cond, cnode *closing_bracket_token);
void co_if_after_statement(cnode *closing_bracket_token);
void co_if_end(const cnode *closing_bracket_token);
void co_while_cond(const cnode *cond, cnode *while_token, cnode *closing_bracket_token);
void co_while_end(const cnode *while_token, const cnode *closing_bracket_token);
void co_begin_function_declaration(cnode *function_token);
void co_end_function_declaration(const cnode *function_token);
void co_begin_function_call(cnode *function_name);
void co_end_function_call(cnode *function_name, cnode *result);
void co_pass_param(cnode *param);
void co_recv_param(cnode *param);
void co_end_compilation();

/* opcode */
co_op *get_next_op(co_op_array *op_array);

/* cval handlers */
cval *getcval(const char *name);
bool putcval(const char *name, cval *val);
bool delcval(const char *name);

// compiler globals
typedef struct _co_compiler_globals {
    HashTable variable_symboltable;
    co_op_array *active_op_array;
} co_compiler_globals;

// parser
int coparse();
#define coerror die

// scanner
int colex(cnode *colval);
int co_scanner_lex(cnode *yylval);
void co_scanner_startup(void);
void co_scanner_shutdown(void);
int co_scanner_openfile(int fd);

co_compiler_globals compiler_globals;

#endif
