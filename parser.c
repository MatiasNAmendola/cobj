/* A Bison parser, made by GNU Bison 2.6.2.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.6.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         coparse
#define yylex           colex
#define yyerror         coerror
#define yylval          colval
#define yychar          cochar
#define yydebug         codebug
#define yynerrs         conerrs

/* Copy the first part of user declarations.  */
/* Line 336 of yacc.c  */
#line 1 "parser.y"

#include "cobj.h"

static Node *
return_none_node(struct arena *arena)
{
    Node *n = node_new(arena, NODE_CONST, NULL, NULL);
    n->u.o = CO_None;
    return node_new(arena, NODE_RETURN, n, NULL);
}


/* Line 336 of yacc.c  */
#line 88 "parser.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.h".  */
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
     T_NOT = 285,
     T_IS = 286,
     T_NOT_EQUAL = 287,
     T_EQUAL = 288,
     T_MAPPING = 289,
     T_MOD_ASSIGN = 290,
     T_DIV_ASSIGN = 291,
     T_MUL_ASSIGN = 292,
     T_SUB_ASSIGN = 293,
     T_ADD_ASSIGN = 294,
     T_SR_ASSIGN = 295,
     T_SL_ASSIGN = 296,
     T_GREATER_OR_EQUAL = 297,
     T_SMALLER_OR_EQUAL = 298,
     T_POW = 299,
     T_SL = 300,
     T_SR = 301,
     UNARY_OP = 302,
     NotName = 303,
     NotParen = 304,
     PreferToRightParen = 305
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 350 of yacc.c  */
#line 18 "parser.y"

    Node *node;


/* Line 350 of yacc.c  */
#line 186 "parser.c"
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

/* Copy the second part of user declarations.  */

