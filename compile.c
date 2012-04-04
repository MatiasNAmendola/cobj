#include "co.h"

#define DEFAULT_INSTR_SIZE      64
#define DEFAULT_BYTECODE_SIZE   64

#ifdef CO_DEBUG
static void dump_code(COObject *code);
#endif

COObject *compile_ast(struct compiler *c);
static void compile_visit_node(struct compiler *c, Node *n);
static COObject *assemble(struct compiler *c);

static void
compiler_unit_free(struct compiler_unit *u)
{
    // TODO
}

static void
compiler_enter_scope(struct compiler *c)
{
    struct compiler_unit *u;
    u = (struct compiler_unit *)COMem_MALLOC(sizeof(struct compiler_unit));
    if (!u) {
        return;
    }
    memset(u, 0, sizeof(struct compiler_unit));

    u->consts = CODict_New();
    u->names = CODict_New();
    u->bytecode = COBytes_FromStringN(NULL, DEFAULT_BYTECODE_SIZE);
    u->bytecode_offset = 0;
    u->argcount = 0;

    /* Push the old compiler_unit on the stack. */
    if (c->u) {
        COObject *capsule = COCapsule_New(c->u);
        if (COList_Append(c->stack, capsule) < 0) {
            compiler_unit_free(u);
            return;
        }
    }
    c->u = u;
    c->nestlevel++;
}

static void
compiler_exit_scope(struct compiler *c)
{
    int n;
    c->nestlevel--;
    compiler_unit_free(c->u);

    /* Restore c->U to the parent unit. */
    n = COList_Size(c->stack) - 1;
    if (n >= 0) {
        COCapsuleObject *capsule =
            (COCapsuleObject *)COList_GetItem(c->stack, n);
        c->u = (struct compiler_unit *)capsule->pointer;
        COList_DelItem(c->stack, n);
    } else {
        c->u = NULL;
    }
}

COObject *
compile(void)
{
    // init
    struct compiler c;
    memset(&c, 0, sizeof(struct compiler));
    c.stack = COList_New(0);

    // do parse
    coparse(&c);

    return compile_ast(&c);
}

/*
 * Get next instruction.
 * Resizes instruction array as necessary.
 */
static int
compile_next_instr(struct compiler *c)
{
    if (c->u->instr == NULL) {
        c->u->instr =
            (struct instr *)COMem_MALLOC(sizeof(struct instr) *
                                         DEFAULT_INSTR_SIZE);
        if (!c->u->instr) {
            // TODO errors
            return -1;
        }
        c->u->ialloc = DEFAULT_INSTR_SIZE;
        c->u->iused = 0;
        memset(c->u->instr, 0, sizeof(struct instr) * DEFAULT_INSTR_SIZE);
    } else if (c->u->iused == c->u->ialloc) {
        size_t oldsize, newsize;
        oldsize = c->u->ialloc * sizeof(struct instr);
        newsize = oldsize << 1;
        if (oldsize > (SIZE_MAX >> 1) || newsize == 0) {
            // TODO errors
            return -1;
        }
        c->u->ialloc <<= 1;
        c->u->instr = (struct instr *)COMem_REALLOC(c->u->instr, newsize);
        if (!c->u->instr) {
            // TODO errors
            return -1;
        }
        memset(c->u->instr + oldsize, 0, newsize - oldsize);
    }
    return c->u->iused++;
}

/*
 * Add an opcode with no argument.
 */
static int
compile_addop(struct compiler *c, int opcode)
{
    struct instr *i;
    int off;
    off = compile_next_instr(c);
    if (off < 0)
        return -1;
    i = &c->u->instr[off];
    i->i_opcode = opcode;
    i->i_hasarg = 0;
    return 0;
}

/*
 * Add an opcode with an integer argument.
 */
static int
compile_addop_i(struct compiler *c, int opcode, int oparg)
{
    struct instr *i;
    int off;
    off = compile_next_instr(c);
    if (off < 0)
        return -1;
    i = &c->u->instr[off];
    i->i_opcode = opcode;
    i->i_oparg = oparg;
    i->i_hasarg = 1;
    return off;
}

/*
 * Add const/name and return index.
 */
static int
compile_add(COObject *dict, COObject *o)
{
    int arg;
    COObject *v;
    v = CODict_GetItem(dict, o);
    if (!v) {
        arg = CODict_Size(dict);
        v = COInt_FromLong(arg);
        if (CODict_SetItem(dict, o, v) < 0) {
            return -1;
        }
    } else {
        arg = COInt_AsLong(v);
    }
    return arg;
}

/*
 * Backpatch instruction.
 */
static void
compile_backpatch(struct compiler *c, int offset)
{
    c->u->instr[offset].i_oparg = c->u->iused - offset;
}

static void
compile_visit_nodelist(struct compiler *c, NodeList *l)
{
    if (!l)
        return;

    for (; l; l = l->next) {
        compile_visit_node(c, l->n);
    }
}

