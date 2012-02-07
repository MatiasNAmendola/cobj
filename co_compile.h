#ifndef CO_COMPILE_H
#define CO_COMPILE_H

#include "co.h"
#include "co_hash.h"

/** cval type **/
#define CVAL_IS_NULL    0
#define CVAL_IS_INT     1
#define CVAL_IS_FLOAT   2
#define CVAL_IS_BOOL    3
#define CVAL_IS_STRING  4

/** cnode type **/
#define IS_CONST        (1<<0)
#define IS_TMP_VAR      (1<<1)
#define IS_VAR          (1<<2)
#define IS_UNUSED       (1<<3)  /* unused variable */
#define SET_UNUSED(op)   (op).op_type = IS_UNUSED

typedef struct _cval cval;
typedef struct _cnode cnode;
typedef struct _co_op co_op;
typedef struct _co_op_array co_op_array;
typedef struct _co_execute_data co_execute_data;
typedef union _temp_variable temp_variable;

/* c op handler */
typedef int (*op_handler_t) (co_execute_data *execute_data);

/* c value container */
struct _cval {
    union {
        long ival;              /* int value */
        double fval;            /* float value */
        struct {
            char *val;
            int len;
        } str;                  /* string value */
        HashTable *ht;          /* hash table value */
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
        co_op_array *op_array;
        co_op *jmp_addr;
    } u;
};

/* c op for opcode */
struct _co_op {
    uchar opcode;
    cnode result;
    cnode op1;
    cnode op2;
    uint line;
};

/* c op array */
struct _co_op_array {
    uchar type;
    co_op *ops;
    co_op *start_op;
    uint t;
    uint last, size;
};

/* temp variable */
union _temp_variable {
    cval tmp_var;
};

/* execute data */
struct _co_execute_data {
    co_op *op;
    HashTable *symboltable;
    temp_variable *ts;
    co_op_array *op_array;
    bool nested;
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
void co_end_compilation();

/* opcode */
co_op *get_next_op(co_op_array *op_array);

/* cval handlers */
extern cval *getcval(const char *name);
extern bool putcval(const char *name, cval *val);
extern bool delcval(const char *name);

#endif