/* Line 353 of yacc.c  */
#line 213 "parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  61
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   690

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  33
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNRULES -- Number of states.  */
#define YYNSTATES  229

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   305

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    51,     2,     2,
      61,    62,    49,    47,    46,    48,    58,    50,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    68,    64,
      42,    69,    43,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    55,     2,    66,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,     2,    67,    65,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    44,    45,    52,
      53,    54,    57,    59,    60,    63
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    14,    18,    19,
      21,    23,    25,    28,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    49,    55,    59,    63,    67,    71,
      75,    79,    83,    87,    91,    95,    99,   103,   108,   112,
     117,   121,   125,   129,   132,   135,   143,   147,   151,   155,
     160,   165,   169,   176,   181,   184,   185,   187,   190,   192,
     193,   195,   196,   198,   202,   205,   207,   211,   217,   225,
     231,   234,   236,   242,   250,   254,   259,   266,   272,   276,
     280,   284,   288,   292,   296,   300,   302,   304,   306,   309,
     314,   316,   319,   326,   332,   340,   346,   353,   356,   357,
     362,   364,   367,   369,   370,   374,   375,   377,   378,   380,
     384,   386,   390,   391,   394,   396
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      71,     0,    -1,    73,    -1,    91,    -1,    92,    -1,    74,
      77,    -1,    72,    -1,    74,    76,    72,    -1,    -1,    12,
      -1,    64,    -1,    75,    -1,    76,    75,    -1,    -1,    76,
      -1,    12,    -1,    21,    -1,     6,    -1,    29,    -1,    28,
      -1,    25,    -1,    24,    -1,    26,    -1,    27,    -1,    61,
      83,    80,    83,    62,    -1,    80,    47,    80,    -1,    80,
      48,    80,    -1,    80,    49,    80,    -1,    80,    50,    80,
      -1,    80,    51,    80,    -1,    80,    42,    80,    -1,    80,
      43,    80,    -1,    80,    33,    80,    -1,    80,    32,    80,
      -1,    80,    45,    80,    -1,    80,    44,    80,    -1,    80,
      11,    80,    -1,    80,    30,    11,    80,    -1,    80,    31,
      80,    -1,    80,    31,    30,    80,    -1,    80,    53,    80,
      -1,    80,    54,    80,    -1,    80,    52,    80,    -1,    48,
      80,    -1,    65,    80,    -1,    61,    83,    80,    83,    46,
      83,    62,    -1,    61,    88,    62,    -1,    55,    86,    66,
      -1,    56,    89,    67,    -1,    80,    61,    86,    62,    -1,
      80,    55,    80,    66,    -1,    80,    58,    29,    -1,    29,
      68,    29,    61,    86,    62,    -1,    79,    97,    73,    20,
      -1,    46,    83,    -1,    -1,    12,    -1,    12,    12,    -1,
      82,    -1,    -1,    85,    -1,    -1,    80,    -1,    85,    46,
      80,    -1,    87,    81,    -1,    83,    -1,    83,    80,    83,
      -1,    87,    46,    83,    80,    83,    -1,    83,    80,    83,
      46,    83,    80,    83,    -1,    88,    46,    83,    80,    83,
      -1,    90,    81,    -1,    83,    -1,    83,    80,    34,    80,
      83,    -1,    90,    46,    83,    80,    34,    80,    83,    -1,
      29,    69,    80,    -1,     7,    29,    69,    80,    -1,    80,
      55,    80,    66,    69,    80,    -1,    80,    58,    29,    69,
      80,    -1,    29,    39,    80,    -1,    29,    38,    80,    -1,
      29,    37,    80,    -1,    29,    36,    80,    -1,    29,    35,
      80,    -1,    29,    40,    80,    -1,    29,    41,    80,    -1,
      22,    -1,    23,    -1,    17,    -1,    17,    80,    -1,    80,
      61,    86,    62,    -1,     8,    -1,     8,    80,    -1,     3,
      80,    78,    73,   102,    20,    -1,     9,    80,    78,    73,
      20,    -1,    10,    29,    11,    80,    78,    73,    20,    -1,
       6,    29,    97,    73,    20,    -1,    16,    73,    96,   101,
      93,    20,    -1,    19,    73,    -1,    -1,    18,    84,    78,
      73,    -1,    94,    -1,    95,    94,    -1,    95,    -1,    -1,
      61,    98,    62,    -1,    -1,   100,    -1,    -1,    29,    -1,
      29,    69,    80,    -1,    99,    -1,   100,    46,    99,    -1,
      -1,     5,    73,    -1,   101,    -1,     4,    80,    78,    73,
     102,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    94,    94,   104,   105,   109,   113,   114,   121,   125,
     126,   130,   131,   134,   136,   145,   146,   150,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     167,   168,   169,   170,   172,   174,   176,   177,   178,   179,
     181,   182,   183,   184,   186,   188,   192,   196,   200,   204,
     209,   212,   215,   228,   242,   243,   247,   248,   252,   253,
     257,   258,   262,   265,   271,   272,   276,   279,   285,   288,
     294,   295,   299,   302,   308,   309,   310,   317,   324,   329,
     334,   339,   344,   349,   354,   359,   362,   365,   368,   371,
     377,   378,   382,   389,   395,   402,   415,   426,   427,   431,
     439,   440,   444,   445,   449,   450,   454,   455,   459,   460,
     466,   469,   475,   476,   480,   481
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_IF", "T_ELIF", "T_ELSE", "T_FUNC",
  "T_LOCAL", "T_RETURN", "T_WHILE", "T_FOR", "T_IN", "T_NEWLINE",
  "T_WHITESPACE", "T_COMMENT", "T_IGNORED", "T_TRY", "T_THROW", "T_CATCH",
  "T_FINALLY", "T_END", "T_DO", "T_BREAK", "T_CONTINUE", "T_NONE",
  "T_BOOL", "T_NUM", "T_FNUM", "T_STRING", "T_NAME", "T_NOT", "T_IS",
  "T_NOT_EQUAL", "T_EQUAL", "T_MAPPING", "T_MOD_ASSIGN", "T_DIV_ASSIGN",
  "T_MUL_ASSIGN", "T_SUB_ASSIGN", "T_ADD_ASSIGN", "T_SR_ASSIGN",
  "T_SL_ASSIGN", "'<'", "'>'", "T_GREATER_OR_EQUAL", "T_SMALLER_OR_EQUAL",
  "','", "'+'", "'-'", "'*'", "'/'", "'%'", "T_POW", "T_SL", "T_SR", "'['",
  "'{'", "UNARY_OP", "'.'", "NotName", "NotParen", "'('", "')'",
  "PreferToRightParen", "';'", "'~'", "']'", "'}'", "':'", "'='",
  "$accept", "start", "stmt", "stmt_list", "open_stmt_list", "stmt_term",
  "stmt_terms", "opt_stmt_terms", "do_or_newline", "funcliteral", "expr",
  "opt_comma", "newlines", "opt_newlines", "expr_list_inline",
  "non_empty_expr_list_inline", "expr_list", "non_empty_expr_list",
  "expr_list_morethanone", "assoc_list", "non_empty_assoc_list",
  "simple_stmt", "compound_stmt", "opt_finally_block", "catch_block",
  "catch_list", "opt_catch_list", "opt_param_list", "param_list", "param",
  "non_empty_param_list", "opt_else", "if_tail", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,    60,    62,   297,   298,    44,    43,    45,    42,
      47,    37,   299,   300,   301,    91,   123,   302,    46,   303,
     304,    40,    41,   305,    59,   126,    93,   125,    58,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    71,    72,    72,    73,    74,    74,    74,    75,
      75,    76,    76,    77,    77,    78,    78,    79,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    81,    81,    82,    82,    83,    83,
      84,    84,    85,    85,    86,    86,    87,    87,    88,    88,
      89,    89,    90,    90,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    92,    92,    92,    92,    92,    93,    93,    94,
      95,    95,    96,    96,    97,    97,    98,    98,    99,    99,
     100,   100,   101,   101,   102,   102
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     3,     0,     1,
       1,     1,     2,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     4,     3,     4,
       3,     3,     3,     2,     2,     7,     3,     3,     3,     4,
       4,     3,     6,     4,     2,     0,     1,     2,     1,     0,
       1,     0,     1,     3,     2,     1,     3,     5,     7,     5,
       2,     1,     5,     7,     3,     4,     6,     5,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     2,     4,
       1,     2,     6,     5,     7,     5,     6,     2,     0,     4,
       1,     2,     1,     0,     3,     0,     1,     0,     1,     3,
       1,     3,     0,     2,     1,     5
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       8,     0,    17,     0,    90,     0,     0,     8,    87,    85,
      86,    21,    20,    22,    23,    19,    18,     0,    59,    59,
      59,     0,     0,     6,     2,    13,   105,     0,     3,     4,
      17,    18,     0,   105,     0,    91,     0,     0,   103,    88,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      56,    58,    65,     0,    55,    71,     0,    55,     0,     0,
      44,     1,     9,    10,    11,    14,     5,   107,     8,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    59,    15,
      16,     0,     0,    59,     8,     8,     0,     8,     0,    61,
     100,   102,   112,    82,    81,    80,    79,    78,    83,    84,
       0,    74,    57,    59,    47,    59,    64,     0,    48,    59,
      70,    59,    59,    46,     7,    12,   108,     0,   110,   106,
       0,    36,     0,     0,    38,    33,    32,    30,    31,    35,
      34,    25,    26,    27,    28,    29,    42,    40,    41,     0,
      51,     0,     0,    51,     0,   112,     0,    75,     0,     0,
      62,     0,    60,   101,     8,    98,    59,    66,    54,     0,
      54,     0,     0,     0,   104,     0,    53,    37,    39,    50,
       0,    49,    50,    49,     0,   114,     0,    95,    93,     8,
       8,     0,   113,     8,     0,     0,    59,    59,     0,    59,
      24,    59,   109,   111,     0,    77,     0,    92,     0,    99,
      63,    97,    96,    52,    67,    72,     0,     0,    69,    76,
       8,    94,    59,    45,    59,   112,    73,    68,   115
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    22,    23,    24,    25,    64,    65,    66,    94,    26,
      27,   116,    51,    52,   161,   162,    53,    54,    59,    56,
      57,    28,    29,   194,   100,   101,   102,    68,   127,   128,
     129,   185,   186
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -76
static const yytype_int16 yypact[] =
{
     277,   132,   -12,    -8,   132,   132,    -6,   277,   132,   -76,
     -76,   -76,   -76,   -76,   -76,   -76,    66,   132,     7,     7,
       7,   132,    35,   -76,   -76,   -11,   -25,   559,   -76,   -76,
     -76,   -30,   409,   -25,   -32,   594,   409,    37,    28,   594,
     132,   132,   132,   132,   132,   132,   132,    20,   132,   -49,
      38,   -76,   132,    -4,    10,   132,    -3,    17,   132,   -40,
     -49,   -76,   -76,   -76,   -76,   217,   -76,    58,   277,   132,
      55,     4,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,    59,     7,   -76,
     -76,   132,    60,     7,   277,   277,   132,   277,   132,   132,
     -76,    28,    62,   594,   594,   594,   594,   594,   594,   594,
      31,   594,   -76,   454,   -76,     7,   -76,   489,   -76,     7,
     -76,   454,     7,   -76,   -76,   -76,    24,    32,   -76,    50,
      79,   629,   132,   132,   629,   629,   629,   205,   205,   205,
     205,    93,    93,   216,   216,   216,     0,     0,     0,   316,
      40,    49,   361,   -76,    56,    22,    80,   594,    99,   409,
     594,   -10,    74,   -76,   277,   103,     7,   -76,   132,   132,
     132,   -38,   132,   132,   -76,    58,   -76,   629,   629,    54,
     132,   121,   -76,   -76,   132,   -76,   104,   -76,   -76,   277,
     277,   132,   -76,   277,   107,    67,   454,   454,   524,     7,
     -76,   454,   594,   -76,   132,   594,   409,   -76,   116,   -76,
     594,   -76,   -76,   -76,   -76,   -76,   132,   289,   -76,   594,
     277,   -76,   454,   -76,   454,    22,   -76,   -76,   -76
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -76,   -76,    72,    18,   -76,    84,   -76,   -76,   -31,   -76,
      -1,    95,   -76,    -5,   -76,   -76,   -75,   -76,   -76,   -76,
     -76,   -76,   -76,   -76,    52,   -76,   -76,   117,   -76,   -20,
     -76,    61,   -63
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -90
static const yytype_int16 yytable[] =
{
      32,    62,    89,    35,    36,    97,   122,    39,   199,    92,
      30,    90,    93,   151,    55,    58,    49,    33,   154,    50,
      60,    34,   123,    37,   200,    38,   184,   164,    11,    12,
      13,    14,    15,    31,   133,    61,    67,    96,    47,   103,
     104,   105,   106,   107,   108,   109,    99,   111,    98,   110,
     112,   113,    17,    63,   117,    91,   115,   121,    92,    18,
      19,    93,   114,   119,   118,    20,   132,   164,   131,    21,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   130,   126,   150,   153,
     152,   195,   166,   173,   174,   157,   175,   159,   160,   176,
     187,    40,    41,    42,    43,    44,    45,    46,   167,   180,
     168,   181,   155,   156,   170,   158,   171,   172,   183,   188,
     191,   -89,   193,   204,   207,   -89,   -89,   212,   189,   213,
     190,   177,   178,   -89,    47,    48,   221,   124,    30,   -89,
     -89,   -89,    80,    81,    82,    83,    84,    85,    91,   125,
      95,    92,   120,   163,    93,   203,    11,    12,    13,    14,
      15,    31,   228,   165,     0,     0,     0,   196,   197,   198,
       0,   201,   202,     0,     0,   220,     0,     0,     0,   205,
      17,     0,   192,   206,     0,   -89,     0,    18,    19,     0,
     210,   214,   215,    20,   217,     0,   218,    21,     0,     0,
       0,     0,     0,   219,     0,     0,     0,   208,   209,     0,
       0,   211,     0,     0,     0,   222,   224,   226,     0,   227,
       1,     0,     0,     2,     3,     4,     5,     6,     0,    62,
       0,     0,     0,     7,     8,     0,     0,     0,   225,     9,
      10,    11,    12,    13,    14,    15,    16,   -90,   -90,   -90,
     -90,     0,    78,    79,    80,    81,    82,    83,    84,    85,
      91,     0,     0,    92,     0,    17,    93,     0,    83,    84,
      85,    91,    18,    19,    92,     0,     0,    93,    20,     0,
       1,    63,    21,     2,     3,     4,     5,     6,     0,     0,
       0,     0,     0,     7,     8,    30,     0,     0,     0,     9,
      10,    11,    12,    13,    14,    15,    16,     0,     0,     0,
       0,     0,     0,    11,    12,    13,    14,    15,    31,     0,
       0,     0,     0,     0,     0,    17,     0,    69,     0,     0,
       0,     0,    18,    19,     0,     0,     0,    17,    20,     0,
       0,     0,    21,     0,    18,    19,    70,    71,    72,    73,
      20,   223,     0,     0,    21,     0,     0,     0,    74,    75,
      76,    77,     0,    78,    79,    80,    81,    82,    83,    84,
      85,    91,    69,     0,    92,     0,     0,    93,     0,     0,
       0,     0,   179,     0,     0,     0,     0,     0,     0,     0,
       0,    70,    71,    72,    73,     0,     0,     0,     0,     0,
       0,     0,     0,    74,    75,    76,    77,     0,    78,    79,
      80,    81,    82,    83,    84,    85,    91,     0,     0,    92,
      69,    89,    93,     0,     0,     0,     0,   182,     0,     0,
      90,     0,     0,     0,     0,     0,     0,     0,     0,    70,
      71,    72,    73,     0,     0,     0,     0,     0,     0,     0,
       0,    74,    75,    76,    77,     0,    78,    79,    80,    81,
      82,    83,    84,    85,    91,    69,    50,    92,     0,     0,
      93,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    70,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,    74,    75,    76,    77,
      69,    78,    79,    80,    81,    82,    83,    84,    85,    91,
       0,     0,    92,     0,     0,    93,     0,     0,     0,    70,
      71,    72,    73,   169,     0,     0,     0,     0,     0,     0,
       0,    74,    75,    76,    77,    69,    78,    79,    80,    81,
      82,    83,    84,    85,    91,     0,     0,    92,     0,     0,
      93,     0,     0,     0,    70,    71,    72,    73,   216,     0,
       0,     0,     0,     0,     0,     0,    74,    75,    76,    77,
      69,    78,    79,    80,    81,    82,    83,    84,    85,    91,
       0,     0,    92,     0,     0,    93,     0,     0,     0,    70,
      71,    72,    73,     0,     0,     0,     0,     0,     0,     0,
       0,    74,    75,    76,    77,    69,    78,    79,    80,    81,
      82,    83,    84,    85,    86,     0,     0,    87,     0,     0,
      88,     0,     0,     0,    70,    71,    72,    73,     0,     0,
       0,     0,     0,     0,     0,     0,    74,    75,    76,    77,
     -90,    78,    79,    80,    81,    82,    83,    84,    85,    91,
       0,     0,    92,     0,     0,    93,     0,     0,     0,   -90,
     -90,   -90,   -90,     0,     0,     0,     0,     0,     0,     0,
       0,    74,    75,    76,    77,     0,    78,    79,    80,    81,
      82,    83,    84,    85,    91,     0,     0,    92,     0,     0,
      93
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-76))

