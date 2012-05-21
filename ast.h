#ifndef AST_H
#define AST_H
/**
 * Abstract Syntax Tree
 *
 * @link http://en.wikipedia.org/wiki/Abstract_syntax_tree
 */

#include "object.h"
#include "arena.h"

typedef enum {
    NODE_TREE,
    NODE_BIN,
    NODE_STORE_SUBSCRIPT,
    NODE_CMP,
    NODE_UNARY,
    NODE_IF,
    NODE_WHILE,
    NODE_FUNC,
    NODE_FUNC_CALL,
    NODE_FUNC_CALL_STMT,
    NODE_RETURN,
    NODE_CONST,
    NODE_NAME,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_LIST,
    NODE_DICT_BUILD,
    NODE_DICT_ADD,
    NODE_TRY,
    NODE_CATCH,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_THROW,
} Node_Type;

typedef struct _Node {
    Node_Type type;

    /* Associated data */
    unsigned char op;
    int oparg;
    COObject *o;

    struct _Node *n1;
    struct _Node *n2;
    struct _Node *n3;
    struct _Node *n4;
} Node;

/* 
 * NODE_TREE
 */
#define nd_node n1
#define nd_next n2
#define nd_end  n3

/*
 * NODE_BIN
 * NODE_STORE_SUBSCRIPT
 * NODE_UNARY
 */
#define nd_left     n1
#define nd_middle   n2
#define nd_right    n3

/*
 * NODE_LIST
 * NODE_DICT_BUILD
 */
#define nd_list     n1

/* NODE_FUNC_CALL
 */
#define nd_func     n1
#define nd_params   n2

/*
 * NODE_IF
 * NODE_WHILE
 */
#define nd_cond     n1
#define nd_condbody n2
#define nd_condelse n3

/*
 * NODE_FUNC
 */
#define nd_funcname n1
#define nd_funcargs n2
#define nd_funcbody n3

/*
 * NODE_TRY
 */
#define nd_trybody  n1
#define nd_catches  n2
#define nd_orelse   n3
#define nd_finally  n4

/*
 * NODE_CATCH
 */
#define nd_catchname    n1
#define nd_catchbody    n2

Node *node_new(struct arena *arena, Node_Type type, Node *nleft, Node *nright);
Node *node_list(struct arena *arena, Node *n, ...);
Node *node_listconcat(Node *a, Node *b);
Node *node_listappend(struct arena *arena, Node *l, Node *n);
int node_listlen(Node *l);
const char *node_type(Node_Type type);

#endif
