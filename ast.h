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
    NODE_RETURN,
    NODE_CONST,
    NODE_NAME,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_LIST_BUILD,
    NODE_LIST_ADD,
} Node_Type;

struct _Node {
    Node_Type type;
    unsigned char op;
    int lineno;
    COObject *o;
    NodeList *list;
    struct _Node *left;
    struct _Node *right;
};

struct _NodeList {
    Node *n;
    struct _NodeList *next;
    struct _NodeList *end;
};

Node *node_new(Node_Type type, Node *nleft, Node *nright);
NodeList *node_list(Node *n, ...);
NodeList *node_concat(NodeList *a, NodeList *b);
NodeList *node_append(NodeList *l, Node *n);
void node_listtree(NodeList *n);
const char *node_type(Node_Type type);

#endif
