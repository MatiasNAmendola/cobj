#ifndef AST_H
#define AST_H
#include "object.h"

typedef enum {
    NODE_IF,
    NODE_WHILE,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_OR,
    NODE_RETURN,
} Node_Type;

typedef struct _Node {
    Node_Type type;
    struct _Node *left;
    struct _Node *right;
    int lineno;
    COObject *o;
} Node;

Node *node_new(Node_Type type, Node *nleft, Node *nright);
#endif
