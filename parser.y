%{
#include "co.h"
#include "compile.h"
#include "vm_opcodes.h"

#define YYSTYPE cnode
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

#include "scanner.h"
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
    |   T_THROW expression ';'
    |   ';' /* empty */
;

compound_statement:
        T_IF '(' expression ')' { co_if_cond(&$3, &$4); } statement { co_if_after_statement(&$4); } optional_else { co_if_end(&$4); }
    |   T_WHILE '(' expression ')' { co_while_cond(&$3, &$1, &$4); } statement { co_while_end(&$1, &$4); }
    |   '{' statement_list '}'
    |   try_catch_finally_stmt
    |   function_declaration
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
        non_empty_function_call_parameter_list
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
