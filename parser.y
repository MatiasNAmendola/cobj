%{
#include "co.h"

//#define YYSTYPE Node *

%}

%pure_parser
%debug
%error-verbose
/*%expect 5*/
%union {
    Node *node;    
    NodeList *list;
}

%parse-param {struct compiler *c}

%nonassoc T_EQUAL T_NOT_EQUAL 
%token T_MOD_ASSIGN T_DIV_ASSIGN T_MUL_ASSIGN T_SUB_ASSIGN T_ADD_ASSIGN T_SR_ASSIGN T_SL_ASSIGN
%nonassoc '<' '>' T_SMALLER_OR_EQUAL T_GREATER_OR_EQUAL
%left   ','
%left   '+' '-'
%left   '*' '/' '%'
%left T_SR T_SL T_POW
%right  '['
%right  T_PRINT
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

%type <node> expr
%type <node> simple_stmt
%type <list> stmt stmt_list start open_stmt_list

%% /* Context-Free Grammar (BNF) */

start: stmt_list { 
     c->xtop = $$; 
     c->xtop = node_concat(c->xtop, node_list(node_new(NODE_RETURN, NULL, NULL), NULL));
    }
;

stmt: /* state something */
        simple_stmt { $$ = node_list($1, NULL); }
;

stmt_list:
         open_stmt_list opt_stmt_seps { $$ = $1; }
;

open_stmt_list:
        stmt { $$ = $1; }
    |   open_stmt_list stmt_seps stmt { $$ = node_concat($1, $3); }
    |   { }/* empty */
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

expr: /* express something */
        T_NAME
    |   T_STRING
    |   T_BOOL
    |   T_NONE
    |   T_NUM
    |   T_FNUM
    |    '(' expr ')' { $$ = $2; }
    |   expr '+' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_ADD; }
    |   expr '-' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_SUB; }
    |   expr '*' expr { $$ = node_new(NODE_BIN, $1, $3); $$->op = OP_MUL; }
    /*
    |   expr '<' expr { co_binary_op(OP_IS_SMALLER, &$$, &$1, &$3); }
    |   expr '>' expr { co_binary_op(OP_IS_SMALLER, &$$, &$3, &$1); }
    |   expr T_EQUAL expr { co_binary_op(OP_IS_EQUAL, &$$, &$1, &$3); }
    |   expr T_NOT_EQUAL expr { co_binary_op(OP_IS_NOT_EQUAL, &$$, &$1, &$3); }
    |   expr T_SMALLER_OR_EQUAL expr { co_binary_op(OP_IS_SMALLER_OR_EQUAL, &$$, &$1, &$3); }
    |   expr T_GREATER_OR_EQUAL expr { co_binary_op(OP_IS_SMALLER_OR_EQUAL, &$$, &$3, &$1); }
    |   expr T_SL expr { co_binary_op(OP_SL, &$$, &$1, &$3); }
    |   expr T_SR expr { co_binary_op(OP_SR, &$$, &$1, &$3); }
    |   expr T_POW expr { co_binary_op(OP_POW, &$$, &$1, &$3); }
    |   expr '/' expr { co_binary_op(OP_DIV, &$$, &$1, &$3); }
    |   expr '%' expr { co_binary_op(OP_MOD, &$$, &$1, &$3); }
    */
;

simple_stmt:
        T_NAME '=' expr { $$ = node_new(NODE_ASSIGN, $1, $3); }
    |   T_PRINT expr { $$ = node_new(NODE_PRINT, $2, NULL); }
    |   expr
;

%%
