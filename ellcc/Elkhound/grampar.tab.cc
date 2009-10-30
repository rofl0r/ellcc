
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 6 "grampar.y"


#include "grampar.h"        // yylex, etc.
#include "gramast.ast.gen.h"// grammar syntax AST definition
#include "gramlex.h"        // GrammarLexer
#include "owner.h"          // Owner

#include <stdlib.h>         // malloc, free
#include <iostream>         // cout

// enable debugging the parser
#ifndef NDEBUG
  #define YYDEBUG 1
#endif

// name of extra parameter to yylex
#define YYLEX_PARAM parseParam

// make it call my yylex
#define yylex(lv, param) grampar_yylex(lv, param)

// Bison calls yyerror(msg) on error; we need the extra
// parameter too, so the macro shoehorns it in there
#define yyerror(msg) grampar_yyerror(msg, YYPARSE_PARAM)

// rename the externally-visible parsing routine to make it
// specific to this instance, so multiple bison-generated
// parsers can coexist
#define yyparse grampar_yyparse


// grab the parameter
#define PARAM ((ParseParams*)parseParam)

// return a locstring for 'str' with no location information
#define noloc(str)                                                    \
  new LocString(SL_UNKNOWN,      /* unknown location */         \
                PARAM->lexer.strtable.add(str))
                
// locstring for NULL, with no location
#define nolocNULL()                                                   \
  new LocString(SL_UNKNOWN, NULL)

// return a locstring with same location info as something else
// (passed as a pointer to a SourceLocationation)
#define sameloc(otherLoc, str)                                        \
  new LocString(otherLoc->loc, PARAM->lexer.strtable.add(str))

// interpret the word into an associativity kind specification
AssocKind whichKind(LocString * /*owner*/ kind);



/* Line 189 of yacc.c  */
#line 127 "grampar.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_INTEGER = 258,
     TOK_NAME = 259,
     TOK_STRING = 260,
     TOK_LIT_CODE = 261,
     TOK_LBRACE = 262,
     TOK_RBRACE = 263,
     TOK_COLON = 264,
     TOK_SEMICOLON = 265,
     TOK_ARROW = 266,
     TOK_LPAREN = 267,
     TOK_RPAREN = 268,
     TOK_COMMA = 269,
     TOK_TERMINALS = 270,
     TOK_TOKEN = 271,
     TOK_NONTERM = 272,
     TOK_FUN = 273,
     TOK_VERBATIM = 274,
     TOK_IMPL_VERBATIM = 275,
     TOK_PRECEDENCE = 276,
     TOK_OPTION = 277,
     TOK_EXPECT = 278,
     TOK_CONTEXT_CLASS = 279,
     TOK_SUBSETS = 280,
     TOK_DELETE = 281,
     TOK_REPLACE = 282,
     TOK_FORBID_NEXT = 283
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 105 "grampar.y"

  int num;
  LocString *str;
  char loc[sizeof(SourceLocation)];

  ASTList<TopForm> *topFormList;
  TopForm *topForm;

  ASTList<TermDecl> *termDecls;
  TermDecl *termDecl;
  ASTList<TermType> *termTypes;
  TermType *termType;
  ASTList<PrecSpec> *precSpecs;

  ASTList<SpecFunc> *specFuncs;
  SpecFunc *specFunc;
  ASTList<LocString> *stringList;

  ASTList<ProdDecl> *prodDecls;
  ProdDecl *prodDecl;
  ASTList<RHSElt> *rhsList;
  RHSElt *rhsElt;