#define yytable_value_is_error(yytable_value) \
  ((yytable_value) == (-90))

static const yytype_int16 yycheck[] =
{
       1,    12,    12,     4,     5,    36,    46,     8,    46,    58,
       6,    21,    61,    88,    19,    20,    17,    29,    93,    12,
      21,    29,    62,    29,    62,     7,     4,     5,    24,    25,
      26,    27,    28,    29,    30,     0,    61,    69,    68,    40,
      41,    42,    43,    44,    45,    46,    18,    48,    11,    29,
      12,    52,    48,    64,    55,    55,    46,    58,    58,    55,
      56,    61,    66,    46,    67,    61,    11,     5,    69,    65,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    68,    29,    29,    29,
      91,   166,    61,    69,    62,    96,    46,    98,    99,    20,
      20,    35,    36,    37,    38,    39,    40,    41,   113,    69,
     115,    62,    94,    95,   119,    97,   121,   122,    62,    20,
      46,     0,    19,    69,    20,     4,     5,    20,   159,    62,
     161,   132,   133,    12,    68,    69,    20,    65,     6,    18,
      19,    20,    49,    50,    51,    52,    53,    54,    55,    65,
      33,    58,    57,   101,    61,   175,    24,    25,    26,    27,
      28,    29,   225,   102,    -1,    -1,    -1,   168,   169,   170,
      -1,   172,   173,    -1,    -1,   206,    -1,    -1,    -1,   180,
      48,    -1,   164,   184,    -1,    64,    -1,    55,    56,    -1,
     191,   196,   197,    61,   199,    -1,   201,    65,    -1,    -1,
      -1,    -1,    -1,   204,    -1,    -1,    -1,   189,   190,    -1,
      -1,   193,    -1,    -1,    -1,   216,   217,   222,    -1,   224,
       3,    -1,    -1,     6,     7,     8,     9,    10,    -1,    12,
      -1,    -1,    -1,    16,    17,    -1,    -1,    -1,   220,    22,
      23,    24,    25,    26,    27,    28,    29,    42,    43,    44,
      45,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    58,    -1,    48,    61,    -1,    52,    53,
      54,    55,    55,    56,    58,    -1,    -1,    61,    61,    -1,
       3,    64,    65,     6,     7,     8,     9,    10,    -1,    -1,
      -1,    -1,    -1,    16,    17,     6,    -1,    -1,    -1,    22,
      23,    24,    25,    26,    27,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    24,    25,    26,    27,    28,    29,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    -1,    11,    -1,    -1,
      -1,    -1,    55,    56,    -1,    -1,    -1,    48,    61,    -1,
      -1,    -1,    65,    -1,    55,    56,    30,    31,    32,    33,
      61,    62,    -1,    -1,    65,    -1,    -1,    -1,    42,    43,
      44,    45,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    11,    -1,    58,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    30,    31,    32,    33,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    42,    43,    44,    45,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      11,    12,    61,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      31,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    43,    44,    45,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    11,    12,    58,    -1,    -1,
      61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    31,    32,    33,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      11,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    30,
      31,    32,    33,    34,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    43,    44,    45,    11,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      61,    -1,    -1,    -1,    30,    31,    32,    33,    34,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      11,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    30,
      31,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    43,    44,    45,    11,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      61,    -1,    -1,    -1,    30,    31,    32,    33,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    43,    44,    45,
      11,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    30,
      31,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    43,    44,    45,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    -1,    -1,
      61
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,     7,     8,     9,    10,    16,    17,    22,
      23,    24,    25,    26,    27,    28,    29,    48,    55,    56,
      61,    65,    71,    72,    73,    74,    79,    80,    91,    92,
       6,    29,    80,    29,    29,    80,    80,    29,    73,    80,
      35,    36,    37,    38,    39,    40,    41,    68,    69,    80,
      12,    82,    83,    86,    87,    83,    89,    90,    83,    88,
      80,     0,    12,    64,    75,    76,    77,    61,    97,    11,
      30,    31,    32,    33,    42,    43,    44,    45,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    58,    61,    12,
      21,    55,    58,    61,    78,    97,    69,    78,    11,    18,
      94,    95,    96,    80,    80,    80,    80,    80,    80,    80,
      29,    80,    12,    80,    66,    46,    81,    80,    67,    46,
      81,    80,    46,    62,    72,    75,    29,    98,    99,   100,
      73,    80,    11,    30,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      29,    86,    80,    29,    86,    73,    73,    80,    73,    80,
      80,    84,    85,    94,     5,   101,    61,    83,    83,    34,
      83,    83,    83,    69,    62,    46,    20,    80,    80,    66,
      69,    62,    66,    62,     4,   101,   102,    20,    20,    78,
      78,    46,    73,    19,    93,    86,    80,    80,    80,    46,
      62,    80,    80,    99,    69,    80,    80,    20,    73,    73,
      80,    73,    20,    62,    83,    83,    34,    83,    83,    80,
      78,    20,    80,    62,    80,    73,    83,    83,   102
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (c, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])



