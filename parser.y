%{
#include "co.h"

static Node *
return_none_node()
{
    Node *n;
    n = COMem_MALLOC(sizeof(Node));
    n->o = CO_None;
    n->type = NODE_CONST;
    return node_new(NODE_RETURN, n, NULL);
}

%}

%pure_parser
%debug
%error-verbose

%union {
    Node *node;
    NodeList *list;
}

%parse-param {struct compiler *c}

/*
 * The relative precedence of different operators is controlled by the order in
 * which they are declared, from lower to higher.
 * See http://www.gnu.org/software/bison/manual/bison.html#Decl-Summary.
 */
%token  T_IF T_ELIF T_ELSE
%token  T_FUNC
%token  T_RETURN
%token  T_WHILE
%token  T_NEWLINES
%token  T_WHITESPACE
%token  T_COMMENT
%token  T_IGNORED
%token  T_TRY
%token  T_THROW
%token  T_CATCH
%token  T_FINALLY
%token  T_END
%token  <node> T_NONE
%token  <node> T_BOOL
%token  <node> T_NUM
%token  <node> T_FNUM
%token  <node> T_STRING
%token  <node> T_NAME
%nonassoc T_EQUAL T_NOT_EQUAL
%token T_MOD_ASSIGN T_DIV_ASSIGN T_MUL_ASSIGN T_SUB_ASSIGN T_ADD_ASSIGN T_SR_ASSIGN T_SL_ASSIGN
%nonassoc '<' '>' T_SMALLER_OR_EQUAL T_GREATER_OR_EQUAL
%left   ','
%left   '+' '-'
%left   '*' '/' '%'
%left   T_SR T_SL T_POW
%right  NEGATIVE
%right  '[' '{'
%right  T_PRINT

%type <node> expr
%type <list> stmt stmt_list start open_stmt_list
%type <list> simple_stmt compound_stmt opt_else if_tail
%type <list> expr_list non_empty_expr_list
%type <list> assoc_list non_empty_assoc_list
%type <list> opt_param_list param_list non_empty_param_list

/*
 * Manual override of shift/reduce conflicts.
 * The general form is that we assign a precedence to the token being shifted
 * and the introuce NotToken with lower precedence or PreferToToken with higher
 * and annotate the reducing rule accordingly.
 * (learn from go/gc.y)
 */
%left NotName
%left T_NAME

%left NotParen
%left '('

%left ')'
%left PreferToRightParen


%% /* Context-Free Grammar (BNF) */

start: stmt_list {
        if ($$) {
            c->xtop = $$;
            c->xtop = nodelist_concat($$, nodelist(return_none_node(), NULL));
        } else {
            c->xtop = nodelist(return_none_node(),
            NULL);
        }
    }
;

stmt: /* state something */
        simple_stmt { $$ = $1; }
    |   compound_stmt { $$ = $1; }
;

stmt_list:
         open_stmt_list opt_stmt_seps { $$ = $1; }
;

open_stmt_list:
        stmt { $$ = $1; }
    |   open_stmt_list stmt_seps stmt {
            if ($1) {
                $$ = nodelist_concat($1, $3);
            } else {
                $$ = $3;
            }
        }
    |   /* empty */ { $$ = 0; }
;

stmt_sep:
        T_NEWLINES
    |   ';'
;

stmt_seps:
        stmt_sep
    |   stmt_seps stmt_sep
;

opt_stmt_seps:
        /* empty */
    |   stmt_seps
;

funcliteral:
        T_FUNC %prec NotName
;

expr: /* express something */
        T_NAME %prec NotParen
    |   T_STRING
    |   T_BOOL
    |   T_NONE
    |   T_NUM
    |   T_FNUM
    |    '(' expr ')' { $$ = $2; }
    |   expr '+' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_ADD; }
    |   expr '-' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_SUB; }
    |   expr '*' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_MUL; }
    |   expr '/' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_DIV; }
    |   expr '%' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_MOD; }
    |   expr '<' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_IS_SMALLER; }
    |   expr '>' expr { $$ = node_new(NODE_BIN, $3, $1); $$->op = OP_IS_SMALLER; }
    |   expr T_EQUAL expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_IS_EQUAL; }
    |   expr T_NOT_EQUAL expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_IS_NOT_EQUAL; }
    |   expr T_SMALLER_OR_EQUAL expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_IS_SMALLER_OR_EQUAL; }
    |   expr T_GREATER_OR_EQUAL expr { $$ = node_new(NODE_BIN, $3, $1); $$->op = OP_IS_SMALLER_OR_EQUAL; }
    |   expr T_SL expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_SL; }
    |   expr T_SR expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_SR; }
    |   expr T_POW expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_POW; }
    |   '[' expr_list ']' {
            $$ = node_new(NODE_LIST_BUILD, NULL, NULL);
            if ($2) {
                NodeList *t = $2;
                while (t) {
                    t->n = node_new(NODE_LIST_ADD, t->n, NULL);
                    t = t->next;
                }
            }
            $$->list = $2;
        }
    |   '{' assoc_list '}' {
            $$ = node_new(NODE_DICT_BUILD, NULL, NULL);
            $$->list = $2;
        }
    |   T_NAME '(' expr_list ')' {
            $$ = node_new(NODE_FUNC_CALL, $1, NULL);
            $$->list = $3;
        }
    |   funcliteral opt_param_list stmt_list T_END {
            Node *t = node_new(NODE_FUNC, NULL, NULL);
            t->nfuncname = 0;
            t->nfuncargs = $2;
            t->nfuncbody = $3;
            if (t->nfuncbody) {
                t->nfuncbody = nodelist_append(t->nfuncbody, return_none_node());
            } else {
                t->nfuncbody = nodelist(return_none_node(), NULL);
            }
            $$ = t;
        }