/* Line 214 of yacc.c  */
#line 217 "grampar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 229 "grampar.tab.c"

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
# if YYENABLE_NLS
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
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

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

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

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   112

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  29
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  59
/* YYNRULES -- Number of states.  */
#define YYNSTATES  106

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      19,    23,    26,    29,    33,    38,    45,    46,    49,    54,
      60,    62,    63,    64,    67,    72,    79,    80,    85,    86,
      92,    93,    96,    98,   100,   101,   104,   111,   112,   114,
     116,   120,   125,   134,   135,   138,   142,   147,   152,   154,
     156,   157,   160,   162,   166,   168,   172,   177,   182,   183
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      30,     0,    -1,    31,    -1,    -1,    31,    32,    -1,    33,
      -1,    34,    -1,    35,    -1,    36,    -1,    50,    -1,    24,
       6,    10,    -1,    19,     6,    -1,    20,     6,    -1,    22,
       4,    10,    -1,    22,     4,     3,    10,    -1,    15,     7,
      37,    40,    42,     8,    -1,    -1,    37,    38,    -1,     3,
       9,     4,    10,    -1,     3,     9,     4,     5,    10,    -1,
       6,    -1,    -1,    -1,    40,    41,    -1,    16,    39,     4,
      10,    -1,    16,    39,     4,     7,    46,     8,    -1,    -1,
      21,     7,    43,     8,    -1,    -1,    43,     4,     3,    44,
      10,    -1,    -1,    44,    45,    -1,     4,    -1,     5,    -1,
      -1,    46,    47,    -1,    18,     4,    12,    48,    13,     6,
      -1,    -1,    49,    -1,     4,    -1,    49,    14,     4,    -1,
      17,    39,     4,    52,    -1,    17,    39,     4,     7,    46,
      51,    56,     8,    -1,    -1,    51,    52,    -1,    11,    54,
      53,    -1,    27,    11,    54,    53,    -1,    26,    11,    54,
      10,    -1,     6,    -1,    10,    -1,    -1,    54,    55,    -1,
       4,    -1,     4,     9,     4,    -1,     5,    -1,     4,     9,
       5,    -1,    21,    12,    45,    13,    -1,    28,    12,    45,
      13,    -1,    -1,    25,    49,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   162,   162,   167,   168,   172,   173,   174,   175,   176,
     180,   185,   186,   191,   192,   203,   208,   209,   217,   219,
     224,   225,   229,   230,   234,   236,   241,   242,   247,   248,
     253,   254,   258,   259,   265,   266,   270,   275,   276,   280,
     281,   292,   295,   300,   301,   305,   308,   311,   317,   318,
     322,   323,   332,   333,   334,   335,   336,   337,   341,   342
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_INTEGER", "TOK_NAME", "TOK_STRING",
  "TOK_LIT_CODE", "\"{\"", "\"}\"", "\":\"", "\";\"", "\"->\"", "\"(\"",
  "\")\"", "\",\"", "\"terminals\"", "\"token\"", "\"nonterm\"", "\"fun\"",
  "\"verbatim\"", "\"impl_verbatim\"", "\"precedence\"", "\"option\"",
  "\"expect\"", "\"context_class\"", "\"subsets\"", "\"delete\"",
  "\"replace\"", "\"forbid_next\"", "$accept", "StartSymbol",
  "TopFormList", "TopForm", "ContextClass", "Verbatim", "Option",
  "Terminals", "TermDecls", "TerminalDecl", "Type", "TermTypes",
  "TermType", "Precedence", "PrecSpecs", "NameOrStringList",
  "NameOrString", "SpecFuncs", "SpecFunc", "FormalsOpt", "Formals",
  "Nonterminal", "Productions", "Production", "Action", "RHS", "RHSElt",
  "Subsets", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    29,    30,    31,    31,    32,    32,    32,    32,    32,
      33,    34,    34,    35,    35,    36,    37,    37,    38,    38,
      39,    39,    40,    40,    41,    41,    42,    42,    43,    43,
      44,    44,    45,    45,    46,    46,    47,    48,    48,    49,
      49,    50,    50,    51,    51,    52,    52,    52,    53,    53,
      54,    54,    55,    55,    55,    55,    55,    55,    56,    56
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       3,     2,     2,     3,     4,     6,     0,     2,     4,     5,
       1,     0,     0,     2,     4,     6,     0,     4,     0,     5,
       0,     2,     1,     1,     0,     2,     6,     0,     1,     1,
       3,     4,     8,     0,     2,     3,     4,     4,     1,     1,
       0,     2,     1,     3,     1,     3,     4,     4,     0,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,    21,     0,     0,     0,     0,
       4,     5,     6,     7,     8,     9,    16,    20,     0,    11,
      12,     0,     0,    22,     0,     0,    13,    10,     0,    17,
      26,    34,    50,     0,     0,    41,    14,     0,    21,     0,
      23,     0,    43,     0,    50,    50,     0,     0,    28,    15,
       0,    35,    58,    52,    54,    48,    49,     0,     0,    45,
      51,     0,     0,     0,    18,     0,     0,     0,     0,    44,
       0,     0,     0,     0,    47,    46,    19,    34,    24,     0,
      27,    37,    39,     0,    42,    53,    55,    32,    33,     0,
       0,     0,    30,     0,    38,    59,     0,    56,    57,    25,
       0,     0,    40,    29,    31,    36
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    10,    11,    12,    13,    14,    23,    29,
      18,    30,    40,    41,    66,   100,    89,    42,    51,    93,
      83,    15,    52,    35,    59,    43,    60,    70
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -74
static const yytype_int8 yypact[] =
{
     -74,    39,    21,   -74,     1,    26,    31,    49,    48,    50,
     -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,    53,   -74,
     -74,    10,    51,    55,     4,    52,   -74,   -74,    54,   -74,
      -7,   -74,   -74,    56,    57,   -74,   -74,    60,    26,    58,
     -74,    61,    41,    -3,   -74,   -74,     7,    62,   -74,   -74,
      66,   -74,     8,    63,   -74,   -74,   -74,    59,    64,   -74,
     -74,     0,    -3,    65,   -74,    37,    18,    67,    69,   -74,
      70,    44,    46,    46,   -74,   -74,   -74,   -74,   -74,    71,
     -74,    69,   -74,    32,   -74,   -74,   -74,   -74,   -74,    47,
      68,    -2,   -74,    72,    73,   -74,    76,   -74,   -74,   -74,
      19,    77,   -74,   -74,   -74,   -74
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,   -74,
      74,   -74,   -74,   -74,   -74,   -74,   -73,     5,   -74,   -74,
      -4,   -74,   -74,    34,    22,     9,   -74,   -74
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      90,    53,    54,    55,    53,    54,    99,    56,    16,    38,
      74,    31,    63,    25,    39,    32,    50,    64,    57,    32,
      26,    57,    79,    87,    88,    58,    80,   104,    58,   103,
      33,    34,    17,    68,    33,    34,     4,    19,     5,     3,
       6,     7,    95,     8,    77,     9,    96,    78,    85,    86,
      87,    88,    21,    61,    62,    20,    22,    24,    28,    50,
      97,    27,    36,    37,    46,    48,    65,    44,    45,    49,
      67,    72,    71,    82,    92,    76,    73,    94,    84,    81,
     102,    98,    91,   105,    75,   101,    69,    96,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    47
};

