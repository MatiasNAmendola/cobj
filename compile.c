#include "co.h"

struct compiler {
    struct co_opline_array *opline_array;
    COObject *c_consts; /* all constatns */
    COObject *c_names;  /* all names */
};

struct compiler c;

/**
 * COObject Bytecode format:
 *
 * bytecode: opcode*
 * opcode: OPCODE arg1 arg2 result
 *
 */
static COObject *
assemble(COCodeObject *co, struct co_opline_array *opline_array)
{

}

static void
co_compiler_add(struct co_opline *op)
{
    if (op->op1.type == IS_CONST) {
        COList_Append(c.c_consts, op->op1.u.co);
        op->op1.u.var = CO_SIZE(c.c_consts) - 1;
    } else if (op->op1.type == IS_VAR) {
        COList_Append(c.c_names, op->op1.u.co);
        op->op1.u.var = CO_SIZE(c.c_names) - 1;
    }

    if (op->op2.type == IS_CONST) {
        COList_Append(c.c_consts, op->op2.u.co);
        op->op2.u.var = CO_SIZE(c.c_consts) - 1;
    } else if (op->op2.type == IS_VAR) {
        COList_Append(c.c_names, op->op2.u.co);
        op->op2.u.var = CO_SIZE(c.c_names) - 1;
    }
}

COCodeObject *
co_compile(void)
{
    c.opline_array = xmalloc(sizeof(struct co_opline_array));
    c.opline_array->size = 1;
    c.opline_array->last = 0;
    c.opline_array->t = 0;
    c.opline_array->ops =
        xmalloc((c.opline_array->size) * sizeof(struct co_opline));
    c.c_consts = COList_New(0);
    c.c_names = COList_New(0);

    // do parse
    coparse(&c);

    for (int i = 0; i < c.opline_array->last; i++) {
        co_compiler_add(c.opline_array->ops + i);
    }

    COCodeObject *co = (COCodeObject *)COCode_New(NULL, COList_AsTuple(c.c_consts), COList_AsTuple(c.c_names));
    co->opline_array = c.opline_array;

    /*co->co_code = assemble(co, c.opline_array); */
    return co;
}

uint
co_get_next_opline_num(void)
{
    return c.opline_array->last;
}

static struct co_opline *
get_next_op(struct co_opline_array *opline_array)
{
    int next_op_num = opline_array->last++;

    struct co_opline *next_op;

    if (next_op_num >= opline_array->size) {
        opline_array->size <<= 1;
        opline_array->ops =
            xrealloc(opline_array->ops,
                     (opline_array->size) * sizeof(struct co_opline));
    }

    next_op = &(opline_array->ops[next_op_num]);

    memset(next_op, 0, sizeof(struct co_opline));
    SET_UNUSED(next_op->result);
    SET_UNUSED(next_op->op1);
    SET_UNUSED(next_op->op2);

    return next_op;
}

static uint
get_temporary_variable(struct co_opline_array *opline_array)
{
    return (opline_array->t)++ * sizeof(COObject *);
}

void
co_binary_op(uchar opcode, struct cnode *result, const struct cnode *op1,
             const struct cnode *op2)
{
    struct co_opline *op = get_next_op(c.opline_array);

    op->opcode = opcode;
    op->op1 = *op1;
    op->op2 = *op2;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(c.opline_array);
    *result = op->result;
}

void
co_assign(struct cnode *result, struct cnode *variable,
          const struct cnode *value)
{
    struct co_opline *op;
    op = get_next_op(c.opline_array);
    op->opcode = OP_LOAD_NAME;
    op->op1 = *variable;

    op = get_next_op(c.opline_array);
    op->opcode = OP_ASSIGN;
    op->op1 = *variable;
    op->op2 = *value;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(c.opline_array);
    *result = op->result;
}

void
co_print(const struct cnode *arg)
{
    struct co_opline *op = get_next_op(c.opline_array);

    op->opcode = OP_PRINT;
    op->op1 = *arg;
    SET_UNUSED(op->op2);
}

void
co_return(const struct cnode *expr)
{
    struct co_opline *op = get_next_op(c.opline_array);

    op->opcode = OP_RETURN;
    op->op1 = *expr;
    SET_UNUSED(op->op2);
}

void
co_if_cond(const struct cnode *cond, struct cnode *if_token)
{
    int if_cond_opline_num = c.opline_array->last;
    struct co_opline *opline = get_next_op(c.opline_array);
    opline->opcode = OP_JMPZ;
    opline->op1 = *cond;
    if_token->u.opline_num = if_cond_opline_num;
    SET_UNUSED(opline->op2);
}