/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, c); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, struct compiler *c)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, c)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    struct compiler *c;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (c);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, struct compiler *c)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, c)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    struct compiler *c;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, c);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, struct compiler *c)
#else
static void
yy_reduce_print (yyvsp, yyrule, c)
    YYSTYPE *yyvsp;
    int yyrule;
    struct compiler *c;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , c);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, c); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, struct compiler *c)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, c)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    struct compiler *c;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (c);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (struct compiler *c)
#else
int
yyparse (c)
    struct compiler *c;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 94 "parser.y"
    {
        if ((yyval.node)) {
            c->xtop = node_listconcat((yyval.node), node_list(c->arena, return_none_node(c->arena), NULL));
        } else {
            c->xtop = node_list(c->arena, return_none_node(c->arena), NULL);
        }
    }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 104 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 105 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 109 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 113 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 114 "parser.y"
    {
            if ((yyvsp[(1) - (3)].node)) {
                (yyval.node) = node_listconcat((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
            } else {
                (yyval.node) = (yyvsp[(3) - (3)].node);
            }
        }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 121 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 160 "parser.y"
    { (yyval.node) = (yyvsp[(3) - (5)].node); }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 161 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_ADD; }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 162 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_SUB; }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 163 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_MUL; }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 164 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_DIV; }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 165 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_MOD; }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 167 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.oparg = Cmp_LT; }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 168 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.oparg = Cmp_GT; }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 169 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.oparg = Cmp_EQ; }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 170 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.oparg =
    Cmp_NE; }
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 172 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
    (yyval.node)->u.oparg = Cmp_LE; }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 174 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
    (yyval.node)->u.oparg = Cmp_GE; }
    break;

  case 36:
