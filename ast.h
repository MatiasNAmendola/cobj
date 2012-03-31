#ifndef AST_H
#define AST_H
/**
 * Abstract Syntax Tree
 *
 * @link http://en.wikipedia.org/wiki/Abstract_syntax_tree
 */

#include "object.h"

typedef enum {
    NODE_IF,
    NODE_WHILE,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_OR,
    NODE_RETURN,
    NODE_CONST,
    NODE_ASSIGN,
    NODE_PRINT,
} Node_Type;

typedef struct _Node {
    Node_Type type;
    int lineno;
    COObject *o;
    struct _Node *left;
    struct _Node *right;
} Node;

typedef struct _NodeList {
    Node *n;
    struct _NodeList *next;
    struct _NodeList *end;
} NodeList;

Node *node_new(Node_Type type, Node *nleft, Node *nright);
NodeList *node_list(Node *n, ...);
NodeList *node_concat(NodeList *a, NodeList *b);
void node_listtree(NodeList *n);

#endif