void
co_if_after_stmt(struct cnode *if_token)
{
    int if_after_stmt_op_num = c.opline_array->last;
    struct co_opline *opline = get_next_op(c.opline_array);
    c.opline_array->ops[if_token->u.opline_num].op2.u.opline_num =
        if_after_stmt_op_num + 1 - if_token->u.opline_num;
    if_token->u.opline_num = if_after_stmt_op_num;
    opline->opcode = OP_JMP;
    SET_UNUSED(opline->op1);
    SET_UNUSED(opline->op2);
}

void
co_if_end(const struct cnode *if_token)
{
    int if_end_op_num = c.opline_array->last;
    c.opline_array->ops[if_token->u.opline_num].op1.u.opline_num =
        if_end_op_num - if_token->u.opline_num;
}

void
co_while_cond(const struct cnode *cond, struct cnode *while_token)
{
    int while_cond_opline_num = c.opline_array->last;
    struct co_opline *opline = get_next_op(c.opline_array);
    opline->opcode = OP_JMPZ;
    opline->op1 = *cond;
    opline->op2.u.opline_num = while_token->u.opline_num; // while start
    while_token->u.opline_num = while_cond_opline_num;
}

void
co_while_end(const struct cnode *while_token)
{
    // add unconditional jumpback
    int while_end_opline_num = c.opline_array->last;
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_JMP;
    op->op1.u.opline_num = c.opline_array->ops[while_token->u.opline_num].op2.u.opline_num - while_end_opline_num; // while start offset

    int while_end_stmt_op_num = c.opline_array->last;
    c.opline_array->ops[while_token->u.opline_num].op2.u.opline_num =
        while_end_stmt_op_num - while_token->u.opline_num;
}

void
co_begin_func_declaration(struct cnode *func_token, struct cnode *func_name)
{
    struct co_opline *op;
    if (func_name) {
        op = get_next_op(c.opline_array);
        op->opcode = OP_LOAD_NAME;
        op->op1 = *func_name;
    }

    int func_opline_num = c.opline_array->last;
    op = get_next_op(c.opline_array);
    op->opcode = OP_DECLARE_FUNCTION;
    if (func_name) {
        op->op1 = *func_name;
    }
    func_token->u.opline_num = func_opline_num;
}

void
co_end_func_declaration(const struct cnode *func_token, struct cnode *result)
{
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_RETURN;

    int func_end_opline_num = c.opline_array->last;
    c.opline_array->ops[func_token->u.opline_num].op2.u.opline_num =
        func_end_opline_num - func_token->u.opline_num - 1;

    if (result) {
        c.opline_array->ops[func_token->u.opline_num].result.type = IS_TMP_VAR;
        c.opline_array->ops[func_token->u.opline_num].result.u.var =
            get_temporary_variable(c.opline_array);
        *result = c.opline_array->ops[func_token->u.opline_num].result;
    }
}

void
co_end_func_call(struct cnode *func_name, struct cnode *result)
{
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_DO_FCALL;
    op->op1 = *func_name;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(c.opline_array);
    *result = op->result;
}

void
co_recv_param(struct cnode *param)
{
    struct co_opline *op;
    op = get_next_op(c.opline_array);
    op->opcode = OP_LOAD_NAME;
    op->op1 = *param;

    op = get_next_op(c.opline_array);
    op->opcode = OP_RECV_PARAM;
    op->op1 = *param;
}

void
co_pass_param(struct cnode *param)
{
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_PASS_PARAM, op->op1 = *param;
}

void
co_list_build(struct cnode *result, struct cnode *tag)
{
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_LIST_BUILD;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(c.opline_array);
    *result = op->result;

    *tag = op->result;
}

void
co_tuple_build(struct cnode *result, struct cnode *tag)
{
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_TUPLE_BUILD;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(c.opline_array);
    *result = op->result;

    *tag = op->result;
}

void
co_append_element(struct cnode *node, struct cnode *element)
{
    struct co_opline *op = get_next_op(c.opline_array);
    op->opcode = OP_APPEND_ELEMENT;
    op->op1 = *node;
    op->op2 = *element;
}

void
co_end_compilation()
{
    struct co_opline *op = get_next_op(c.opline_array);

    op->opcode = OP_EXIT;

#ifdef CO_DEBUG
    co_print_opcode(c.opline_array);
#endif

    SET_UNUSED(op->op1);
    SET_UNUSED(op->op2);
}

int
colex(struct cnode *colval)
{
    int retval;

again:
    retval = co_scanner_lex(colval);
    switch (retval) {
    case T_WHITESPACE:
    case T_COMMENT:
    case T_IGNORED:
        goto again;
    default:
        break;
    }

    return retval;
}

void
coerror(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    error(err, params);
    va_end(params);
    exit(128);
}
