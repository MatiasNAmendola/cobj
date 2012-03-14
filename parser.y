%{
#include "co.h"

#define YYSTYPE struct cnode
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

%}

%pure_parser

/*
 * Dangling else statment cause one shift/reduce conflict
 */
%expect 1

%nonassoc T_EQUAL T_NOT_EQUAL 
%token T_MOD_ASSIGN T_DIV_ASSIGN T_MUL_ASSIGN T_SUB_ASSIGN T_ADD_ASSIGN T_SR_ASSIGN T_SL_ASSIGN
%nonassoc '<' '>' T_SMALLER_OR_EQUAL T_GREATER_OR_EQUAL
%token T_SMALLER_OR_EQUAL
%token T_GREATER_OR_EQUAL
%left   ','
%left   '+' '-'
%left   '*' '/' '%'
%left T_SR T_SL
%right  '['
%right  T_PRINT
%token  T_PRINT
%token  T_NONE
%token  T_BOOL
%token  T_NUM
%token  T_FNUM
%token  T_STRING
%token  T_NAME
%token  T_IF
%token  T_ELSE
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

%% /* Context-Free Grammar (BNF) */

start: stmt_list { co_end_compilation(); }
;

stmt: /* state something */
        simple_stmt
    |   compound_stmt
;

stmt_list:
         open_stmt_list opt_stmt_seps
;

open_stmt_list:
        stmt
    |   open_stmt_list stmt_seps stmt
    |   /* empty */
;

stmt_sep:
        T_NEWLINE
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
    |   expr '<' expr { co_binary_op(OP_IS_SMALLER, &$$, &$1, &$3); }
    |   expr '>' expr { co_binary_op(OP_IS_SMALLER, &$$, &$3, &$1); }
    |   expr T_EQUAL expr { co_binary_op(OP_IS_EQUAL, &$$, &$1, &$3); }
    |   expr T_NOT_EQUAL expr { co_binary_op(OP_IS_NOT_EQUAL, &$$, &$1, &$3); }
    |   expr T_SMALLER_OR_EQUAL expr { co_binary_op(OP_IS_SMALLER_OR_EQUAL, &$$, &$1, &$3); }
    |   expr T_GREATER_OR_EQUAL expr { co_binary_op(OP_IS_SMALLER_OR_EQUAL, &$$, &$3, &$1); }
    |   expr T_SL expr { co_binary_op(OP_SL, &$$, &$1, &$3); }
    |   expr T_SR expr { co_binary_op(OP_SR, &$$, &$1, &$3); }
    |   expr '+' expr { co_binary_op(OP_ADD, &$$, &$1, &$3); }
    |   expr '-' expr { co_binary_op(OP_SUB, &$$, &$1, &$3); }
    |   expr '*' expr { co_binary_op(OP_MUL, &$$, &$1, &$3); }
    |   expr '/' expr { co_binary_op(OP_DIV, &$$, &$1, &$3); }
    |   expr '%' expr { co_binary_op(OP_MOD, &$$, &$1, &$3); }
    |   T_NAME '(' { co_begin_func_call(&$1); } func_call_param_list ')' { co_end_func_call(&$1, &$$); }
    |   T_FUNC { co_begin_func_declaration(&$1, NULL); } '(' param_list ')' '{' stmt_list '}' { co_end_func_declaration(&$1, &$$); }
/*    |   '(' { co_tuple_build(&$$, &$1); } expr_list_with_comma ')' { $$ = $1; } */
    |   '[' { co_list_build(&$$, &$1); } expr_list ']' { $$ = $1; $2.type = IS_UNUSED; }
;


simple_stmt:
        T_NAME '=' expr { co_assign(&$$, &$1, &$3); }
    |   T_NAME T_ADD_ASSIGN expr { co_binary_op(OP_ADD, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_SUB_ASSIGN expr { co_binary_op(OP_SUB, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_MUL_ASSIGN expr { co_binary_op(OP_MUL, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_DIV_ASSIGN expr { co_binary_op(OP_DIV, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_MOD_ASSIGN expr { co_binary_op(OP_MOD, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_SR_ASSIGN expr { co_binary_op(OP_SR, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_SL_ASSIGN expr { co_binary_op(OP_SL, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_PRINT expr { co_print(&$2); }
    |   T_RETURN     { co_return(NULL); }
    |   T_RETURN expr { co_return(&$2); }
    |   expr
    |   T_THROW expr
;

compound_stmt:
        T_IF '(' expr ')' { co_if_cond(&$3, &$4); } stmt { co_if_after_stmt(&$4); } opt_else { co_if_end(&$4); }
    |   T_WHILE '(' expr ')' { co_while_cond(&$3, &$1, &$4); } stmt { co_while_end(&$1, &$4); }
    |   try_catch_finally_stmt
    |   T_FUNC T_NAME { co_begin_func_declaration(&$1, &$2); } '(' param_list ')' '{' stmt_list '}' { co_end_func_declaration(&$1, &$$); }
    |   '{' stmt_list '}'
;

try_catch_finally_stmt:
        try_block non_empty_catch_list
    |   try_block catch_list finally_block
;

try_block:
        T_TRY '{' stmt_list '}'
;
    
catch_block:
        T_CATCH '(' expr ')' '{' stmt_list '}'
;

catch_list:
         non_empty_catch_list { $$ = $1; }
    |   /* empty */
;

non_empty_catch_list:
        catch_block
    |   catch_block non_empty_catch_list
;

finally_block:
        T_FINALLY '{' stmt_list '}'
;

param_list:
        non_empty_param_list
    |   /* empty */
;

non_empty_param_list:
        T_NAME { co_recv_param(&$1); }
    |   T_NAME ',' non_empty_param_list { co_recv_param(&$1); }
;

expr_list:
        non_empty_expr_list
    |   /* empty */
;

non_empty_expr_list:
        expr { co_append_element(&$0, &$1); }
    |   non_empty_expr_list ',' expr { co_append_element(&$0, &$3); }
;

func_call_param_list:
        non_empty_func_call_param_list
    |   /* empty */
;

non_empty_func_call_param_list:
        expr { co_pass_param(&$1); }
    |   non_empty_func_call_param_list ',' expr { co_pass_param(&$3); }
;

opt_else:
       /* empty */
    |   T_ELSE stmt
;

%%
