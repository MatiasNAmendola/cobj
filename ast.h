#ifndef AST_H
#define AST_H
#include "object.h"

enum Node_Type {
    NODE_IF,
    NODE_WHILE,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_OR,
    NODE_RETURN,
};

typedef struct _Node {
    int type;
    union {
        struct _Node *node;
        COObject *o;
    } u1;
    union {
        struct _Node *node;
        COObject *o;
    } u2;
    union {
        struct _Node *node;
        COObject *o;
    } u3;
} Node;

#endif
