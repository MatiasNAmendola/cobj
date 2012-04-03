#ifndef AST_H
#define AST_H
/**
 * Abstract Syntax Tree
 *
 * @link http://en.wikipedia.org/wiki/Abstract_syntax_tree
 */

#include "object.h"

typedef struct _NodeList NodeList;
typedef struct _Node Node;

typedef enum {
    NODE_BIN,       /* binary op node */
    NODE_IF,
    NODE_WHILE,
    NODE_FUNC,
    NODE_RETURN,
    NODE_CONST,
    NODE_NAME,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_LIST_BUILD,
    NODE_LIST_ADD,
    NODE_DICT_BUILD,
    NODE_DICT_ADD,
} Node_Type;

struct _Node {
    Node_Type type;

    /* Associated data */
    unsigned char op;
    COObject *o;

    /* For Binary/Unary Node */
    Node *left;
    Node *right;

    /* For Dict/List, etc */
    NodeList *list;

    /* For If/While, etc */
    Node *ntest;
    NodeList *nbody;
    NodeList *nelse;

    /* For func */
    Node *nfuncname;
    NodeList *nfuncparams;
    NodeList *nfuncbody;
};

struct _NodeList {
    Node *n;
    NodeList *next;
    NodeList *end;
};

Node *node_new(Node_Type type, Node *nleft, Node *nright);
NodeList *node_list(Node *n, ...);
NodeList *node_concat(NodeList *a, NodeList *b);
NodeList *node_append(NodeList *l, Node *n);
void node_listtree(NodeList *n);
const char *node_type(Node_Type type);

#endif