/* Line 1787 of yacc.c  */
#line 176 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.oparg = Cmp_IN; }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 177 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node)); (yyval.node)->u.oparg = Cmp_NOT_IN; }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 178 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.oparg = Cmp_IS; }
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 179 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_CMP, (yyvsp[(1) - (4)].node), (yyvsp[(4) - (4)].node)); (yyval.node)->u.oparg = Cmp_IS_NOT; }
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 181 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_SL; }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 182 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_SR; }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 183 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_POW; }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 184 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_UNARY, (yyvsp[(2) - (2)].node), NULL); (yyval.node)->u.op =
    OP_UNARY_NEGATE; }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 186 "parser.y"
    { (yyval.node) = node_new(c->arena, NODE_UNARY, (yyvsp[(2) - (2)].node), NULL); (yyval.node)->u.op =
    OP_UNARY_INVERT; }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 188 "parser.y"
    { 
            (yyval.node) = node_new(c->arena, NODE_TUPLE, NULL, NULL);
            (yyval.node)->nd_list = node_list(c->arena, (yyvsp[(3) - (7)].node), NULL);
        }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 192 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_TUPLE, NULL, NULL);
            (yyval.node)->nd_list = (yyvsp[(2) - (3)].node);
        }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 196 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_LIST, NULL, NULL);
            (yyval.node)->nd_list = (yyvsp[(2) - (3)].node);
        }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 200 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_DICT_BUILD, NULL, NULL);
            (yyval.node)->nd_list = (yyvsp[(2) - (3)].node);
        }
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 204 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_FUNC_CALL, NULL, NULL);
            (yyval.node)->nd_func = (yyvsp[(1) - (4)].node);
            (yyval.node)->nd_params = (yyvsp[(3) - (4)].node);
        }
    break;

  case 50:
