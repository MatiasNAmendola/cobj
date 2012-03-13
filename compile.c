#include "compile.h"
#include "vm_opcodes.h"
#include "parser.h"
#include "vm_execute.h"
#include "debug.h"

struct co_compiler_globals compiler_globals;

void
init_compiler()
{
    CG(active_opline_array) = xmalloc(sizeof(struct co_opline_array));
    init_opline_array(CG(active_opline_array), 1);
}

void
init_opline_array(struct co_opline_array *opline_array, int ops_size)
{
    opline_array->size = ops_size;
    opline_array->last = 0;
    opline_array->t = 0;
    opline_array->ops =
        xmalloc((opline_array->size) * sizeof(struct co_opline));
}

struct co_opline *
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
    return (opline_array->t)++ * sizeof(union temp_variable);
}

void
co_binary_op(uchar opcode, struct cnode *result, const struct cnode *op1,
             const struct cnode *op2)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));

    op->opcode = opcode;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(CG(active_opline_array));
    op->op1 = *op1;
    op->op2 = *op2;
    *result = op->result;
}

void
co_assign(struct cnode *result, struct cnode *variable,
          const struct cnode *value)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));
    op->opcode = OP_BIND_NAME;
    op->op1 = *variable;

    op = get_next_op(CG(active_opline_array));
    op->opcode = OP_ASSIGN;
    op->op1 = *variable;
    op->op2 = *value;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(CG(active_opline_array));
    *result = op->result;
}

void
co_print(const struct cnode *arg)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));

    op->opcode = OP_PRINT;
    op->op1 = *arg;
    SET_UNUSED(op->op2);
}

void
co_return(const struct cnode *expr)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));

    op->opcode = OP_RETURN;
    op->op1 = *expr;
    SET_UNUSED(op->op2);
}

void
co_if_cond(const struct cnode *cond, struct cnode *closing_bracket_token)
{
    int if_cond_opline_num = CG(active_opline_array)->last;
    struct co_opline *opline = get_next_op(CG(active_opline_array));
    opline->opcode = OP_JMPZ;
    opline->op1 = *cond;
    closing_bracket_token->u.opline_num = if_cond_opline_num;
    SET_UNUSED(opline->op2);
}

void
co_if_after_statement(struct cnode *closing_bracket_token)
{
    int if_after_stmt_op_num = CG(active_opline_array)->last;
    struct co_opline *opline = get_next_op(CG(active_opline_array));
    CG(active_opline_array)->ops[closing_bracket_token->u.opline_num].op2.
        u.opline_num =
        if_after_stmt_op_num + 1 - closing_bracket_token->u.opline_num;
    closing_bracket_token->u.opline_num = if_after_stmt_op_num;
    opline->opcode = OP_JMP;
    SET_UNUSED(opline->op1);
    SET_UNUSED(opline->op2);
}

void
co_if_end(const struct cnode *closing_bracket_token)
{
    int if_end_op_num = CG(active_opline_array)->last;
    CG(active_opline_array)->ops[closing_bracket_token->u.opline_num].op1.
        u.opline_num = if_end_op_num - closing_bracket_token->u.opline_num;
}

void
co_while_cond(const struct cnode *cond, struct cnode *while_token,
              struct cnode *closing_bracket_token)
{
    int while_cond_opline_num = CG(active_opline_array)->last;
    struct co_opline *opline = get_next_op(CG(active_opline_array));
    opline->opcode = OP_JMPZ;
    opline->op1 = *cond;
    while_token->u.opline_num = while_cond_opline_num - 1;
    closing_bracket_token->u.opline_num = while_cond_opline_num;
}

void
co_while_end(const struct cnode *while_token,
             const struct cnode *closing_bracket_token)
{
    // add unconditional jumpback
    int while_end_opline_num = CG(active_opline_array)->last;
    struct co_opline *op = get_next_op(CG(active_opline_array));
    op->opcode = OP_JMP;
    op->op1.u.opline_num = while_token->u.opline_num - while_end_opline_num;
    SET_UNUSED(op->op1);
    SET_UNUSED(op->op2);

    int while_end_stmt_op_num = CG(active_opline_array)->last;
    CG(active_opline_array)->ops[closing_bracket_token->u.opline_num].op2.
        u.opline_num =
        while_end_stmt_op_num - closing_bracket_token->u.opline_num;
}

void
co_begin_function_declaration(struct cnode *function_token,
                              struct cnode *function_name)
{
    struct co_opline *op;
    if (function_name) {
        op = get_next_op(CG(active_opline_array));
        op->opcode = OP_BIND_NAME;
        op->op1 = *function_name;
    }

    int function_opline_num = CG(active_opline_array)->last;
    op = get_next_op(CG(active_opline_array));
    op->opcode = OP_DECLARE_FUNCTION;
    if (function_name) {
        op->op1 = *function_name;
    }
    function_token->u.opline_num = function_opline_num;
}

void
co_end_function_declaration(const struct cnode *function_token,
                            struct cnode *result)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));
    op->opcode = OP_RETURN;

    int function_end_opline_num = CG(active_opline_array)->last;
    CG(active_opline_array)->ops[function_token->u.opline_num].op2.
        u.opline_num =
        function_end_opline_num - function_token->u.opline_num - 1;

    if (result) {
        CG(active_opline_array)->ops[function_token->u.opline_num].result.type =
            IS_TMP_VAR;
        CG(active_opline_array)->ops[function_token->u.opline_num].result.
            u.var = get_temporary_variable(CG(active_opline_array));
        *result =
            CG(active_opline_array)->ops[function_token->u.opline_num].result;
    }
}

void
co_begin_function_call(struct cnode *function_name)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));
    op->opcode = OP_INIT_FCALL;
    op->op1 = *function_name;
}

void
co_end_function_call(struct cnode *function_name, struct cnode *result)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));
    op->opcode = OP_DO_FCALL;
    op->op1 = *function_name;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(CG(active_opline_array));
    *result = op->result;
}

void
co_recv_param(struct cnode *param)
{
    struct co_opline *op;
    op = get_next_op(CG(active_opline_array));
    op->opcode = OP_BIND_NAME;
    op->op1 = *param;

    op = get_next_op(CG(active_opline_array));
    op->opcode = OP_RECV_PARAM;
    op->op1 = *param;
}

void
co_pass_param(struct cnode *param)
{
    struct co_opline *op = get_next_op(CG(active_opline_array));
    op->opcode = OP_PASS_PARAM, op->op1 = *param;
}

void
co_end_compilation()
{
    struct co_opline *op = get_next_op(CG(active_opline_array));

    op->opcode = OP_EXIT;

#ifdef CO_DEBUG
    co_print_opcode(CG(active_opline_array));
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
