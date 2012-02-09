#include "co_compile.h"
#include "co_globals.h"
#include "co_vm_opcodes.h"
#include "co_parser.h"
#include "co_vm_execute.h"

co_compiler_globals compiler_globals;
co_executor_globals executor_globals;

cval *
getcval(const char *name)
{
    cval *val;

    if (co_symtable_find(&CG(variable_symboltable), name, strlen(name), (void **)&val)) {
        return val;
    } else {
        return NULL;
    }
}

bool
putcval(const char *name, cval *val)
{
    return co_symtable_update(&CG(variable_symboltable), name, strlen(name), val, sizeof(cval));
}

bool
delcval(const char *name)
{
    return co_symtable_del(&CG(variable_symboltable), name, strlen(name));
}

void
init_compiler()
{
    CG(active_op_array) = xmalloc(sizeof(co_op_array));
    co_stack_init(&CG(function_call_stack));
    co_hash_init(&CG(function_symboltable), 2, NULL);
    co_hash_init(&CG(variable_symboltable), 2, NULL);
    init_op_array(CG(active_op_array), 1);
}

void
init_op_array(co_op_array *op_array, int ops_size)
{
    op_array->size = ops_size;
    op_array->last = 0;
    op_array->ops = xrealloc(op_array->ops, (op_array->size) * sizeof(co_op));
    op_array->t = 0;
    op_array->start_op = NULL;
}

static inline void
init_op(co_op *op)
{
    memset(op, 0, sizeof(co_op));
    op->line = CG(line);
    op->result.op_type = IS_UNUSED;
}

co_op *
get_next_op(co_op_array *op_array)
{
    uint next_op_num = op_array->last++;

    co_op *next_op;

    if (next_op_num >= op_array->size) {
        op_array->size <<= 1;
        op_array->ops = xrealloc(op_array->ops, (op_array->size) * sizeof(co_op));
    }

    next_op = &(op_array->ops[next_op_num]);

    init_op(next_op);

    return next_op;
}

static uint
get_temporary_variable(co_op_array *op_array)
{
    return (op_array->t)++ * sizeof(temp_variable);
}

void
co_binary_op(uchar opcode, cnode *result, const cnode *op1, const cnode *op2)
{
    co_op *op = get_next_op(CG(active_op_array));

    op->opcode = opcode;
    op->result.op_type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(CG(active_op_array));
    op->op1 = *op1;
    op->op2 = *op2;
    *result = op->result;
}

void
co_assign(cnode *result, cnode *variable, const cnode *value)
{
    co_op *op = get_next_op(CG(active_op_array));

    op->opcode = OP_ASSIGN;
    op->op1 = *variable;
    op->op2 = *value;
    op->result.op_type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable(CG(active_op_array));
    *result = op->result;
}

void
co_print(const cnode *arg)
{
    co_op *op = get_next_op(CG(active_op_array));

    op->opcode = OP_PRINT;
    op->op1 = *arg;
    SET_UNUSED(op->op2);
}

void
co_if_cond(const cnode *cond, cnode *closing_bracket_token)
{
    uint if_cond_opline_num = CG(active_op_array)->last;
    co_op *opline = get_next_op(CG(active_op_array));
    opline->opcode = OP_JMPZ;
    opline->op1 = *cond;
    closing_bracket_token->u.opline_num = if_cond_opline_num;
    SET_UNUSED(opline->op2);
}

void
co_if_after_statement(cnode *closing_bracket_token)
{
    uint if_after_stmt_op_num = CG(active_op_array)->last;
    co_op *opline = get_next_op(CG(active_op_array));
    CG(active_op_array)->ops[closing_bracket_token->u.opline_num].op2.u.opline_num =
        if_after_stmt_op_num + 1;
    closing_bracket_token->u.opline_num = if_after_stmt_op_num;
    opline->opcode = OP_JMP;
    SET_UNUSED(opline->op1);
    SET_UNUSED(opline->op2);
}

void
co_if_end(const cnode *closing_bracket_token)
{
    uint if_end_op_num = CG(active_op_array)->last;
    CG(active_op_array)->ops[closing_bracket_token->u.opline_num].op1.u.opline_num = if_end_op_num;
}

void
co_while_cond(const cnode *cond, cnode *while_token, cnode *closing_bracket_token)
{
    uint while_cond_opline_num = CG(active_op_array)->last;
    co_op *opline = get_next_op(CG(active_op_array));
    opline->opcode = OP_JMPZ;
    opline->op1 = *cond;
    while_token->u.opline_num = while_cond_opline_num - 1;
    closing_bracket_token->u.opline_num = while_cond_opline_num;
    SET_UNUSED(opline->op2);
}

void
co_while_end(const cnode *while_token, const cnode *closing_bracket_token)
{
    // add unconditional jumpback
    co_op *op = get_next_op(CG(active_op_array));
    op->opcode = OP_JMP;
    op->op1.u.opline_num = while_token->u.opline_num;
    SET_UNUSED(op->op1);
    SET_UNUSED(op->op2);

    uint while_end_stmt_op_num = CG(active_op_array)->last;
    CG(active_op_array)->ops[closing_bracket_token->u.opline_num].op2.u.opline_num = while_end_stmt_op_num;
}

void
co_begin_function_declaration(const cnode *function_token, const cnode *function_name, cnode *closing_bracket_token)
{
    uint function_opline_num = CG(active_op_array)->last;
    co_op *op = get_next_op(CG(active_op_array));
    op->opcode = OP_DECLARE_FUNCTION;
    op->op1.u.opline_num = function_opline_num + 1;
    closing_bracket_token->u.opline_num = function_opline_num;
}

void
co_end_function_declaration(const cnode *closing_bracket_token)
{
    CG(active_op_array)->ops[closing_bracket_token->u.opline_num].op2.u.opline_num = CG(active_op_array)->last;
}

/*void*/
/*co_function_call*/
/*{*/
/*}*/

void
co_end_compilation()
{
    co_op *op = get_next_op(CG(active_op_array));

    op->opcode = OP_EXIT;

#ifdef CO_DEBUG
    for (int i = 0; i < CG(active_op_array)->last; i++) {
        printf("%d\n", CG(active_op_array)->ops[i].opcode);
    }
#endif

    SET_UNUSED(op->op1);
    SET_UNUSED(op->op2);
}
