%{
#include "co.h"

static Node *
return_none_node(struct arena *arena)
{
    Node *n = node_new(arena, NODE_CONST, NULL, NULL);
    n->o = CO_None;
    return node_new(arena, NODE_RETURN, n, NULL);
}

%}

%pure_parser
%debug
%error-verbose

%union {
    Node *node;
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
%token  T_NEWLINE
%token  T_WHITESPACE
%token  T_COMMENT
%token  T_IGNORED
%token  T_TRY
%token  T_THROW
%token  T_CATCH
%token  T_FINALLY
%token  T_END
%token  T_THEN
%token  T_BREAK
%token  T_CONTINUE
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
%right  '[' '{'
%right  T_PRINT
%left   UNARY_OP

%type <node> expr
%type <node> stmt stmt_list start open_stmt_list
%type <node> simple_stmt compound_stmt opt_else if_tail
%type <node> expr_list non_empty_expr_list expr_list_inline non_empty_expr_list_inline
%type <node> assoc_list non_empty_assoc_list
%type <node> opt_param_list name_list non_empty_name_list
%type <node> catch_block
%type <node> catch_list opt_catch_list opt_finally_block

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
            c->xtop = nodelist_concat($$, nodelist(c->arena, return_none_node(c->arena), NULL));
        } else {
            c->xtop = nodelist(c->arena, return_none_node(c->arena), NULL);
        }
    }
;

stmt: /* state something */
        simple_stmt { $$ = $1; }
    |   compound_stmt { $$ = $1; }
;

stmt_list:
         open_stmt_list opt_stmt_terms { $$ = $1; }
;

open_stmt_list:
        stmt { $$ = $1; }
    |   open_stmt_list stmt_terms stmt {
            if ($1) {
                $$ = nodelist_concat($1, $3);
            } else {
                $$ = $3;
            }
        }
    |   /* empty */ { $$ = 0; }
;

stmt_term:
        T_NEWLINE
    |   ';'
;

stmt_terms:
        stmt_term
    |   stmt_terms stmt_term
;

opt_stmt_terms:
        /* empty */
    |   stmt_terms
;

/*
 * `then` is nessary to distinguish expr with following statements, cuz expr can
 * also be statement. e.g. if `then` is omit, there is reduce/reduce conflict in
 * stmt: if a - b end.
 */
then:
        T_NEWLINE
    |   T_THEN
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
    |   expr '+' expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_ADD; }
    |   expr '-' expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_SUB; }
    |   expr '*' expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_MUL; }
    |   expr '/' expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_DIV; }
    |   expr '%' expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_MOD; }
    |   expr '<' expr { $$ = node_new(c->arena, NODE_CMP, $1, $3); $$->oparg = Cmp_LT; }
    |   expr '>' expr { $$ = node_new(c->arena, NODE_CMP, $1, $3); $$->oparg = Cmp_GT; }
    |   expr T_EQUAL expr { $$ = node_new(c->arena, NODE_CMP, $1, $3); $$->oparg = Cmp_EQ; }
    |   expr T_NOT_EQUAL expr { $$ = node_new(c->arena, NODE_CMP, $1, $3); $$->oparg =
    Cmp_NE; }
    |   expr T_SMALLER_OR_EQUAL expr { $$ = node_new(c->arena, NODE_CMP, $1, $3);
    $$->oparg = Cmp_LE; }
    |   expr T_GREATER_OR_EQUAL expr { $$ = node_new(c->arena, NODE_CMP, $1, $3);
    $$->oparg = Cmp_GE; }
    |   expr T_SL expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_SL; }
    |   expr T_SR expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_SR; }
    |   expr T_POW expr { $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_POW; }
    |   '-' expr %prec UNARY_OP { $$ = node_new(c->arena, NODE_UNARY, $2, NULL); $$->op =
    OP_UNARY_NEGATE; }
    |   '~' expr %prec UNARY_OP { $$ = node_new(c->arena, NODE_UNARY, $2, NULL); $$->op =
    OP_UNARY_INVERT; }
    |   '[' expr_list ']' {
            $$ = node_new(c->arena, NODE_LIST, NULL, NULL);
            $$->nd_list = $2;
        }
    |   '{' assoc_list '}' {
            $$ = node_new(c->arena, NODE_DICT_BUILD, NULL, NULL);
            $$->nd_list = $2;
        }
    |   expr '(' expr_list ')' {
            $$ = node_new(c->arena, NODE_FUNC_CALL, NULL, NULL);
            $$->nd_func = $1;
            $$->nd_params = $3;
        }
    |   expr '[' expr ']' {
            $$ = node_new(c->arena, NODE_BIN, $1, $3); $$->op = OP_BINARY_SUBSCRIPT;
        }
    |   funcliteral opt_param_list stmt_list T_END {
            Node *t = node_new(c->arena, NODE_FUNC, NULL, NULL);
            t->nd_funcname = 0;
            t->nd_funcargs = $2;
            t->nd_funcbody = $3;
            if (t->nd_funcbody) {
                t->nd_funcbody = nodelist_append(c->arena, t->nd_funcbody, return_none_node(c->arena));
            } else {
                t->nd_funcbody = nodelist(c->arena, return_none_node(c->arena), NULL);
            }
            $$ = t;
        }
