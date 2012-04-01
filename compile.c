#include "co.h"

struct compiler c;

COObject *compile_ast(NodeList *l);

COObject *
co_compile(void)
{
    // init
    c.consts = COList_New(0);
    c.names = COList_New(0);
    c.bytecode = COBytes_FromStringN(NULL, 0);
    c.c_instr = NULL;

    // do parse
    coparse(&c);

#ifdef CO_DEBUG
    node_listtree(c.xtop);
    exit(0);
#endif

    return compile_ast(c.xtop);
}

/*
 * Get next instruction.
 * Resizes instruction array as necessary.
 */
#define DEFAULT_INSTR_SIZE 64
static int
compile_next_instr(void)
{
    if (c.c_instr == NULL) {
        c.c_instr = (struct instr *)COMem_MALLOC(sizeof(struct instr) * DEFAULT_INSTR_SIZE);
        if (!c.c_instr) {
            // TODO errors
            return -1;
        }
        c.c_ialloc = DEFAULT_INSTR_SIZE;
        c.c_iused = 0;
        memset(c.c_instr, 0, sizeof(struct instr) * DEFAULT_INSTR_SIZE);
    } else if (c.c_iused == c.c_ialloc) {
        size_t oldsize, newsize;
        oldsize = c.c_ialloc * sizeof(struct instr);
        newsize = oldsize << 1;
        if (oldsize > (SIZE_MAX >> 1) || newsize == 0) {
            // TODO errors
            return -1;
        }
        c.c_ialloc <<= 1;
        c.c_instr = (struct instr *)COMem_REALLOC(c.c_instr, newsize);
        if (!c.c_instr) {
            // TODO errors
            return -1;
        }
        memset(c.c_instr + oldsize, 0, newsize - oldsize);
    }
    return c.c_iused++;
}

static int
compile_addop_i(int opcode, int oparg)
{
    struct instr *i;
    int off;
    off = compile_next_instr();
    if (off < 0)
        return -1;
    i = &c.c_instr[off];
    i->i_opcode = opcode;
    i->i_oparg = oparg;
    i->i_hasarg = 1;
    return 0;
}

static int
compile_visit_const(Node *n)
{
    if (n->type != NODE_CONST) {
        error("fatal");
    }

    int arg = COList_Append(c.consts, n->o);
    return compile_addop_i(OP_PRINT, arg);
}

/*
 * Assemble instruction into bytecode.
 */
static int
assemble_emit(struct instr *i)
{
    return 0;
}

static int
assemble(void)
{
    int i;
    for (i = 0; i < c.c_iused; i++) {
        assemble_emit(c.c_instr + i);
    }
    return 0;
}

/*
 * Compiles an node list (ast) into code object.
 */
COObject *
compile_ast(NodeList *l)
{
    Node *n;
    for (; l; l = l->next) {
        n = l->n;
        if (n->type == NODE_PRINT) {
            if (n->left->type == NODE_CONST) {
                compile_visit_const(n->left);
            }
        }
    }
    assemble();
    return COCode_New(c.bytecode, COList_AsTuple(c.consts), COList_AsTuple(c.names));
}