static void
compile_visit_node(struct compiler *c, Node *n)
{
    int oparg;
    if (!n)
        return;

    switch (n->type) {
    case NODE_PRINT:
        compile_visit_node(c, n->left);
        compile_addop(c, OP_PRINT);
        break;
    case NODE_RETURN:
        compile_visit_node(c, n->left);
        compile_addop(c, OP_RETURN);
        break;
    case NODE_CONST:
        oparg = compile_add(c->u->consts, n->o);
        compile_addop_i(c, OP_LOAD_CONST, oparg);
        break;
    case NODE_BIN:
        compile_visit_node(c, n->left);
        compile_visit_node(c, n->right);
        compile_addop(c, n->op);
        break;
    case NODE_LIST_BUILD:
        compile_addop(c, OP_LIST_BUILD);
        compile_visit_nodelist(c, n->list);
        break;
    case NODE_LIST_ADD:
        compile_visit_node(c, n->left);
        compile_addop(c, OP_LIST_ADD);
        break;
    case NODE_DICT_BUILD:
        compile_addop(c, OP_DICT_BUILD);
        compile_visit_nodelist(c, n->list);
        break;
    case NODE_DICT_ADD:
        compile_visit_node(c, n->left);
        compile_visit_node(c, n->right);
        compile_addop(c, OP_DICT_ADD);
        break;
    case NODE_NAME:
        oparg = compile_add(c->u->names, n->o);
        compile_addop_i(c, OP_LOAD_NAME, oparg);
        break;
    case NODE_ASSIGN:
        compile_visit_node(c, n->right);
        oparg = compile_add(c->u->names, n->left->o);
        compile_addop_i(c, OP_ASSIGN, oparg);
        break;
    case NODE_IF:
        compile_visit_node(c, n->ntest);
        int offset = compile_addop_i(c, OP_JMPZ, -1);
        compile_visit_nodelist(c, n->nbody);
        int offset_else = compile_addop_i(c, OP_JMP, -1);
        compile_backpatch(c, offset);
        compile_visit_nodelist(c, n->nelse);
        compile_backpatch(c, offset_else);
        break;
    case NODE_WHILE:
        {
            int while_start = c->u->iused;
            compile_visit_node(c, n->ntest);
            int offset = compile_addop_i(c, OP_JMPZ, -1);
            compile_visit_nodelist(c, n->nbody);
            compile_addop_i(c, OP_JMPX, while_start);
            compile_backpatch(c, offset);
        }
        break;
    case NODE_FUNC:
        compiler_enter_scope(c);
        c->u->argcount = nodelist_len(n->nfuncargs);
        NodeList *l = n->nfuncargs;
        while (l) {
            oparg = compile_add(c->u->names, l->n->o);
            compile_addop_i(c, OP_ASSIGN, oparg);
            l = l->next;
        }
        compile_visit_nodelist(c, n->nfuncbody);
        COObject *co = assemble(c);
#ifdef CO_DEBUG
        printf("begin function\n");
        dump_code(co);
        printf("end function\n");
#endif
        compiler_exit_scope(c);
        oparg = compile_add(c->u->consts, co);
        compile_addop_i(c, OP_LOAD_CONST, oparg);

        compile_addop(c, OP_DECLARE_FUNCTION);

        oparg = compile_add(c->u->names, n->nfuncname->o);
        compile_addop_i(c, OP_ASSIGN, oparg);
        break;
    case NODE_FUNC_CALL:
        compile_visit_nodelist(c, n->list);
        compile_visit_node(c, n->left);
        oparg = nodelist_len(n->list);
        compile_addop_i(c, OP_DO_FCALL, oparg);
        break;
    default:
        error("unknown node type: %d, %s", n->type, node_type(n->type));
    }
}

static int
instrsize(struct instr *i)
{
    if (i->i_hasarg) {
        return 3;               /* 1 (opcode) + 2 (oparg) */
    } else {
        return 1;               /* 1 (opcode) */
    }
}

/*
 * Compute jump offset before emit bytecode.
 */
static void
assemble_jump_offsets(struct compiler *c)
{
    int i;
    int j;
    for (i = 0; i < c->u->iused; i++) {
        struct instr *instr = c->u->instr + i;
        int offset = 0;
        if (instr->i_opcode == OP_JMPZ
            || instr->i_opcode == OP_JMP
            || instr->i_opcode == OP_DECLARE_FUNCTION) {
            // relatively
            for (j = 1; j < instr->i_oparg; j++) {
                struct instr *subinstr = instr + j;
                offset += instrsize(subinstr);
            }
            instr->i_oparg = offset;
        } else if (instr->i_opcode == OP_JMPX) {
            // absolutely
            for (j = 0; j < instr->i_oparg; j++) {
                struct instr *subinstr = c->u->instr + j;
                offset += instrsize(subinstr);
            }
            instr->i_oparg = offset;
        }
    }
}

/*
 * Assemble instruction into bytecode.
 */