;

opt_comma:
        ',' opt_newlines
    |   /* empty */
;

newlines:
        T_NEWLINE
    |   T_NEWLINE T_NEWLINE
;

opt_newlines:
        newlines
    |   /* empty */
;

expr_list_inline:
        non_empty_expr_list_inline
    |   /* empty */ { $$ = 0; }
;

non_empty_expr_list_inline:
        expr {
            $$ = nodelist(c->arena, $1, NULL);
        }
    |   non_empty_expr_list_inline ',' expr {
            $$ = nodelist_append(c->arena, $1, $3);
        }
;

expr_list:
        non_empty_expr_list opt_comma { $$ = $1; }
    |   /* empty */ { $$ = 0; }
;

non_empty_expr_list:
        opt_newlines expr opt_newlines {
            $$ = nodelist(c->arena, $2, NULL);
        }
    |   non_empty_expr_list ',' opt_newlines expr opt_newlines {
            $$ = nodelist_append(c->arena, $1, $4);
        }
;

assoc_list:
        non_empty_assoc_list opt_comma
    |   /* empty */ { $$ = 0; }
;

non_empty_assoc_list:
        opt_newlines expr ':' expr opt_newlines {
            $$ = nodelist(c->arena, node_new(c->arena, NODE_DICT_ADD, $2, $4), NULL);
        }
    |   non_empty_assoc_list ',' opt_newlines expr ':' expr opt_newlines {
            $$ = nodelist_append(c->arena, $1, node_new(c->arena, NODE_DICT_ADD, $4, $6));
        }
;

