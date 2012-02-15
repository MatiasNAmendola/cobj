%{

#include "co.h"
#include "co_compile.h"
#include "co_vm_opcodes.h"

#define YYERROR_VERBOSE
#define YYSTYPE cnode
#define YYPARSE_PARAM yyscanner
#define YYLEX_PARAM yyscanner

#include "co_scanner.h"
%}

%pure_parser
/*
 * Dangling else statment cause one shift/reduce conflict
 */
%expect 1 

%left   '<' '>'
%left   ','
%left   '+' '-'
%left   '*' '/' '%'
%right  T_PRINT
%token  T_PRINT "print (T_PRINT)"
%token  T_NONE
%token  T_BOOL
%token  T_NUM
%token  T_FNUM
%token  T_STRING
%token  T_NAME
%token  T_IF
%token  T_ELSE
%token  T_FUNC
%token  T_WHILE "while (T_WHILE)"

%% /* Grammar Rules */

start:
        statement_list { co_end_compilation(); }
    |   /* empty */
;

expression: /* express something */
        T_NAME      { $$ = $1; }
    |   T_STRING    { $$ = $1; }
    |   T_BOOL      { $$ = $1; }
    |   T_NONE      { $$ = $1; }
    |   T_NUM       { $$ = $1; }
    |   T_FNUM      { $$ = $1; }
    |   '(' expression ')' { $$ = $2; }
    |   expression '<' expression { co_binary_op(OP_IS_SMALLER, &$$, &$1, &$3); }
    |   expression '>' expression { co_binary_op(OP_IS_SMALLER, &$$, &$3, &$1); }
    |   expression '+' expression { co_binary_op(OP_ADD, &$$, &$1, &$3); }
    |   expression '-' expression { co_binary_op(OP_SUB, &$$, &$1, &$3); }
    |   expression '*' expression { co_binary_op(OP_MUL, &$$, &$1, &$3); }
    |   expression '/' expression { co_binary_op(OP_DIV, &$$, &$1, &$3); }
    |   expression '%' expression { co_binary_op(OP_MOD, &$$, &$1, &$3); }
    |   function_call
;

statement: /* state something */
        simple_statement
    |   compound_statement
;

simple_statement:
        T_NAME '='  expression ';' { co_assign(&$$, &$1, &$3); }
    |   T_PRINT expression ';' { co_print(&$2); }
    |   expression ';'
    |   ';' /* empty */
;

compound_statement:
        T_IF '(' expression ')' { co_if_cond(&$3, &$4); } statement { co_if_after_statement(&$4); } optional_else { co_if_end(&$4); }
    |   T_WHILE '(' expression ')' { co_while_cond(&$3, &$1, &$4); } statement { co_while_end(&$1, &$4); }
    |   '{' statement_list '}'
    |   function_declaration
;

function_declaration:
    T_FUNC T_NAME { co_begin_function_declaration(&$2); } '(' parameter_list ')' '{' statement_list '}' { co_end_function_declaration(&$2); }
;

parameter_list:
        non_empty_parameter_list
    |   /* empty */
;

non_empty_parameter_list:
        T_NAME { co_recv_param(&$1); }
    |   T_NAME ',' non_empty_parameter_list
;

function_call_parameter_list:
        non_empty_function_call_parameter_list { $$ = $1; }
    |   /* empty */
;

non_empty_function_call_parameter_list:
        expression { co_pass_param(&$1); }
    |   expression ',' non_empty_parameter_list
;

function_call:
        T_NAME '(' { co_begin_function_call(&$1); } function_call_parameter_list ')' { co_end_function_call(&$1, &$$); }
;

optional_else:
        /* empty */
   |   T_ELSE statement
;

statement_list:
        statement
    |   statement_list statement
;

%%
