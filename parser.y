%{
#include "co.h"

#define YYSTYPE struct cnode
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

%}

%pure_parser

%expect 5

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
    |   T_NAME '(' func_call_param_list ')' { co_end_func_call(&$1, &$$); }
    |   T_FUNC { co_begin_func_declaration(&$1, NULL); } opt_param_list stmt_list T_END { co_end_func_declaration(&$1, &$$); }
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
        T_IF expr { co_if_cond(&$2, &$1); }  stmt_list { co_if_after_stmt(&$1); } if_tail T_END { co_if_end(&$1); }
    |   T_WHILE { $1.u.opline_num = co_get_next_opline_num(); } expr {
    co_while_cond(&$3, &$1); } stmt_list T_END { co_while_end(&$1); }
    |   T_TRY stmt_list opt_catch_list opt_finally_block T_END
    |   T_FUNC T_NAME { co_begin_func_declaration(&$1, &$2); } opt_param_list
            stmt_list
        T_END { co_end_func_declaration(&$1, &$$); }
;

opt_finally_block:
        T_FINALLY  stmt_list 
    |   /* empty */
;

catch_block:
        T_CATCH opt_excep_list stmt_list 
;

catch_list:
        catch_block
    |   catch_list catch_block
;

opt_catch_list:
        catch_list
    |   /* empty */
;

opt_excep_list:
        T_NAME
    |   /* empty */
;

opt_param_list:
        '(' param_list ')'
    |   /* empty */
;

param_list:
        non_empty_param_list
    |   /* empty */
;

non_empty_param_list:
        T_NAME { co_recv_param(&$1); }
    |   T_NAME ',' non_empty_param_list { co_recv_param(&$1); }
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
        non_empty_expr_list opt_comma
    |   /* empty */
;

non_empty_expr_list:
        opt_newlines expr opt_newlines { co_append_element(&$0, &$2); }
    |   non_empty_expr_list ',' opt_newlines expr opt_newlines { co_append_element(&$0, &$4); }
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
    |   T_ELSE stmt_list
;

if_tail:
       opt_else
    |  T_ELIF expr { co_if_cond(&$2, &$1); }  stmt_list { co_if_after_stmt(&$1); } if_tail  { co_if_end(&$1); }
;

%%