simple_stmt:
        T_NAME '=' expr { Node *t = node_new(c->arena, NODE_ASSIGN, $1, $3); $$ = nodelist(c->arena, t, NULL); }
    |   expr '[' expr ']' '=' expr {
            Node *t = node_new(c->arena, NODE_STORE_SUBSCRIPT, NULL, NULL);
            t->nd_left = $1;
            t->nd_middle = $3;
            t->nd_right = $6;
            $$ = nodelist(c->arena, t, NULL);
        }
    |   T_NAME T_ADD_ASSIGN expr {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_ADD;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_SUB_ASSIGN expr  {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_SUB;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_MUL_ASSIGN expr  {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_MUL;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_DIV_ASSIGN expr  {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_DIV;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_MOD_ASSIGN expr {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_MOD;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_SR_ASSIGN expr {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_SR;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_NAME T_SL_ASSIGN expr {
            Node *t;
            t = node_new(c->arena, NODE_BIN, $1, $3); t->op = OP_BINARY_SL;
            $$ = nodelist(c->arena, node_new(c->arena, NODE_ASSIGN, $1, t), NULL);
        }
    |   T_BREAK {
            $$ = nodelist(c->arena, node_new(c->arena, NODE_BREAK, 0, 0), NULL);
        }
    |   T_CONTINUE {
            $$ = nodelist(c->arena, node_new(c->arena, NODE_CONTINUE, 0, 0), NULL);
        }
    |   T_THROW {
            $$ = nodelist(c->arena, node_new(c->arena, NODE_THROW, 0, 0), NULL);
        }
    |   T_THROW expr {
            $$ = nodelist(c->arena, node_new(c->arena, NODE_THROW, $2, 0), NULL);
        }
    |   T_PRINT expr { Node *t = node_new(c->arena, NODE_PRINT, $2, NULL); $$ = nodelist(c->arena, t, NULL); }
    |   expr { 
            if ($1->type == NODE_FUNC_CALL) {
                $1->type = NODE_FUNC_CALL_STMT;
            }
            $$ = nodelist(c->arena, $1, NULL); 
        }
    |   T_RETURN { $$ = nodelist(c->arena, return_none_node(c->arena), NULL); }
    |   T_RETURN expr { $$ = nodelist(c->arena, node_new(c->arena, NODE_RETURN, $2, NULL), NULL); }
;

compound_stmt:
        T_IF expr then stmt_list if_tail T_END {
            Node *t = node_new(c->arena, NODE_IF, NULL, NULL);
            t->nd_cond = $2;
            t->nd_condbody = $4;
            t->nd_condelse = $5;
            $$ = nodelist(c->arena, t, NULL);
        }
    |   T_WHILE expr then stmt_list T_END {
            Node *t = node_new(c->arena, NODE_WHILE, NULL, NULL);
            t->nd_cond = $2;
            t->nd_condbody = $4;
            $$ = nodelist(c->arena, t, NULL);
        }
    |   T_FUNC T_NAME opt_param_list stmt_list T_END {
            Node *t = node_new(c->arena, NODE_FUNC, NULL, NULL);
            t->nd_funcname = $2;
            t->nd_funcargs = $3;
            t->nd_funcbody = $4;
            if (t->nd_funcbody) {
                t->nd_funcbody = nodelist_append(c->arena, t->nd_funcbody, return_none_node(c->arena));
            } else {
                t->nd_funcbody = nodelist(c->arena, return_none_node(c->arena),
                NULL);
            }
            $$ = nodelist(c->arena, t, NULL);
        }
    |   T_TRY stmt_list opt_catch_list opt_else opt_finally_block T_END {
            Node *t = node_new(c->arena, NODE_TRY, NULL, NULL);
            t->nd_trybody = $2;
            t->nd_catches = $3;
            t->nd_orelse = $4;
            t->nd_finally = $5;
            $$ = nodelist(c->arena, t, NULL);
        }
;

opt_finally_block:
        T_FINALLY stmt_list { $$ = $2; }
    |   /* empty */ { $$ = 0; }
;

catch_block:
        T_CATCH expr_list_inline then stmt_list {
            $$ = node_new(c->arena, NODE_CATCH, NULL, NULL);
            $$->nd_catchname = $2;
            $$->nd_catchbody = $4;
        }
;

catch_list:
        catch_block { $$ = nodelist(c->arena, $1, NULL); }
    |   catch_list catch_block { $$ = nodelist_append(c->arena, $1, $2); }
;

opt_catch_list:
        catch_list
    |   /* empty */ { $$ = 0; }
;

opt_param_list:
        '(' name_list ')' { $$ = $2; }
    |   %prec NotParen /* empty */ { $$ = 0; }
;

name_list:
        non_empty_name_list
    |   /* empty */ { $$ = 0; }
;

non_empty_name_list:
        T_NAME {
            $$ = nodelist(c->arena, $1, NULL);
        }
    |   non_empty_name_list ',' T_NAME {
            $$ = nodelist_append(c->arena, $1, $3);
        }
;

opt_else:
        /* empty */ { $$ = 0; }
    |   T_ELSE stmt_list { $$ = $2; }
;

if_tail:
       opt_else
    |  T_ELIF expr then stmt_list if_tail {
            Node *t = node_new(c->arena, NODE_IF, NULL, NULL);
            t->nd_cond = $2;
            t->nd_condbody = $4;
            t->nd_condelse = $5;
            $$ = nodelist(c->arena, t, NULL);
        }
;

%%