static int
assemble_emit(struct compiler *c, struct instr *i)
{
    size_t len = COBytes_Size(c->u->bytecode);
    int size;

    size = instrsize(i);

    // resize if necessary 
    if (c->u->bytecode_offset + size >= len) {
        if (COBytes_Resize(c->u->bytecode, len * 2) < 0)
            return -1;
    }

    char *code = COBytes_AsString(c->u->bytecode) + c->u->bytecode_offset;
    c->u->bytecode_offset += size;
    *code++ = i->i_opcode;
    if (i->i_hasarg) {
        *code++ = i->i_oparg & 0xff;
        *code++ = i->i_oparg >> 8;
    }

    return 0;
}

/*
 * Assemble instructions into code object.
 */
static COObject *
assemble(struct compiler *c)
{
    assemble_jump_offsets(c);

    int i;
    for (i = 0; i < c->u->iused; i++) {
        assemble_emit(c, c->u->instr + i);
    }

    COBytes_Resize(c->u->bytecode, c->u->bytecode_offset);

    // Dict to List
    COObject *consts = COList_New(0);
    COObject *names = COList_New(0);
    COObject *key;
    COObject *val;
    // consts
    CODict_Rewind(c->u->consts);
    while (CODict_Next(c->u->consts, &key, &val) == 0) {
        COList_Insert(consts, COInt_AsLong(val), key);
    }
    c->u->consts = consts;
    // names
    CODict_Rewind(c->u->names);
    while (CODict_Next(c->u->names, &key, &val) == 0) {
        COList_Insert(names, COInt_AsLong(val), key);
    }
    c->u->names = names;

    return COCode_New(c->u->bytecode, COList_AsTuple(c->u->consts),
                      COList_AsTuple(c->u->names), c->u->argcount);
}

#ifdef CO_DEBUG
static char *
opcode_name(unsigned char opcode)
{
#define GIVE_NAME(type) \
    case (type):        \
        return #type

    switch (opcode) {
        GIVE_NAME(OP_NOP);
        GIVE_NAME(OP_ADD);
        GIVE_NAME(OP_SUB);
        GIVE_NAME(OP_MUL);
        GIVE_NAME(OP_DIV);
        GIVE_NAME(OP_MOD);
        GIVE_NAME(OP_POW);
        GIVE_NAME(OP_SL);
        GIVE_NAME(OP_SR);
        GIVE_NAME(OP_IS_SMALLER);
        GIVE_NAME(OP_IS_SMALLER_OR_EQUAL);
        GIVE_NAME(OP_IS_EQUAL);
        GIVE_NAME(OP_IS_NOT_EQUAL);
        GIVE_NAME(OP_ASSIGN);
        GIVE_NAME(OP_PRINT);
        GIVE_NAME(OP_JMPZ);
        GIVE_NAME(OP_JMP);
        GIVE_NAME(OP_JMPX);
        GIVE_NAME(OP_DECLARE_FUNCTION);
        GIVE_NAME(OP_RETURN);
        GIVE_NAME(OP_DO_FCALL);
        GIVE_NAME(OP_RECV_PARAM);
        GIVE_NAME(OP_PASS_PARAM);
        GIVE_NAME(OP_TRY);
        GIVE_NAME(OP_THROW);
        GIVE_NAME(OP_CATCH);
        GIVE_NAME(OP_LOAD_NAME);
        GIVE_NAME(OP_LOAD_CONST);
        GIVE_NAME(OP_TUPLE_BUILD);
        GIVE_NAME(OP_LIST_BUILD);
        GIVE_NAME(OP_LIST_ADD);
        GIVE_NAME(OP_POP_TOP);
        GIVE_NAME(OP_DICT_BUILD);
        GIVE_NAME(OP_DICT_ADD);
    }
    error("unknow opcode: %d\n", opcode);
    return NULL;
}

static void
dump_code(COObject *code)
{
#define NEXTOP()    (*bytecode++)
#define NEXTARG()   (bytecode += 2, (bytecode[-1]<<8) + bytecode[-2])
    char *bytecode = COBytes_AsString(((COCodeObject *)code)->co_code);
    char *start = bytecode;
    unsigned char opcode;
    for (;;) {
        opcode = NEXTOP();
        printf("%ld.\t%s", bytecode - start - 1, opcode_name(opcode));
        switch (opcode) {
        case OP_RETURN:
            /* exit bytecode */
            printf("\n");
            return;
            break;
            /* op with arg */
        case OP_ASSIGN:
        case OP_LOAD_CONST:
        case OP_LOAD_NAME:
        case OP_JMP:
        case OP_JMPX:
        case OP_JMPZ:
        case OP_DO_FCALL:
            printf("\t\t%d", NEXTARG());
            break;
        }
        printf("\n");
    }
}
#endif

/*
 * Compiles an node list (ast) into code object.
 */
COObject *
compile_ast(struct compiler *c)
{
    compiler_enter_scope(c);
    compile_visit_nodelist(c, c->xtop);
    COObject *co = assemble(c);
    compiler_exit_scope(c);

#ifdef CO_DEBUG
    dump_code(co);
#endif
    return co;
}

int
colex(YYSTYPE * colval)
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