/* Line 1787 of yacc.c  */
#line 209 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_LOAD_SUBSCRIPT, (yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); (yyval.node)->u.op = OP_BINARY_SUBSCRIPT;
        }
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 212 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_LOAD_DOTSUBSCRIPT, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node)->u.op = OP_BINARY_SUBSCRIPT;
        }
    break;

  case 52:
/* Line 1787 of yacc.c  */
#line 215 "parser.y"
    {
            Node *f = node_new(c->arena, NODE_LOAD_DOTSUBSCRIPT, (yyvsp[(1) - (6)].node), (yyvsp[(3) - (6)].node)); 
            f->u.op = OP_BINARY_SUBSCRIPT;
            (yyval.node) = node_new(c->arena, NODE_FUNC_CALL, NULL, NULL);
            (yyval.node)->nd_func = f;
            Node *p;
            if ((yyvsp[(5) - (6)].node)) {
                p = node_listprepend(c->arena, (yyvsp[(5) - (6)].node), (yyvsp[(1) - (6)].node));
            } else {
                p = (yyvsp[(1) - (6)].node);
            }
            (yyval.node)->nd_params = p;
        }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 228 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_FUNC, NULL, NULL);
            t->nd_funcargs = (yyvsp[(2) - (4)].node);
            t->nd_funcbody = (yyvsp[(3) - (4)].node);
            if (t->nd_funcbody) {
                t->nd_funcbody = node_listappend(c->arena, t->nd_funcbody, return_none_node(c->arena));
            } else {
                t->nd_funcbody = node_list(c->arena, return_none_node(c->arena), NULL);
            }
            (yyval.node) = t;
        }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 258 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 262 "parser.y"
    {
            (yyval.node) = node_list(c->arena, (yyvsp[(1) - (1)].node), NULL);
        }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 265 "parser.y"
    {
            (yyval.node) = node_listappend(c->arena, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
        }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 271 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (2)].node); }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 272 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 276 "parser.y"
    {
            (yyval.node) = node_list(c->arena, (yyvsp[(2) - (3)].node), NULL);
        }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 279 "parser.y"
    {
            (yyval.node) = node_listappend(c->arena, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
        }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 285 "parser.y"
    {
            (yyval.node) = node_list(c->arena, (yyvsp[(2) - (7)].node), (yyvsp[(6) - (7)].node), NULL);
        }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 288 "parser.y"
    {
            (yyval.node) = node_listappend(c->arena, (yyvsp[(1) - (5)].node), (yyvsp[(4) - (5)].node));
        }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 295 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 299 "parser.y"
    {
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_DICT_ADD, (yyvsp[(2) - (5)].node), (yyvsp[(4) - (5)].node)), NULL);
        }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 302 "parser.y"
    {
            (yyval.node) = node_listappend(c->arena, (yyvsp[(1) - (7)].node), node_new(c->arena, NODE_DICT_ADD, (yyvsp[(4) - (7)].node), (yyvsp[(6) - (7)].node)));
        }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 308 "parser.y"
    { Node *t = node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); (yyval.node) = node_list(c->arena, t, NULL); }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 309 "parser.y"
    { Node *t = node_new(c->arena, NODE_ASSIGN_LOCAL, (yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].node)); (yyval.node) = node_list(c->arena, t, NULL); }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 310 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_STORE_SUBSCRIPT, NULL, NULL);
            t->nd_left = (yyvsp[(1) - (6)].node);
            t->nd_middle = (yyvsp[(3) - (6)].node);
            t->nd_right = (yyvsp[(6) - (6)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 317 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_STORE_DOTSUBSCRIPT, NULL, NULL);
            t->nd_left = (yyvsp[(1) - (5)].node);
            t->nd_middle = (yyvsp[(3) - (5)].node);
            t->nd_right = (yyvsp[(5) - (5)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 324 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_ADD;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 329 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_SUB;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 334 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_MUL;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 339 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_DIV;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 344 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_MOD;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 349 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_SR;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 354 "parser.y"
    {
            Node *t;
            t = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); t->u.op = OP_BINARY_SL;
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(1) - (3)].node), t), NULL);
        }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 359 "parser.y"
    {
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_BREAK, 0, 0), NULL);
        }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 362 "parser.y"
    {
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_CONTINUE, 0, 0), NULL);
        }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 365 "parser.y"
    {
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_THROW, 0, 0), NULL);
        }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 368 "parser.y"
    {
            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_THROW, (yyvsp[(2) - (2)].node), 0), NULL);
        }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 371 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_FUNC_CALL_STMT, NULL, NULL);
            (yyval.node)->nd_func = (yyvsp[(1) - (4)].node);
            (yyval.node)->nd_params = (yyvsp[(3) - (4)].node);
            (yyval.node) = node_list(c->arena, (yyval.node), NULL); 
        }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 377 "parser.y"
    { (yyval.node) = node_list(c->arena, return_none_node(c->arena), NULL); }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 378 "parser.y"
    { (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_RETURN, (yyvsp[(2) - (2)].node), NULL), NULL); }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 382 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_IF, NULL, NULL);
            t->nd_cond = (yyvsp[(2) - (6)].node);
            t->nd_condbody = (yyvsp[(4) - (6)].node);
            t->nd_condelse = (yyvsp[(5) - (6)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 389 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_WHILE, NULL, NULL);
            t->nd_cond = (yyvsp[(2) - (5)].node);
            t->nd_condbody = (yyvsp[(4) - (5)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 395 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_FOR, NULL, NULL);
            t->nd_foritem = (yyvsp[(2) - (7)].node);
            t->nd_forlist = (yyvsp[(4) - (7)].node);
            t->nd_forbody = (yyvsp[(6) - (7)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 402 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_FUNC, NULL, NULL);
            t->nd_funcargs = (yyvsp[(3) - (5)].node);
            t->nd_funcbody = (yyvsp[(4) - (5)].node);
            if (t->nd_funcbody) {
                t->nd_funcbody = node_listappend(c->arena, t->nd_funcbody, return_none_node(c->arena));
            } else {
                t->nd_funcbody = node_list(c->arena, return_none_node(c->arena),
                NULL);
            }

            (yyval.node) = node_list(c->arena, node_new(c->arena, NODE_ASSIGN, (yyvsp[(2) - (5)].node), t), NULL);
        }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 415 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_TRY, NULL, NULL);
            t->nd_trybody = (yyvsp[(2) - (6)].node);
            t->nd_catches = (yyvsp[(3) - (6)].node);
            t->nd_orelse = (yyvsp[(4) - (6)].node);
            t->nd_finally = (yyvsp[(5) - (6)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 426 "parser.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 427 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 431 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_CATCH, NULL, NULL);
            (yyval.node)->nd_catchname = (yyvsp[(2) - (4)].node);
            (yyval.node)->nd_catchbody = (yyvsp[(4) - (4)].node);
        }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 439 "parser.y"
    { (yyval.node) = node_list(c->arena, (yyvsp[(1) - (1)].node), NULL); }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 440 "parser.y"
    { (yyval.node) = node_listappend(c->arena, (yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].node)); }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 445 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 449 "parser.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 450 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 455 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 460 "parser.y"
    {
            (yyval.node) = node_new(c->arena, NODE_BIN, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
        }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 466 "parser.y"
    {
            (yyval.node) = node_list(c->arena, (yyvsp[(1) - (1)].node), NULL);
        }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 469 "parser.y"
    {
            (yyval.node) = node_listappend(c->arena, (yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
        }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 475 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 113:
/* Line 1787 of yacc.c  */
#line 476 "parser.y"
    { (yyval.node) = (yyvsp[(2) - (2)].node); }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 481 "parser.y"
    {
            Node *t = node_new(c->arena, NODE_IF, NULL, NULL);
            t->nd_cond = (yyvsp[(2) - (5)].node);
            t->nd_condbody = (yyvsp[(4) - (5)].node);
            t->nd_condelse = (yyvsp[(5) - (5)].node);
            (yyval.node) = node_list(c->arena, t, NULL);
        }
    break;


/* Line 1787 of yacc.c  */
#line 2440 "parser.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (c, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (c, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, c);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, c);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (c, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, c);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, c);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2048 of yacc.c  */
#line 490 "parser.y"

