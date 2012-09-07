/* A Bison parser, made by GNU Bison 2.6.2.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef CO_PARSER_H
# define CO_PARSER_H
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int codebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_IF = 258,
     T_ELIF = 259,
     T_ELSE = 260,
     T_FUNC = 261,
     T_LOCAL = 262,
     T_RETURN = 263,
     T_WHILE = 264,
     T_FOR = 265,
     T_IN = 266,
     T_NEWLINE = 267,
     T_WHITESPACE = 268,
     T_COMMENT = 269,
     T_IGNORED = 270,
     T_TRY = 271,
     T_THROW = 272,
     T_CATCH = 273,
     T_FINALLY = 274,
     T_END = 275,
     T_DO = 276,
     T_BREAK = 277,
     T_CONTINUE = 278,
     T_NONE = 279,
     T_BOOL = 280,
     T_NUM = 281,
     T_FNUM = 282,
     T_STRING = 283,
     T_NAME = 284,
     T_NOT_EQUAL = 285,
     T_EQUAL = 286,
     T_MAPPING = 287,
     T_MOD_ASSIGN = 288,
     T_DIV_ASSIGN = 289,
     T_MUL_ASSIGN = 290,
     T_SUB_ASSIGN = 291,
     T_ADD_ASSIGN = 292,
     T_SR_ASSIGN = 293,
     T_SL_ASSIGN = 294,
     T_GREATER_OR_EQUAL = 295,
     T_SMALLER_OR_EQUAL = 296,
     T_POW = 297,
     T_SL = 298,
     T_SR = 299,
     UNARY_OP = 300,
     NotName = 301,
     NotParen = 302,
     PreferToRightParen = 303
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2049 of yacc.c  */
#line 18 "parser.y"

    Node *node;


/* Line 2049 of yacc.c  */
#line 110 "parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int coparse (void *YYPARSE_PARAM);
#else
int coparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int coparse (struct compiler *c);
#else
int coparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !CO_PARSER_H  */
