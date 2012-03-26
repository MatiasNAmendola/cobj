#ifndef COMPILE_H
#define COMPILE_H

#include "co_compat.h"
#include "object.h"
#include "objects/listobject.h"

/** cnode type **/
#define IS_CONST        (1<<0)
#define IS_TMP_VAR      (1<<1)
#define IS_VAR          (1<<2)
#define IS_UNUSED       (1<<3)

struct cnode {
    int type;
    union {
        COObject *co;
        uint var;
        int opline_num;
    } u;
};

/* compiler */
COObject *co_compile(void);
uint co_get_next_opline_num(void);

/* parser-driven code generators */
void co_binary_op(uchar opcode, struct cnode *result, const struct cnode *op1,
                  const struct cnode *op2);
void co_print(const struct cnode *op);
void co_assign(struct cnode *result, struct cnode *variable,
               const struct cnode *op);
void co_if_cond(const struct cnode *cond, struct cnode *if_token);
void co_if_after_stmt(struct cnode *if_token);
void co_if_end(const struct cnode *if_token);
void co_while_cond(const struct cnode *cond, struct cnode *while_token);
void co_while_end(const struct cnode *while_token);
void co_begin_func_declaration(struct cnode *func_token,
                               struct cnode *func_name);
void co_end_func_declaration(const struct cnode *func_token,
                             struct cnode *result);
void co_end_func_call(struct cnode *func_name, struct cnode *result);
void co_pass_param(struct cnode *param);
void co_recv_param(struct cnode *param);
void co_return(const struct cnode *expr);
void co_list_build(struct cnode *result, struct cnode *tag);
void co_list_add(struct cnode *node, struct cnode *element);
void co_dict_build(struct cnode *result, struct cnode *tag);
void co_dict_add(struct cnode *node, struct cnode *key, struct cnode *item);
void co_end_compilation();

// parser
int coparse();
void coerror(const char *err, ...);

// scanner
int colex(struct cnode *colval);
int co_scanner_lex(struct cnode *yylval);
void co_scanner_startup(void);
void co_scanner_shutdown(void);
int co_scanner_setfile(COObject *f);
int co_scanner_setcode(char *code);

#endif
