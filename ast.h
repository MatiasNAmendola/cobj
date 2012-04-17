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
    NODE_BIN,                   /* binary op node */
    NODE_CMP,
    NODE_UNARY,
    NODE_IF,
    NODE_WHILE,
    NODE_FUNC,
    NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_CONST,
    NODE_NAME,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_LIST_BUILD,
    NODE_LIST_ADD,
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
    Node *right;

    /* For Dict/List, etc */
    NodeList *list;

    /* For If/While, etc */
    Node *ntest;
    NodeList *nbody;
    NodeList *nelse;

    /* For func */
    Node *nfuncname;
    NodeList *nfuncargs;
    NodeList *nfuncbody;

    /* For Try/Catch/Fianlly */
    NodeList *ntrybody;
    NodeList *ncatches;
    NodeList *nfinally;

    /* For Catch */
    NodeList *ncatchname;
    NodeList *ncatchbody;
};

struct _NodeList {
    Node *n;
    NodeList *next;
    NodeList *end;
};

Node *node_new(Node_Type type, Node *nleft, Node *nright);
NodeList *nodelist(Node *n, ...);
NodeList *nodelist_concat(NodeList *a, NodeList *b);
NodeList *nodelist_append(NodeList *l, Node *n);
int nodelist_len(NodeList *l);
NodeList *nodelist_changetype(NodeList *l, Node_Type t);
void nodelisttree(NodeList *n);
const char *node_type(Node_Type type);
void node_print(Node *n);

#endif