static const yytype_int8 yycheck[] =
{
      73,     4,     5,     6,     4,     5,     8,    10,     7,    16,
      10,     7,     5,     3,    21,    11,    18,    10,    21,    11,
      10,    21,     4,     4,     5,    28,     8,   100,    28,    10,
      26,    27,     6,    25,    26,    27,    15,     6,    17,     0,
      19,    20,    10,    22,     7,    24,    14,    10,     4,     5,
       4,     5,     4,    44,    45,     6,     6,     4,     3,    18,
      13,    10,    10,     9,     4,     7,     4,    11,    11,     8,
       4,    12,     9,     4,     3,    10,    12,    81,     8,    12,
       4,    13,    77,     6,    62,    13,    52,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    30,    31,     0,    15,    17,    19,    20,    22,    24,
      32,    33,    34,    35,    36,    50,     7,     6,    39,     6,
       6,     4,     6,    37,     4,     3,    10,    10,     3,    38,
      40,     7,    11,    26,    27,    52,    10,     9,    16,    21,
      41,    42,    46,    54,    11,    11,     4,    39,     7,     8,
      18,    47,    51,     4,     5,     6,    10,    21,    28,    53,
      55,    54,    54,     5,    10,     4,    43,     4,    25,    52,
      56,     9,    12,    12,    10,    53,    10,     7,    10,     4,
       8,    12,     4,    49,     8,     4,     5,     4,     5,    45,
      45,    46,     3,    48,    49,    10,    14,    13,    13,     8,
      44,    13,     4,    10,    45,     6
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
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
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
yyparse (void)
#else
int
yyparse ()

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

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  if (yyn == YYPACT_NINF)
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

/* Line 1455 of yacc.c  */
#line 163 "grampar.y"
    { ((ParseParams*)parseParam)->treeTop = new GrammarAST((yyvsp[(1) - (1)].topFormList)); (yyval.num)=0; ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 167 "grampar.y"
    { (yyval.topFormList) = new ASTList<TopForm>; ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 168 "grampar.y"
    { ((yyval.topFormList)=(yyvsp[(1) - (2)].topFormList))->append((yyvsp[(2) - (2)].topForm)); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 172 "grampar.y"
    { (yyval.topForm) = (yyvsp[(1) - (1)].topForm); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 173 "grampar.y"
    { (yyval.topForm) = (yyvsp[(1) - (1)].topForm); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 174 "grampar.y"
    { (yyval.topForm) = (yyvsp[(1) - (1)].topForm); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 175 "grampar.y"
    { (yyval.topForm) = (yyvsp[(1) - (1)].topForm); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 176 "grampar.y"
    { (yyval.topForm) = (yyvsp[(1) - (1)].topForm); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 181 "grampar.y"
    { (yyval.topForm) = new TF_context((yyvsp[(2) - (3)].str)); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 185 "grampar.y"
    { (yyval.topForm) = new TF_verbatim(false, (yyvsp[(2) - (2)].str)); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 186 "grampar.y"
    { (yyval.topForm) = new TF_verbatim(true, (yyvsp[(2) - (2)].str)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 191 "grampar.y"
    { (yyval.topForm) = new TF_option((yyvsp[(2) - (3)].str), 1); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 192 "grampar.y"
    { (yyval.topForm) = new TF_option((yyvsp[(2) - (4)].str), (yyvsp[(3) - (4)].num)); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 204 "grampar.y"
    { (yyval.topForm) = new TF_terminals((yyvsp[(3) - (6)].termDecls), (yyvsp[(4) - (6)].termTypes), (yyvsp[(5) - (6)].precSpecs)); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 208 "grampar.y"
    { (yyval.termDecls) = new ASTList<TermDecl>; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 209 "grampar.y"
    { ((yyval.termDecls)=(yyvsp[(1) - (2)].termDecls))->append((yyvsp[(2) - (2)].termDecl)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 218 "grampar.y"
    { (yyval.termDecl) = new TermDecl((yyvsp[(1) - (4)].num), (yyvsp[(3) - (4)].str), sameloc((yyvsp[(3) - (4)].str), "")); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 220 "grampar.y"
    { (yyval.termDecl) = new TermDecl((yyvsp[(1) - (5)].num), (yyvsp[(3) - (5)].str), (yyvsp[(4) - (5)].str)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 224 "grampar.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 225 "grampar.y"
    { (yyval.str) = nolocNULL(); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 229 "grampar.y"
    { (yyval.termTypes) = new ASTList<TermType>; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 230 "grampar.y"
    { ((yyval.termTypes)=(yyvsp[(1) - (2)].termTypes))->append((yyvsp[(2) - (2)].termType)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 235 "grampar.y"
    { (yyval.termType) = new TermType((yyvsp[(3) - (4)].str), (yyvsp[(2) - (4)].str), new ASTList<SpecFunc>); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 237 "grampar.y"
    { (yyval.termType) = new TermType((yyvsp[(3) - (6)].str), (yyvsp[(2) - (6)].str), (yyvsp[(5) - (6)].specFuncs)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 241 "grampar.y"
    { (yyval.precSpecs) = new ASTList<PrecSpec>; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 242 "grampar.y"
    { (yyval.precSpecs) = (yyvsp[(3) - (4)].precSpecs); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 247 "grampar.y"
    { (yyval.precSpecs) = new ASTList<PrecSpec>; ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 249 "grampar.y"
    { ((yyval.precSpecs)=(yyvsp[(1) - (5)].precSpecs))->append(new PrecSpec(whichKind((yyvsp[(2) - (5)].str)), (yyvsp[(3) - (5)].num), (yyvsp[(4) - (5)].stringList))); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 253 "grampar.y"
    { (yyval.stringList) = new ASTList<LocString>; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 254 "grampar.y"
    { ((yyval.stringList)=(yyvsp[(1) - (2)].stringList))->append((yyvsp[(2) - (2)].str)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 258 "grampar.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 259 "grampar.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 265 "grampar.y"
    { (yyval.specFuncs) = new ASTList<SpecFunc>; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 266 "grampar.y"
    { ((yyval.specFuncs)=(yyvsp[(1) - (2)].specFuncs))->append((yyvsp[(2) - (2)].specFunc)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 271 "grampar.y"
    { (yyval.specFunc) = new SpecFunc((yyvsp[(2) - (6)].str), (yyvsp[(4) - (6)].stringList), (yyvsp[(6) - (6)].str)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 275 "grampar.y"
    { (yyval.stringList) = new ASTList<LocString>; ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 276 "grampar.y"
    { (yyval.stringList) = (yyvsp[(1) - (1)].stringList); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 280 "grampar.y"
    { (yyval.stringList) = new ASTList<LocString>((yyvsp[(1) - (1)].str)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 281 "grampar.y"
    { ((yyval.stringList)=(yyvsp[(1) - (3)].stringList))->append((yyvsp[(3) - (3)].str)); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 293 "grampar.y"
    { (yyval.topForm) = new TF_nonterm((yyvsp[(3) - (4)].str), (yyvsp[(2) - (4)].str), new ASTList<SpecFunc>,
                                     new ASTList<ProdDecl>((yyvsp[(4) - (4)].prodDecl)), NULL); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 296 "grampar.y"
    { (yyval.topForm) = new TF_nonterm((yyvsp[(3) - (8)].str), (yyvsp[(2) - (8)].str), (yyvsp[(5) - (8)].specFuncs), (yyvsp[(6) - (8)].prodDecls), (yyvsp[(7) - (8)].stringList)); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 300 "grampar.y"
    { (yyval.prodDecls) = new ASTList<ProdDecl>; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 301 "grampar.y"
    { ((yyval.prodDecls)=(yyvsp[(1) - (2)].prodDecls))->append((yyvsp[(2) - (2)].prodDecl)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 305 "grampar.y"
    { SourceLocation loc;
                                                  ASSIGN_SOURCE_LOCATION(loc, (yyvsp[(1) - (3)].loc));
                                                  (yyval.prodDecl) = new ProdDecl(loc, PDK_NEW, (yyvsp[(2) - (3)].rhsList), (yyvsp[(3) - (3)].str)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 308 "grampar.y"
    { SourceLocation loc;
                                                  ASSIGN_SOURCE_LOCATION(loc, (yyvsp[(2) - (4)].loc));
                                                  (yyval.prodDecl) = new ProdDecl(loc, PDK_REPLACE,(yyvsp[(3) - (4)].rhsList), (yyvsp[(4) - (4)].str)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 311 "grampar.y"
    { SourceLocation loc;
                                                  ASSIGN_SOURCE_LOCATION(loc, (yyvsp[(2) - (4)].loc));
                                                  (yyval.prodDecl) = new ProdDecl(loc, PDK_DELETE, (yyvsp[(3) - (4)].rhsList), nolocNULL()); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 317 "grampar.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 318 "grampar.y"
    { (yyval.str) = nolocNULL(); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 322 "grampar.y"
    { (yyval.rhsList) = new ASTList<RHSElt>; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 323 "grampar.y"
    { ((yyval.rhsList)=(yyvsp[(1) - (2)].rhsList))->append((yyvsp[(2) - (2)].rhsElt)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 332 "grampar.y"
    { (yyval.rhsElt) = new RH_name(sameloc((yyvsp[(1) - (1)].str), ""), (yyvsp[(1) - (1)].str)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 333 "grampar.y"
    { (yyval.rhsElt) = new RH_name((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 334 "grampar.y"
    { (yyval.rhsElt) = new RH_string(sameloc((yyvsp[(1) - (1)].str), ""), (yyvsp[(1) - (1)].str)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 335 "grampar.y"
    { (yyval.rhsElt) = new RH_string((yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].str)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 336 "grampar.y"
    { (yyval.rhsElt) = new RH_prec((yyvsp[(3) - (4)].str)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 337 "grampar.y"
    { (yyval.rhsElt) = new RH_forbid((yyvsp[(3) - (4)].str)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 341 "grampar.y"
    { (yyval.stringList) = NULL; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 342 "grampar.y"
    { (yyval.stringList) = (yyvsp[(2) - (3)].stringList); ;}
    break;



/* Line 1455 of yacc.c  */
#line 1927 "grampar.tab.c"
      default: break;
    }
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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
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
		      yytoken, &yylval);
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
      if (yyn != YYPACT_NINF)
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
		  yystos[yystate], yyvsp);
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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



/* Line 1675 of yacc.c  */
#line 346 "grampar.y"

/* ------------------ extra C code ------------------ */
AssocKind whichKind(LocString * /*owner*/ kind)
{ 
  // delete 'kind' however we exit
  Owner<LocString> killer(kind);
  
  #define CHECK(syntax, value)   \
    if (kind->equals(syntax)) {  \
      return value;              \
    }
  CHECK("left", AK_LEFT);
  CHECK("right", AK_RIGHT);
  CHECK("nonassoc", AK_NONASSOC);
  CHECK("prec", AK_NEVERASSOC);
  CHECK("assoc_split", AK_SPLIT);
  #undef CHECK

  xbase(stringc << toString(kind->loc)
                << ": invalid associativity kind: " << *kind);
}

