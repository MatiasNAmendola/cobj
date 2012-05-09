#ifndef AST_H
#define AST_H
/**
 * Abstract Syntax Tree
 *
 * @link http://en.wikipedia.org/wiki/Abstract_syntax_tree
 */

#include "object.h"
#include "arena.h"

typedef struct _Node Node;

typedef enum {
    NODE_BLOCK,
    NODE_BIN,                   /* binary op node */
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

struct _Node {
    Node_Type type;

    /* Associated data */
    unsigned char op;
    int oparg;
    COObject *o;

    /* For Binary/Unary Node */
    Node *left;
    Node *middle;
    Node *right;

    /* For Dict/List, etc */
    Node *list;

    /* For If/While, etc */
    Node *ntest;
    Node *nbody;
    Node *nelse;

    /* For Func */
    Node *nfuncname;
    Node *nfuncargs;
    Node *nfuncbody;

    /* For Try/Catch/ELse/Fianlly */
    Node *ntrybody;
    Node *ncatches;
    Node *norelse;
    Node *nfinally;

    /* For Catch */
    Node *ncatchname;
    Node *ncatchbody;

    /* For List */
    Node *n;
    Node *next;
    Node *end;
};

Node *node_new(struct arena *arena, Node_Type type, Node *nleft, Node *nright);
Node *nodelist(struct arena *arena, Node *n, ...);
Node *nodelist_concat(Node *a, Node *b);
Node *nodelist_append(struct arena *arena, Node *l, Node *n);
int nodelist_len(Node *l);
Node *nodelist_changetype(Node *l, Node_Type t);
void nodelisttree(Node *n);
const char *node_type(Node_Type type);
void node_print(Node *n);
void nodelist_free(Node *l);

#endif
