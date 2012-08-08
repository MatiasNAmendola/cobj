#include "co.h"

#define DEFAULT_INSTR_SIZE      64
#define DEFAULT_BYTECODE_SIZE   64

// instruction
struct instr {
    unsigned char i_opcode;
    unsigned int i_hasarg:1;
    unsigned int i_oparg;
    struct block *i_target;     /* JUMP Target */
};

// block
struct block {
    /* Each block in compiler unit is linked via b_listnext in the reverse
     * order. */
    struct block *b_listnext;
    struct instr *b_instr;      /* pointer to an array of instructions */
    int b_iused;                /* number of instructions used */
    int b_ialloc;               /* length of instruction array */
    /* If b_next is non-NULL, it's a pointer to the next block reached by normal
     * control flow. */
    struct block *b_next;

    unsigned b_seen:1;          /* flag to used to perform a DFS of blocks */
    int b_startdepth;           /* depth of stack upon entry of block */
    int b_offset;               /* instruction offset of this block */
};

/*
 * A frame block is used to handle loops, try/catch/finally, etc.
 */
enum fblocktype { FB_LOOP, FB_EXCEPT, FB_FINALLY, FB_FINALLY_TRY };
struct fblock {
    enum fblocktype fb_type;
    struct block *fb_block;
};

/* 
 * Compliation unit, change on entry and exit of function scope.
 */
struct compiler_unit {
    COObject *u_consts;
    COObject *u_names;
    COObject *u_localnames;

    /* Pointer to the most recently allocated block. By following b_listnext,
     * you can reach all early allocated blocks. */
    struct block *u_blocklist;
    struct block *u_curblock;   /* currently block */

    int u_nfblocks;
    struct fblock u_fblock[FRAME_MAXBLOCKS];

    int u_argcount;
};

struct assembler {
    COObject *a_bytecode;
    int a_bytecode_offset;
    int a_nblocks;              /* number of reachable blocks */
    struct block **a_postorder; /* list of blocks in dfs postorder */
};

/* Forward declarations */
static int compiler_visit_node(struct compiler *c, Node *n);
static COObject *assemble(struct compiler *c);
#ifdef CO_DEBUG
static void dump_code(COObject *code);
#endif

static struct block *
compiler_new_block(struct compiler *c)
{
    struct block *b;
    b = (struct block *)COObject_Mem_MALLOC(sizeof(struct block));
    if (!b)
        return NULL;
    memset(b, 0, sizeof(struct block));
    b->b_listnext = c->u->u_blocklist;
    c->u->u_blocklist = b;
    return b;
}

static struct block *
compiler_next_block(struct compiler *c)
{
    struct block *b = compiler_new_block(c);
    if (!b)
        return NULL;
    c->u->u_curblock->b_next = b;
    c->u->u_curblock = b;
    return b;
}

static struct block *
compiler_use_new_block(struct compiler *c)
{
    struct block *block = compiler_new_block(c);
    if (!block)
        return NULL;
    c->u->u_curblock = block;
    return block;
}

static struct block *
compiler_use_next_block(struct compiler *c, struct block *block)
{
    assert(block != NULL);
    c->u->u_curblock->b_next = block;
    c->u->u_curblock = block;
    return block;
}

static int
compiler_push_fblock(struct compiler *c, enum fblocktype type, struct block *b)
{
    struct fblock *fb;
    if (c->u->u_nfblocks >= FRAME_MAXBLOCKS) {
        COErr_SetString(COException_SystemError, "too many nested blocks");
        return 0;
    }
    fb = &c->u->u_fblock[c->u->u_nfblocks++];
    fb->fb_type = type;
    fb->fb_block = b;
    return 1;
}

static void
compiler_pop_fblock(struct compiler *c, enum fblocktype type, struct block *b)
{
    assert(c->u->u_nfblocks > 0);
    c->u->u_nfblocks--;
    assert(c->u->u_fblock[c->u->u_nfblocks].fb_type == type);
    assert(c->u->u_fblock[c->u->u_nfblocks].fb_block == b);
}

static void
compiler_unit_free(struct compiler_unit *u)
{
    struct block *b, *next;
    b = u->u_blocklist;
    while (b) {
        if (b->b_instr)
            COObject_Mem_FREE(b->b_instr);
        next = b->b_listnext;
        COObject_Mem_FREE(b);
        b = next;
    }
    CO_DECREF(u->u_localnames);
    CO_DECREF(u->u_names);
    CO_DECREF(u->u_consts);
    COObject_Mem_FREE(u);
}

