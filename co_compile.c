#include "co_compile.h"
#include "co_globals.h"
#include "co_globals_macros.h"
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
    init_op_array(CG(active_op_array), IS_STRING, 1);
}

void
init_op_array(co_op_array *op_array, uchar type, int ops_size)
{
    op_array->type = type;
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

uint
get_next_op_num(co_op_array *op_array)
{
    return op_array->last;
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

    if (opcode == CO_ADD) {
        op->handler = co_do_add;
    } else if (opcode == CO_SUB) {
        op->handler = co_do_sub;
    }
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

    op->handler = co_do_assign;
    op->opcode = CO_ASSIGN;
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

    op->handler = co_do_print;
    op->opcode = CO_PRINT;
    op->op1 = *arg;
    SET_UNUSED(op->op2);
}

void
co_if_cond(const cnode *cond, cnode *closing_bracket_token)
{
    uint if_cond_opline_num = get_next_op_num(CG(active_op_array));
    co_op *opline = get_next_op(CG(active_op_array));
    opline->handler = co_do_if_cond;
    opline->opcode = CO_JMPZ;
    opline->op1 = *cond;
    closing_bracket_token->u.opline_num = if_cond_opline_num;
    SET_UNUSED(opline->op2);
}

void
co_if_after_statement(cnode *closing_bracket_token)
{
    uint if_after_stmt_op_num = get_next_op_num(CG(active_op_array));
    co_op *opline = get_next_op(CG(active_op_array));
    CG(active_op_array)->ops[closing_bracket_token->u.opline_num].op2.u.opline_num =
        if_after_stmt_op_num + 1;
    closing_bracket_token->u.opline_num = if_after_stmt_op_num;
    opline->opcode = CO_JMP;
    opline->handler = co_do_if_after_statement;
    SET_UNUSED(opline->op1);
    SET_UNUSED(opline->op2);
}

void
co_if_end(const cnode *closing_bracket_token)
{
    uint if_end_op_num = get_next_op_num(CG(active_op_array));
    CG(active_op_array)->ops[closing_bracket_token->u.opline_num].op1.u.opline_num = if_end_op_num;
}

void
co_end_compilation()
{
    co_op *op = get_next_op(CG(active_op_array));

    op->handler = co_do_exit;
    op->opcode = CO_EXIT;
    SET_UNUSED(op->op1);
    SET_UNUSED(op->op2);
}
