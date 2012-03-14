#ifndef COMPILE_H
#define COMPILE_H

#include "co-compat.h"
#include "hash.h"
#include "stack.h"
#include "llist.h"
#include "object.h"
#include "objects/listobject.h"

/** cnode type **/
#define IS_CONST        (1<<0)
#define IS_TMP_VAR      (1<<1)
#define IS_VAR          (1<<2)
#define IS_UNUSED       (1<<3)
#define SET_UNUSED(op)  (op).type = IS_UNUSED

#define CG(v)   compiler_globals.v

/*
 * op node
 */
struct cnode {
    int type;
    union {
        COObject *co;
        uint var;
        int opline_num;
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
 * op block
 */
struct co_opline_array {
    struct co_opline *ops;
    uint last, size;
    uint t;                     // number of temp variables
    struct co_opline_array *prev_opline_array;
};

/* temp variable */
union temp_variable {
    COObject *tmp_co;
};

/* compiler */
void init_compiler();
void init_opline_array(struct co_opline_array *opline_array, int ops_size);

/* parser-driven code generators */
void co_binary_op(uchar opcode, struct cnode *result, const struct cnode *op1,
                  const struct cnode *op2);
void co_print(const struct cnode *op);
void co_assign(struct cnode *result, struct cnode *variable,
               const struct cnode *op);
void co_if_cond(const struct cnode *cond, struct cnode *closing_bracket_token);
void co_if_after_statement(struct cnode *closing_bracket_token);
void co_if_end(const struct cnode *closing_bracket_token);
void co_while_cond(const struct cnode *cond, struct cnode *while_token,
                   struct cnode *closing_bracket_token);
void co_while_end(const struct cnode *while_token,
                  const struct cnode *closing_bracket_token);
void co_begin_function_declaration(struct cnode *function_token,
                                   struct cnode *function_name);
void co_end_function_declaration(const struct cnode *function_token,
                                 struct cnode *result);
void co_begin_function_call(struct cnode *function_name);
void co_end_function_call(struct cnode *function_name, struct cnode *result);
void co_pass_param(struct cnode *param);
void co_recv_param(struct cnode *param);
void co_return(const struct cnode *expr);
void co_list_build(struct cnode *result, struct cnode *tag);
void co_append_element(struct cnode *node, struct cnode *element);
void co_end_compilation();

/* opcode */
struct co_opline *get_next_op(struct co_opline_array *opline_array);

// compiler globals
struct co_compiler_globals {
    struct co_opline_array *active_opline_array;
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