static void
compiler_enter_scope(struct compiler *c)
{
    struct compiler_unit *u;
    u = (struct compiler_unit *)
        COObject_Mem_MALLOC(sizeof(struct compiler_unit));
    if (!u) {
        return;
    }
    memset(u, 0, sizeof(struct compiler_unit));

    u->u_consts = CODict_New();
    u->u_names = CODict_New();
    u->u_localnames = CODict_New();
    u->u_argcount = 0;

    /* Push the old compiler_unit on the stack. */
    if (c->u) {
        COObject *capsule = COCapsule_New(c->u, NULL);
        if (COList_Append(c->stack, capsule) < 0) {
            CO_DECREF(capsule);
            compiler_unit_free(u);
            return;
        }
        CO_DECREF(capsule);
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

    /* Restore c->u to the parent unit. */
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
compile(struct arena *arena)
{
    // init
    struct compiler c;
    memset(&c, 0, sizeof(struct compiler));
    c.stack = COList_New(0);
    c.arena = arena;

    // do parse
    coparse(&c);

    // compile ast
    compiler_enter_scope(&c);
    compiler_use_new_block(&c);
    compiler_visit_node(&c, c.xtop);
    COObject *co = assemble(&c);
    compiler_exit_scope(&c);

#ifdef CO_DEBUG
    dump_code(co);
#endif

    // free compiler
    CO_DECREF(c.stack);

    return co;
}

/*
 * Get next instruction.
 * Resizes instruction array as necessary.
 */
static int
compiler_next_instr(struct compiler *c, struct block *b)
{
    if (b->b_instr == NULL) {
        b->b_instr =
            (struct instr *)COObject_Mem_MALLOC(sizeof(struct instr) *
                                                DEFAULT_INSTR_SIZE);
        if (!b->b_instr) {
            return -1;
        }
        b->b_ialloc = DEFAULT_INSTR_SIZE;
        b->b_iused = 0;
        memset(b->b_instr, 0, sizeof(struct instr) * DEFAULT_INSTR_SIZE);
    } else if (b->b_iused == b->b_ialloc) {
        size_t oldsize, newsize;
        oldsize = b->b_ialloc * sizeof(struct instr);
        newsize = oldsize << 1;
        if (oldsize > (SIZE_MAX >> 1) || newsize == 0) {
            return -1;
        }
        b->b_ialloc <<= 1;
        b->b_instr = (struct instr *)COObject_Mem_REALLOC(b->b_instr, newsize);
        if (!b->b_instr) {
            return -1;
        }
        memset(b->b_instr + oldsize, 0, newsize - oldsize);
    }
    return b->b_iused++;
}

/*
 * Add const/name and return index.
 */
static int
compiler_add(COObject *dict, COObject *o)
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
 * Add an opcode with no argument.
 */
static int
compiler_addop(struct compiler *c, int opcode)
{
    struct instr *i;
    int off;
    off = compiler_next_instr(c, c->u->u_curblock);
    if (off < 0)
        return -1;
    i = &c->u->u_curblock->b_instr[off];
    i->i_opcode = opcode;
    i->i_hasarg = 0;
    return 0;
}

/*
 * Add an opcode with an integer argument.
 */
static int
compiler_addop_i(struct compiler *c, int opcode, int oparg)
{
    struct instr *i;
    int off;
    off = compiler_next_instr(c, c->u->u_curblock);
    if (off < 0)
        return -1;
    i = &c->u->u_curblock->b_instr[off];
    i->i_opcode = opcode;
    i->i_oparg = oparg;
    i->i_hasarg = 1;
    return off;
}

/*
 * Add an opcode with jump target.
 */
static int
compiler_addop_j(struct compiler *c, int opcode, struct block *b)
{
    struct instr *i;
    int off;
    off = compiler_next_instr(c, c->u->u_curblock);
    if (off < 0)
        return -1;
    i = &c->u->u_curblock->b_instr[off];
    i->i_opcode = opcode;
    i->i_hasarg = 1;
    i->i_target = b;
    return 0;
}

/*
 * Add object load opcode.
 */
static int
compiler_addop_o(struct compiler *c, int opcode, COObject *dict, COObject *o)
{
    int oparg = compiler_add(dict, o);
    if (oparg < 0)
        return 0;
    return compiler_addop_i(c, opcode, oparg);
}

static int
compiler_visit_node(struct compiler *c, Node *n)
{
    int oparg;
    if (!n)
        return 0;

    switch (n->type) {
    case NODE_TREE:
        for (; n; n = n->nd_next) {
            compiler_visit_node(c, n->nd_node);
        }
        break;
    case NODE_PRINT:
        compiler_visit_node(c, n->nd_left);
        compiler_addop(c, OP_PRINT);
        break;
    case NODE_RETURN:
        compiler_visit_node(c, n->nd_left);
        compiler_addop(c, OP_RETURN);
        break;
    case NODE_CONST:
        oparg = compiler_add(c->u->u_consts, n->o);
        compiler_addop_i(c, OP_LOAD_CONST, oparg);
        break;
    case NODE_BIN:
        compiler_visit_node(c, n->nd_left);
        compiler_visit_node(c, n->nd_right);
        compiler_addop(c, n->op);
        break;
    case NODE_STORE_SUBSCRIPT:
        compiler_visit_node(c, n->nd_left);
        compiler_visit_node(c, n->nd_middle);
        compiler_visit_node(c, n->nd_right);
        compiler_addop(c, OP_STORE_SUBSCRIPT);
        break;
    case NODE_CMP:
        compiler_visit_node(c, n->nd_left);
        compiler_visit_node(c, n->nd_right);
        compiler_addop_i(c, OP_CMP, n->oparg);
        break;
    case NODE_UNARY:
        compiler_visit_node(c, n->nd_left);
        compiler_addop(c, n->op);
        break;
    case NODE_TUPLE:
        compiler_visit_node(c, n->nd_list);
        compiler_addop_i(c, OP_BUILD_TUPLE, node_listlen(n->nd_list));
        break;
    case NODE_LIST:
        compiler_visit_node(c, n->nd_list);
        compiler_addop_i(c, OP_BUILD_LIST, node_listlen(n->nd_list));
        break;
    case NODE_DICT_BUILD:
        compiler_addop_i(c, OP_DICT_BUILD, node_listlen(n->nd_list));
        compiler_visit_node(c, n->nd_list);
        break;
    case NODE_DICT_ADD:
        compiler_visit_node(c, n->nd_left);
        compiler_visit_node(c, n->nd_right);
        compiler_addop(c, OP_DICT_ADD);
        break;
    case NODE_NAME:
        oparg = compiler_add(c->u->u_names, n->o);
        compiler_addop_i(c, OP_LOAD_NAME, oparg);
        break;
    case NODE_ASSIGN:
        compiler_visit_node(c, n->nd_right);
        oparg = compiler_add(c->u->u_names, n->nd_left->o);
        compiler_addop_i(c, OP_STORE_NAME, oparg);
        break;
    case NODE_IF:
        {
            struct block *ifelse, *ifend;
            ifend = compiler_new_block(c);
            if (n->nd_condelse) {
                ifelse = compiler_new_block(c);
            } else {
                ifelse = ifend;
            }
            compiler_visit_node(c, n->nd_cond);
            compiler_addop_j(c, OP_JMPZ, ifelse);
            compiler_next_block(c);
            compiler_visit_node(c, n->nd_condbody);
            compiler_addop_j(c, OP_JMPX, ifend);
            if (n->nd_condelse) {
                compiler_use_next_block(c, ifelse);
                compiler_visit_node(c, n->nd_condelse);
            }
            compiler_use_next_block(c, ifend);
            break;
        }
    case NODE_WHILE:
        {
            struct block *loop_start, *loop_end, *loop_exit;
            loop_start = compiler_new_block(c);
            loop_end = compiler_new_block(c);
            loop_exit = compiler_new_block(c);
            compiler_addop_j(c, OP_SETUP_LOOP, loop_end);
            compiler_use_next_block(c, loop_start);

            if (!compiler_push_fblock(c, FB_LOOP, loop_start))
                return 0;

            compiler_visit_node(c, n->nd_cond);
            compiler_addop_j(c, OP_JMPZ, loop_exit);
            compiler_visit_node(c, n->nd_condbody);
            compiler_addop_j(c, OP_JMPX, loop_start);
            compiler_use_next_block(c, loop_exit);
            compiler_addop(c, OP_POP_BLOCK);

            compiler_pop_fblock(c, FB_LOOP, loop_start);

            compiler_use_next_block(c, loop_end);
        }
        break;
    case NODE_FUNC:
        compiler_enter_scope(c);
        compiler_use_new_block(c);
        c->u->u_argcount = node_listlen(n->nd_funcargs);
        Node *l = n->nd_funcargs;
        while (l) {
            oparg = compiler_add(c->u->u_names, l->nd_node->o);
            l = l->nd_next;
        }
        compiler_visit_node(c, n->nd_funcbody);
        COObject *co = assemble(c);
#ifdef CO_DEBUG
        printf("begin function\n");
        dump_code(co);
        printf("end function\n");
#endif
        compiler_exit_scope(c);
        oparg = compiler_add(c->u->u_consts, co);
        CO_DECREF(co);
        compiler_addop_i(c, OP_LOAD_CONST, oparg);
        compiler_addop(c, OP_DECLARE_FUNCTION);

        if (n->nd_funcname) {
            oparg = compiler_add(c->u->u_names, n->nd_funcname->o);
            compiler_addop_i(c, OP_STORE_NAME, oparg);
        }
        break;
    case NODE_FUNC_CALL:
        compiler_visit_node(c, n->nd_params);
        compiler_visit_node(c, n->nd_func);
        oparg = node_listlen(n->nd_params);
        compiler_addop_i(c, OP_CALL_FUNCTION, oparg);
        break;
    case NODE_FUNC_CALL_STMT:
        compiler_visit_node(c, n->nd_params);
        compiler_visit_node(c, n->nd_func);
        oparg = node_listlen(n->nd_params);
        compiler_addop_i(c, OP_CALL_FUNCTION, oparg);
        compiler_addop(c, OP_POP_TOP);
        break;
    case NODE_TRY:
        {
            struct block *body, *end, *handler, *orelse, *finally_end;
            body = compiler_new_block(c);
            end = compiler_new_block(c);
            if (n->nd_orelse) {
                orelse = compiler_new_block(c);
            }
            if (n->nd_finally) {
                finally_end = compiler_new_block(c);
                compiler_addop_j(c, OP_SETUP_FINALLY, finally_end);
            }
            handler = compiler_new_block(c);
            compiler_addop_j(c, OP_SETUP_TRY, handler);
            compiler_use_next_block(c, body);
            if (!compiler_push_fblock(c, FB_EXCEPT, body))
                return 0;
            compiler_visit_node(c, n->nd_trybody);
            compiler_addop(c, OP_POP_BLOCK);
            compiler_pop_fblock(c, FB_EXCEPT, body);
            if (n->nd_orelse) {
                compiler_addop_j(c, OP_JMPX, orelse);
            } else {
                compiler_addop_j(c, OP_JMPX, end);
            }

            Node *l = n->nd_catches;
            Node *catch;
            int len = node_listlen(l);
            int i = 0;
            compiler_use_next_block(c, handler);
            for (; l; l = l->nd_next, i++) {
                catch = l->nd_node;
                if (!catch->nd_catchname && i < len - 1) {
                    printf("len - 1: %d, i: %d\n", len - 1, i);
                    // default catch must be last
                    assert(0);
                }
                handler = compiler_new_block(c);
                if (!handler)
                    return 0;
                if (catch->nd_catchname) {
                    Node *namelist = catch->nd_catchname;
                    compiler_addop(c, OP_DUP_TOP);
                    for (; namelist; namelist = namelist->nd_next) {
                        compiler_visit_node(c, namelist->nd_node);
                    }
                    compiler_addop_i(c, OP_BUILD_TUPLE,
                                     node_listlen(catch->nd_catchname));
                    compiler_addop_i(c, OP_CMP, Cmp_EXC_MATCH);
                    compiler_addop_j(c, OP_JMPZ, handler);
                    compiler_addop(c, OP_POP_TOP);
                    compiler_visit_node(c, catch->nd_catchbody);
                } else {
                    struct block *default_body;
                    default_body = compiler_new_block(c);
                    compiler_push_fblock(c, FB_FINALLY, default_body);
                    compiler_visit_node(c, catch->nd_catchbody);
                    compiler_pop_fblock(c, FB_FINALLY, default_body);
                }
                compiler_addop_j(c, OP_JMPX, end);
                compiler_use_next_block(c, handler);
            }
            compiler_addop(c, OP_END_TRY);
            if (n->nd_orelse) {
                compiler_use_next_block(c, orelse);
                compiler_visit_node(c, n->nd_orelse);
            }
            compiler_use_next_block(c, end);
            if (n->nd_finally) {
                compiler_addop_o(c, OP_LOAD_CONST, c->u->u_consts, CO_None);
                compiler_use_next_block(c, finally_end);
                compiler_visit_node(c, n->nd_finally);
                compiler_addop(c, OP_END_FINALLY);
            }
        }
        break;
    case NODE_THROW:
        if (n->nd_left) {
            compiler_visit_node(c, n->nd_left);
            compiler_addop_i(c, OP_THROW, 1);
        } else {
            compiler_addop_i(c, OP_THROW, 0);
        }
        break;
    case NODE_BREAK:
        compiler_addop(c, OP_BREAK_LOOP);
        break;
    case NODE_CONTINUE:
        if (!c->u->u_nfblocks) {
            // TODO errors
        }
        int i = c->u->u_nfblocks - 1;
        switch (c->u->u_fblock[i].fb_type) {
        case FB_LOOP:
            compiler_addop_j(c, OP_CONTINUE_LOOP, c->u->u_fblock[i].fb_block);
            break;
        default:
            // TODO errors
            break;
        }
        break;
    default:
        error("unknown node type: %d, %s", n->type, node_type(n->type));
    }
    return 0;                   /* unreachable */
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

static int
blocksize(struct block *b)
{
    int i;
    int size = 0;
    for (i = 0; i < b->b_iused; i++)
        size += instrsize(&b->b_instr[i]);
    return size;
}

/*
 * Compute the size of each block and fixup jump offsets.
 */
static void
assembler_jump_offsets(struct assembler *a, struct compiler *c)
{
    int bsize, totalsize;
    struct block *b;
    int i;

    totalsize = 0;
    for (i = a->a_nblocks - 1; i >= 0; i--) {
        b = a->a_postorder[i];
        bsize = blocksize(b);
        b->b_offset = totalsize;
        totalsize += bsize;
    }
    for (b = c->u->u_blocklist; b != NULL; b = b->b_listnext) {
        for (i = 0; i < b->b_iused; i++) {
            struct instr *instr = &b->b_instr[i];
            if (instr->i_opcode == OP_JMPX
                || instr->i_opcode == OP_JMPZ
                || instr->i_opcode == OP_SETUP_LOOP
                || instr->i_opcode == OP_SETUP_TRY
                || instr->i_opcode == OP_CONTINUE_LOOP) {
                // absolutely
                instr->i_oparg = instr->i_target->b_offset;
            } else {
                continue;
            }
        }
    }
}

/*
 * Assemble instruction into bytecode.
 */
static int
assembler_emit(struct assembler *a, struct instr *i)
{
    size_t len = COBytes_Size(a->a_bytecode);
    int size;

    size = instrsize(i);

    // resize if necessary 
    if (a->a_bytecode_offset + size >= len) {
        if (COBytes_Resize(a->a_bytecode, len * 2) < 0)
            return -1;
    }

    char *code = COBytes_AsString(a->a_bytecode) + a->a_bytecode_offset;
    a->a_bytecode_offset += size;
    *code++ = i->i_opcode;
    if (i->i_hasarg) {
        *code++ = i->i_oparg & 0xff;
        *code++ = i->i_oparg >> 8;
    }

    return 0;
}

/*
 * Do depth-first search.
 */
static void
dfs(struct assembler *a, struct block *b)
{
    int i;
    struct instr *instr = NULL;
    if (b->b_seen)
        return;
    b->b_seen = 1;
    if (b->b_next != NULL)
        dfs(a, b->b_next);
    for (i = 0; i < b->b_iused; i++) {
        instr = &b->b_instr[i];
        if (instr->i_opcode == OP_JMP ||
            instr->i_opcode == OP_JMPZ || instr->i_opcode == OP_JMPX) {
            dfs(a, instr->i_target);
        }
    }
    a->a_postorder[a->a_nblocks++] = b;
}

static int
assembler_init(struct assembler *a, int nblocks)
{
    memset(a, 0, sizeof(struct assembler));
    a->a_bytecode = COBytes_FromStringN(NULL, DEFAULT_BYTECODE_SIZE);
    a->a_bytecode_offset = 0;
    a->a_postorder =
        (struct block **)COObject_Mem_MALLOC(sizeof(struct block *) * nblocks);
    a->a_nblocks = 0;
    return 0;
}

static void
assembler_free(struct assembler *a)
{
    CO_XDECREF(a->a_bytecode);
    COObject_Mem_FREE(a->a_postorder);
}

static int
opcode_stack_effect(int opcode, int oparg)
{
    switch (opcode) {
    case OP_NOP:
        return 0;
    case OP_BINARY_ADD:
    case OP_BINARY_SUB:
    case OP_BINARY_MUL:
    case OP_BINARY_DIV:
    case OP_BINARY_MOD:
    case OP_BINARY_POW:
    case OP_BINARY_SL:
    case OP_BINARY_SR:
    case OP_BINARY_SUBSCRIPT:
        return -1;
    case OP_CMP:
        return -1;
    case OP_UNARY_NEGATE:
    case OP_UNARY_INVERT:
        return 0;
    case OP_STORE_NAME:
        return -1;
    case OP_PRINT:
        return -1;
    case OP_JMPZ:
        return -1;
    case OP_JMP:
    case OP_JMPX:
        return 0;
    case OP_DECLARE_FUNCTION:
        return 0;
    case OP_RETURN:
        return -1;
    case OP_CALL_FUNCTION:
        return -oparg;
    case OP_THROW:
        return -oparg;
    case OP_LOAD_NAME:
    case OP_LOAD_CONST:
    case OP_LOAD_LOCAL:
        return 1;
    case OP_BUILD_TUPLE:
    case OP_BUILD_LIST:
        return 1 - oparg;
    case OP_DICT_BUILD:
        return 1;
    case OP_DICT_ADD:
        return -1;
    case OP_SETUP_LOOP:
    case OP_SETUP_TRY:
    case OP_SETUP_FINALLY:
    case OP_POP_BLOCK:
    case OP_BREAK_LOOP:
    case OP_CONTINUE_LOOP:
        return 0;
    case OP_DUP_TOP:
        return 1;
    case OP_POP_TOP:
        return -1;
    case OP_END_TRY:
    case OP_END_FINALLY:
        return -1;
    case OP_STORE_SUBSCRIPT:
        return -3;
    default:
        error("opcode_stack_effect error, opcode: %d\n", opcode);
    }
    return 0;
}

static int
stackdepth_walk(struct compiler *c, struct block *b, int depth, int maxdepth)
{
    int i, base_depth;
    struct instr *instr;
    if (b->b_seen || b->b_startdepth >= depth)
        return maxdepth;

    b->b_seen = 1;
    b->b_startdepth = depth;
    for (i = 0; i < b->b_iused; i++) {
        instr = &b->b_instr[i];
        depth += opcode_stack_effect(instr->i_opcode, instr->i_oparg);
        if (depth > maxdepth)
            maxdepth = depth;
        if (instr->i_target) {
            base_depth = depth;
            maxdepth =
                stackdepth_walk(c, instr->i_target, base_depth, maxdepth);
        }
    }
    if (b->b_next)
        maxdepth = stackdepth_walk(c, b->b_next, depth, maxdepth);
    b->b_seen = 0;
    return maxdepth;
}

/*
 * Find the max stacksize we need.
 * We assume that cycles in the flow graph have no effect on the stack depth.
 * (Loop stack is cleaned on exit of loop.)
 */
static int
stackdepth(struct compiler *c)
{
    struct block *b, *entryblock;
    entryblock = NULL;
    for (b = c->u->u_blocklist; b != NULL; b = b->b_listnext) {
        b->b_seen = 0;
        b->b_startdepth = INT_MIN;
        entryblock = b;
    }
    if (!entryblock)
        return 0;
    return stackdepth_walk(c, entryblock, 0, 0);
}

static COObject *
dict_keys_inorder(COObject *dict, int offset)
{
    COObject *tuple;
    COObject *key;
    COObject *val;

    ssize_t i;
    ssize_t size = CODict_Size(dict);
    tuple = COTuple_New(size);
    if (!tuple)
        return NULL;

    while (CODict_Next(dict, &key, &val) == 0) {
        i = COInt_AsLong(val);
        CO_INCREF(key);
        COTuple_SET_ITEM(tuple, i - offset, key);
    }
    return tuple;
}

static COObject *
makecode(struct compiler *c, struct assembler *a)
{
    COBytes_Resize(a->a_bytecode, a->a_bytecode_offset);

    COObject *consts = COList_New(0);
    COObject *names = COList_New(0);
    COObject *localnames = COList_New(0);
    COObject *name = NULL;
    int nlocals;

    // consts
    consts = dict_keys_inorder(c->u->u_consts, 0);
    if (!consts)
        goto error;
    names = dict_keys_inorder(c->u->u_names, 0);
    if (!names)
        goto error;
    localnames = dict_keys_inorder(c->u->u_localnames, 0);
    if (!localnames)
        goto error;
    nlocals = CODict_Size(c->u->u_localnames);

    // name
    name = COStr_FromString("<main>");

    COObject *co = COCode_New(name, a->a_bytecode,
                              consts, names, localnames,
                              c->u->u_argcount, stackdepth(c), nlocals);

error:
    CO_XDECREF(name);
    CO_XDECREF(consts);
    CO_XDECREF(names);
    return co;
}

/*
 * Assemble instructions into code object.
 */
static COObject *
assemble(struct compiler *c)
{
    struct block *b, *entryblock;
    int nblocks;
    struct assembler a;

    nblocks = 0;
    entryblock = NULL;
    for (b = c->u->u_blocklist; b != NULL; b = b->b_listnext) {
        nblocks++;
        entryblock = b;
    }
    if (assembler_init(&a, nblocks)) {
        return NULL;
    }
    dfs(&a, entryblock);

    assembler_jump_offsets(&a, c);

    /* Emit code in reverse postorder. */
    int i, j;
    for (i = a.a_nblocks - 1; i >= 0; i--) {
        b = a.a_postorder[i];
        for (j = 0; j < b->b_iused; j++) {
            assembler_emit(&a, b->b_instr + j);
        }
    }

    COObject *co = makecode(c, &a);

    assembler_free(&a);

    return co;
}

#ifdef CO_DEBUG
static void
dump_code(COObject *code)
{
#define NEXTOP()    (*bytecode++)
#define NEXTARG()   (bytecode += 2, (bytecode[-1]<<8) + bytecode[-2])
#define GETITEM(v, i)   COTuple_GET_ITEM((COTupleObject *)(v), i)
    COCodeObject *_code = (COCodeObject *)code;
    char *bytecode = COBytes_AsString(_code->co_code);
    char *start = bytecode;
    unsigned char opcode;
    int oparg;
    for (;;) {
        if (bytecode - start == COBytes_Size(_code->co_code)) {
            /* exit code */
            return;
        }
        opcode = NEXTOP();
        printf("%ld.\t%s", bytecode - start - 1, opcode_name(opcode));
        switch (opcode) {
        case OP_LOAD_LOCAL:
            oparg = NEXTARG();
            printf("\t\t%d", oparg);
            printf(" (");
            COObject_Print(GETITEM(_code->co_localnames, oparg), stdout);
            printf(")");
            break;
        case OP_LOAD_CONST:
            oparg = NEXTARG();
            printf("\t\t%d", oparg);
            printf(" (");
            COObject_Print(GETITEM(_code->co_consts, oparg), stdout);
            printf(")");
            break;
        case OP_LOAD_NAME:
            oparg = NEXTARG();
            printf("\t\t%d", oparg);
            printf(" (");
            COObject_Print(GETITEM(_code->co_names, oparg), stdout);
            printf(")");
            break;
        case OP_STORE_NAME:
            oparg = NEXTARG();
            printf("\t\t%d", oparg);
            printf(" (");
            COObject_Print(GETITEM(_code->co_names, oparg), stdout);
            printf(")");
            break;
        case OP_CONTINUE_LOOP:
        case OP_JMP:
        case OP_JMPX:
        case OP_JMPZ:
        case OP_CALL_FUNCTION:
        case OP_CMP:
        case OP_SETUP_LOOP:
        case OP_SETUP_TRY:
        case OP_SETUP_FINALLY:
        case OP_THROW:
        case OP_BUILD_TUPLE:
        case OP_BUILD_LIST:
        case OP_DICT_BUILD:
            oparg = NEXTARG();
            printf("\t\t%d", oparg);
            break;
        }
        printf("\n");
    }
}
#endif

int
colex(YYSTYPE *colval)
{
    int retval;

again:
    retval = scanner_lex(colval);
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

int
coerror(struct compiler *c, const char *err, ...)
{
    va_list params;

    va_start(params, err);

    char msg[1024];
    vsnprintf(msg, sizeof(msg), err, params);
    fprintf(stderr, "%s\n", msg);

    va_end(params);
    exit(128);
}
