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
%token  T_WHITESPACE
%token  T_COMMENT
%token  T_IGNORED
%token  T_TRY
%token  T_THROW
%token  T_CATCH
%token  T_FINALLY

%% /* Context-Free Grammar (BNF) */

start:
        statement_list { co_end_compilation(); }
    |   /* empty */
;

expression: /* express something */
        T_NAME
    |   T_STRING
    |   T_BOOL
    |   T_NONE
    |   T_NUM
    |   T_FNUM
    |    '(' expression ')' { $$ = $2; }
    |   expression '<' expression { co_binary_op(OP_IS_SMALLER, &$$, &$1, &$3); }
    |   expression '>' expression { co_binary_op(OP_IS_SMALLER, &$$, &$3, &$1); }
    |   expression T_EQUAL expression { co_binary_op(OP_IS_EQUAL, &$$, &$1, &$3); }
    |   expression T_NOT_EQUAL expression { co_binary_op(OP_IS_NOT_EQUAL, &$$, &$1, &$3); }
    |   expression T_SMALLER_OR_EQUAL expression { co_binary_op(OP_IS_SMALLER_OR_EQUAL, &$$, &$1, &$3); }
    |   expression T_GREATER_OR_EQUAL expression { co_binary_op(OP_IS_SMALLER_OR_EQUAL, &$$, &$3, &$1); }
    |   expression T_SL expression { co_binary_op(OP_SL, &$$, &$1, &$3); }
    |   expression T_SR expression { co_binary_op(OP_SR, &$$, &$1, &$3); }
    |   expression '+' expression { co_binary_op(OP_ADD, &$$, &$1, &$3); }
    |   expression '-' expression { co_binary_op(OP_SUB, &$$, &$1, &$3); }
    |   expression '*' expression { co_binary_op(OP_MUL, &$$, &$1, &$3); }
    |   expression '/' expression { co_binary_op(OP_DIV, &$$, &$1, &$3); }
    |   expression '%' expression { co_binary_op(OP_MOD, &$$, &$1, &$3); }
    |   function_call
    |   function_literal
/*    |   '(' { co_tuple_build(&$$, &$1); } expression_list_with_comma ')' { $$ = $1; } */
    |   '[' { co_list_build(&$$, &$1); } expression_list ']' { $$ = $1; $2.type = IS_UNUSED; }
;

statement: /* state something */
        simple_statement
    |   compound_statement
;

statement_list:
        statement
    |   statement_list statement
;

simple_statement:
        T_NAME '=' expression ';' { co_assign(&$$, &$1, &$3); }
    |   T_NAME T_ADD_ASSIGN expression ';' { co_binary_op(OP_ADD, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_SUB_ASSIGN expression ';' { co_binary_op(OP_SUB, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_MUL_ASSIGN expression ';' { co_binary_op(OP_MUL, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_DIV_ASSIGN expression ';' { co_binary_op(OP_DIV, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_MOD_ASSIGN expression ';' { co_binary_op(OP_MOD, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_SR_ASSIGN expression ';' { co_binary_op(OP_SR, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_NAME T_SL_ASSIGN expression ';' { co_binary_op(OP_SL, &$$, &$1, &$3); co_assign(&$$, &$1, &$$); }
    |   T_PRINT expression ';' { co_print(&$2); }
    |   T_RETURN ';'            { co_return(NULL); }
    |   T_RETURN expression ';' { co_return(&$2); }
    |   expression ';'
    |   T_THROW expression ';'
    |   ';' /* empty */
;

compound_statement:
        T_IF '(' expression ')' { co_if_cond(&$3, &$4); } statement { co_if_after_statement(&$4); } optional_else { co_if_end(&$4); }
    |   T_WHILE '(' expression ')' { co_while_cond(&$3, &$1, &$4); } statement { co_while_end(&$1, &$4); }
    |   try_catch_finally_stmt
    |   function_declaration
    |   compound_statement_with_parentheses
;

compound_statement_with_parentheses:
        '{' statement_list '}'
    |   '{' /* empty */ '}'
;

try_catch_finally_stmt:
        try_block non_empty_catch_list
    |   try_block catch_list finally_block
;

try_block:
        T_TRY '{' statement_list '}'
;
    
catch_block:
        T_CATCH '(' expression ')' '{' statement_list '}'
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
        T_FINALLY '{' statement_list '}'
;

function_declaration:
    T_FUNC T_NAME { co_begin_function_declaration(&$1, &$2); } '(' parameter_list ')' compound_statement_with_parentheses { co_end_function_declaration(&$1, &$$); }
;

function_literal:
    T_FUNC { co_begin_function_declaration(&$1, NULL); } '(' parameter_list ')' compound_statement_with_parentheses { co_end_function_declaration(&$1, &$$); }
;

parameter_list:
        non_empty_parameter_list
    |   /* empty */
;

non_empty_parameter_list:
        T_NAME { co_recv_param(&$1); }
    |   T_NAME ',' non_empty_parameter_list { co_recv_param(&$1); }
;

/* TODO merge expression_list with function_call_parameter_list, they are almost the same! */
expression_list:
        non_empty_expression_list
    |   /* empty */
;

non_empty_expression_list:
        expression { co_append_element(&$0, &$1); }
    |   non_empty_expression_list ',' expression { co_append_element(&$0, &$3); }
;

expression_list_with_comma:
        expression ','
    |   expression_list_with_comma expression
;

function_call_parameter_list:
        non_empty_function_call_parameter_list
    |   /* empty */
;

non_empty_function_call_parameter_list:
        expression { co_pass_param(&$1); }
    |   non_empty_function_call_parameter_list ',' expression { co_pass_param(&$3); }
;

function_call:
        T_NAME '(' { co_begin_function_call(&$1); } function_call_parameter_list ')' { co_end_function_call(&$1, &$$); }
;

optional_else:
       /* empty */
    |   T_ELSE statement
;
%%