;

opt_comma:
        ',' opt_newlines
    |   /* empty */
;

opt_newlines:
        T_NEWLINES
    |   /* empty */
;

expr_list:
        non_empty_expr_list opt_comma { $$ = $1; }
    |   /* empty */ { $$ = 0; }
;

non_empty_expr_list:
        opt_newlines expr opt_newlines {
            $$ = nodelist($2, NULL);
        }
    |   non_empty_expr_list ',' opt_newlines expr opt_newlines {
            $$ = nodelist_append($1, $4);
        }
;

assoc_list:
        non_empty_assoc_list opt_comma
    |   /* empty */ { $$ = 0; }
;

non_empty_assoc_list:
        opt_newlines expr ':' expr opt_newlines {
            $$ = nodelist(node_new(NODE_DICT_ADD, $2, $4), NULL);
        }
    |   non_empty_assoc_list ',' opt_newlines expr ':' expr opt_newlines {
            $$ = nodelist_append($1, node_new(NODE_DICT_ADD, $4, $6));
        }
;

simple_stmt:
        T_NAME '=' expr { Node *t = node_new(NODE_ASSIGN, $1, $3); $$ = nodelist(t, NULL); }
    |   T_NAME T_ADD_ASSIGN expr {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_ADD;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_SUB_ASSIGN expr  {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_SUB;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_MUL_ASSIGN expr  {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_MUL;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_DIV_ASSIGN expr  {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_DIV;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_MOD_ASSIGN expr {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_MOD;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_SR_ASSIGN expr {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_SR;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_SL_ASSIGN expr {
            Node *t;
            t = node_new(NODE_BIN, $1, $3); t->op = OP_SL;
            $$ = nodelist(t, node_new(NODE_ASSIGN, $1, t), NULL);
        }
    |   T_PRINT expr { Node *t = node_new(NODE_PRINT, $2, NULL); $$ = nodelist(t, NULL); }
    |   expr { $$ = nodelist($1, NULL); }
    |   T_RETURN { $$ = nodelist(return_none_node(), NULL); }
    |   T_RETURN expr { $$ = nodelist(node_new(NODE_RETURN, $2, NULL), NULL); }
;

compound_stmt:
        T_IF expr stmt_list if_tail T_END {
            Node *t = node_new(NODE_IF, NULL, NULL);
            t->ntest = $2;
            t->nbody = $3;
            t->nelse = $4;
            $$ = nodelist(t, NULL);
        }
    |   T_WHILE expr stmt_list T_END {
            Node *t = node_new(NODE_WHILE, NULL, NULL);
            t->ntest = $2;
            t->nbody = $3;
            $$ = nodelist(t, NULL);
        }
    |   T_FUNC T_NAME opt_param_list stmt_list T_END {
            Node *t = node_new(NODE_FUNC, NULL, NULL);
            t->nfuncname = $2;
            t->nfuncargs = $3;
            t->nfuncbody = $4;
            if (t->nfuncbody) {
                t->nfuncbody = nodelist_append(t->nfuncbody, return_none_node());
            } else {
                t->nfuncbody = nodelist(return_none_node());
            }
            $$ = nodelist(t, NULL);
        }
;

opt_param_list:
        '(' param_list ')' { $$ = $2; }
    |   %prec NotParen /* empty */ { $$ = 0; }
;

param_list:
        non_empty_param_list
    |   /* empty */ { $$ = 0; }
;

non_empty_param_list:
        T_NAME {
            $$ = nodelist($1, NULL);
        }
    |   non_empty_param_list ',' T_NAME {
            $$ = nodelist_append($1, $3);
        }
;

opt_else:
        /* empty */ { $$ = 0; }
    |   T_ELSE stmt_list { $$ = $2; }
;

if_tail:
       opt_else
    |  T_ELIF expr stmt_list if_tail {
            Node *t = node_new(NODE_IF, NULL, NULL);
            t->ntest = $2;
            t->nbody = $3;
            t->nelse = $4;
            $$ = nodelist(t, NULL);
        }
;

%%