/*
uint
co_get_next_opline_num(void)
{
    return COList_Size(c.c_oplines);
}

static COOplineObject *
next_op()
{
    COOplineObject *next_op = (COOplineObject *)COOpline_New();
    next_op->arg1.type = IS_UNUSED;
    next_op->arg2.type = IS_UNUSED;
    next_op->result.type = IS_UNUSED;

    COList_Append(c.c_oplines, (COObject *)next_op);

    return next_op;
}

static uint
get_temporary_variable()
{
    return c.c_numoftmpvars++ * sizeof(COObject *);
}

void
co_binary_op(uchar opcode, struct cnode *result, const struct cnode *arg1,
             const struct cnode *arg2)
{
    COOplineObject *op = next_op();

    op->opcode = opcode;
    op->arg1 = *arg1;
    op->arg2 = *arg2;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable();
    *result = op->result;
}

void
co_assign(struct cnode *result, struct cnode *variable,
          const struct cnode *value)
{
    COOplineObject *op;

    op = next_op();
    op->opcode = OP_ASSIGN;
    op->arg1 = *variable;
    op->arg2 = *value;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable();
    *result = op->result;
}

void
co_print(const struct cnode *arg)
{
    COOplineObject *op = next_op();

    op->opcode = OP_PRINT;
    op->arg1 = *arg;
}

void
co_return(const struct cnode *expr)
{
    COOplineObject *op = next_op();

    op->opcode = OP_RETURN;
    op->arg1 = *expr;
}

void
co_if_cond(const struct cnode *cond, struct cnode *if_token)
{
    int if_cond_opline_num = CO_SIZE(c.c_oplines);
    COOplineObject *opline = next_op();
    opline->opcode = OP_JMPZ;
    opline->arg1 = *cond;
    if_token->u.opline_num = if_cond_opline_num;
}

void
co_if_after_stmt(struct cnode *if_token)
{
    int if_after_stmt_op_num = CO_SIZE(c.c_oplines);
    COOplineObject *opline = next_op();
    COOplineObject *ifopline =
        (COOplineObject *)COList_GetItem(c.c_oplines, if_token->u.opline_num);
    ifopline->arg2.u.opline_num =
        if_after_stmt_op_num + 1 - if_token->u.opline_num;
    if_token->u.opline_num = if_after_stmt_op_num;
    opline->opcode = OP_JMP;
}

void
co_if_end(const struct cnode *if_token)
{
    int if_end_op_num = CO_SIZE(c.c_oplines);
    COOplineObject *ifopline =
        (COOplineObject *)COList_GetItem(c.c_oplines, if_token->u.opline_num);
    ifopline->arg1.u.opline_num = if_end_op_num - if_token->u.opline_num;
}

void
co_while_cond(const struct cnode *cond, struct cnode *while_token)
{
    int while_cond_opline_num = CO_SIZE(c.c_oplines);
    COOplineObject *opline = next_op();
    opline->opcode = OP_JMPZ;
    opline->arg1 = *cond;
    opline->arg2.u.opline_num = while_token->u.opline_num;      // while start
    while_token->u.opline_num = while_cond_opline_num;
}

void
co_while_end(const struct cnode *while_token)
{
    // add unconditional jumpback
    int while_end_opline_num = CO_SIZE(c.c_oplines);
    COOplineObject *op = next_op();
    op->opcode = OP_JMP;
    COOplineObject *whileopline = (COOplineObject *)COList_GetItem(c.c_oplines,
                                                                   while_token->u.opline_num);
    op->arg1.u.opline_num = whileopline->arg2.u.opline_num - while_end_opline_num;      // while start offset

    int while_end_stmt_op_num = CO_SIZE(c.c_oplines);
    whileopline->arg2.u.opline_num =
        while_end_stmt_op_num - while_token->u.opline_num;
}

void
co_begin_func_declaration(struct cnode *func_token, struct cnode *func_name)
{
    COOplineObject *op;
    int func_opline_num = CO_SIZE(c.c_oplines);
    op = next_op();
    op->opcode = OP_DECLARE_FUNCTION;
    if (func_name) {
        op->arg1 = *func_name;
    }
    func_token->u.opline_num = func_opline_num;
}

void
co_end_func_declaration(const struct cnode *func_token, struct cnode *result)
{
    COOplineObject *op = next_op();
    op->opcode = OP_RETURN;

    int func_end_opline_num = CO_SIZE(c.c_oplines);
    COOplineObject *funcopline =
        (COOplineObject *)COList_GetItem(c.c_oplines, func_token->u.opline_num);
    funcopline->arg2.u.opline_num =
        func_end_opline_num - func_token->u.opline_num - 1;

    if (result) {
        funcopline->result.type = IS_TMP_VAR;
        funcopline->result.u.var = get_temporary_variable();
        *result = funcopline->result;
    }
}

void
co_end_func_call(struct cnode *func_name, struct cnode *result)
{
    COOplineObject *op = next_op();
    op->opcode = OP_DO_FCALL;
    op->arg1 = *func_name;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable();
    *result = op->result;
}

void
co_recv_param(struct cnode *param)
{
    COOplineObject *op;
    op = next_op();
    op->opcode = OP_RECV_PARAM;
    op->arg1 = *param;
}

void
co_pass_param(struct cnode *param)
{
    COOplineObject *op = next_op();
    op->opcode = OP_PASS_PARAM, op->arg1 = *param;
}

void
co_list_build(struct cnode *result, struct cnode *tag)
{
    COOplineObject *op = next_op();
    op->opcode = OP_LIST_BUILD;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable();
    *result = op->result;

    *tag = op->result;
}

void
co_list_add(struct cnode *node, struct cnode *element)
{
    COOplineObject *op = next_op();
    op->opcode = OP_LIST_ADD;
    op->arg1 = *node;
    op->arg2 = *element;
}

void
co_dict_build(struct cnode *result, struct cnode *tag)
{
    COOplineObject *op = next_op();
    op->opcode = OP_DICT_BUILD;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable();
    *result = op->result;

    *tag = op->result;
}

void
co_dict_add(struct cnode *node, struct cnode *key, struct cnode *item)
{
    COOplineObject *op = next_op();
    op->opcode = OP_DICT_ADD;
    op->arg1 = *node;
    op->arg2 = *key;;
    op->result = *item;
}

void
co_tuple_build(struct cnode *result, struct cnode *tag)
{
    COOplineObject *op = next_op();
    op->opcode = OP_TUPLE_BUILD;
    op->result.type = IS_TMP_VAR;
    op->result.u.var = get_temporary_variable();
    *result = op->result;

    *tag = op->result;
}

void
co_end_compilation()
{
    COOplineObject *op = next_op();

    op->opcode = OP_RETURN;
}
*/

int
colex(YYSTYPE *colval)
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
coerror(struct compiler *c, const char *err, ...)
{
    va_list params;

    va_start(params, err);
    error(err, params);
    va_end(params);
    exit(128);
}
