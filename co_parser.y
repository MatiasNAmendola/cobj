%{

#include <stdio.h>
#include "co.h"
#include "co_compile.h"
#include "co_vm_opcodes.h"

#define yyerror coerror
#define yyparse coparse
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

%left   ','
%left   '+' '-'
%left   '*' '/' '%'
%token  T_NUM
%token  T_FNUM
%token  T_STRING
%token  T_NAME
%token  T_IF
%token  T_ELSE
%token  T_FUNC

%% /* Grammar Rules */

start:
        statement_list { co_end_compilation(); }
    |   /* empty */
;

expression: /* express something */
        T_NAME      { $$ = $1; }
    |   T_STRING    { $$ = $1; }
    |   T_NUM       { $$ = $1; }
    |   T_FNUM      { $$ = $1; }
    |   '(' expression ')' { $$ = $2; }
    |   expression '+' expression { co_binary_op(OP_ADD, &$$, &$1, &$3); }
    |   expression '-' expression { co_binary_op(OP_SUB, &$$, &$1, &$3); }
    |   expression '*' expression { co_binary_op(OP_MUL, &$$, &$1, &$3); }
    |   expression '/' expression { co_binary_op(OP_DIV, &$$, &$1, &$3); }
    |   expression '%' expression { co_binary_op(OP_MOD, &$$, &$1, &$3); }
;

statement: /* state something */
        simple_statement
    |   compound_statement
;

simple_statement:
        T_NAME '='  expression ';' { co_assign(&$$, &$1, &$3); }
    |   '=' expression ';' { co_print(&$2); }
    |   ';' /* empty */
;

compound_statement:
        T_IF '(' expression ')' { co_if_cond(&$3, &$4); } statement { co_if_after_statement(&$4); } optional_else { co_if_end(&$4); }
    |   '{' statement_list '}'
;

function_declaration:
    T_FUNC T_NAME '(' ')' '{' statement '}'
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
