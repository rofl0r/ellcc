/* A Bison parser, made from agrampar.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	TOK_NAME	257
# define	TOK_INTLIT	258
# define	TOK_EMBEDDED_CODE	259
# define	TOK_LBRACE	260
# define	TOK_RBRACE	261
# define	TOK_SEMICOLON	262
# define	TOK_ARROW	263
# define	TOK_LPAREN	264
# define	TOK_RPAREN	265
# define	TOK_LANGLE	266
# define	TOK_RANGLE	267
# define	TOK_STAR	268
# define	TOK_AMPERSAND	269
# define	TOK_COMMA	270
# define	TOK_EQUALS	271
# define	TOK_COLON	272
# define	TOK_CLASS	273
# define	TOK_PUBLIC	274
# define	TOK_PRIVATE	275
# define	TOK_PROTECTED	276
# define	TOK_VERBATIM	277
# define	TOK_IMPL_VERBATIM	278
# define	TOK_XML_VERBATIM	279
# define	TOK_CTOR	280
# define	TOK_DTOR	281
# define	TOK_PURE_VIRTUAL	282
# define	TOK_CUSTOM	283
# define	TOK_OPTION	284
# define	TOK_NEW	285
# define	TOK_ENUM	286

#line 6 "agrampar.y"


#include "agrampar.h"       // agrampar_yylex, etc.

#include <stdlib.h>         // malloc, free
#include <iostream.h>       // cout

// enable debugging the parser
#ifndef NDEBUG
  #define YYDEBUG 1
#endif

// permit having other parser's codes in the same program
#define yyparse agrampar_yyparse


#line 69 "agrampar.y"
#ifndef YYSTYPE
typedef union YYSTYPE {
  ASTSpecFile *file;
  ASTList<ToplevelForm> *formList;
  TF_class *tfClass;
  ASTList<CtorArg> *ctorArgList;
  ASTList<Annotation> *userDeclList;
  string *str;
  enum AccessCtl accessCtl;
  AccessMod *accessMod;
  ToplevelForm *verbatim;
  Annotation *annotation;
  TF_option *tfOption;
  ASTList<string> *stringList;
  TF_enum *tfEnum;
  ASTList<string> *enumeratorList;
  string *enumerator;
  ASTList<BaseClass> *baseClassList;
  BaseClass *baseClass;
  CustomCode *customCode;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		116
#define	YYFLAG		-32768
#define	YYNTBASE	33

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 286 ? yytranslate[x] : 62)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     2,     3,     6,     9,    12,    15,    18,    21,
      28,    36,    37,    39,    43,    45,    46,    53,    62,    65,
      66,    68,    71,    75,    77,    81,    83,    86,    88,    90,
      94,    96,    98,   100,   102,   104,   108,   109,   112,   115,
     121,   123,   127,   130,   134,   136,   138,   140,   142,   144,
     146,   148,   153,   155,   159,   162,   165,   168,   173,   174,
     177,   183,   190,   192,   196,   198,   199,   202,   204,   208,
     210,   212,   214
};
static const short yyrhs[] =
{
      34,     0,     0,    34,    35,     0,    34,    52,     0,    34,
      53,     0,    34,    55,     0,    34,    47,     0,    34,     8,
       0,    36,    19,     3,    39,    58,    37,     0,    36,    19,
       3,    40,    40,    58,    37,     0,     0,    31,     0,     6,
      38,     7,     0,     8,     0,     0,    38,     9,     3,    39,
      58,     8,     0,    38,     9,     3,    39,    58,     6,    45,
       7,     0,    38,    46,     0,     0,    40,     0,    10,    11,
       0,    10,    41,    11,     0,    42,     0,    41,    16,    42,
       0,    43,     0,    42,    43,     0,     3,     0,     4,     0,
      12,    44,    13,     0,    14,     0,    15,     0,    17,     0,
      19,     0,    42,     0,    42,    16,    44,     0,     0,    45,
      46,     0,    50,    48,     0,    50,     5,    17,     5,     8,
       0,    47,     0,    29,     3,    48,     0,     5,     8,     0,
       6,     5,     7,     0,    20,     0,    21,     0,    22,     0,
      26,     0,    27,     0,    28,     0,    49,     0,    49,    10,
      51,    11,     0,     3,     0,    51,    16,     3,     0,    23,
      48,     0,    24,    48,     0,    25,    48,     0,    30,     3,
      54,     8,     0,     0,    54,     3,     0,    32,     3,     6,
      56,     7,     0,    32,     3,     6,    56,    16,     7,     0,
      57,     0,    56,    16,    57,     0,     3,     0,     0,    18,
      59,     0,    61,     0,    59,    16,    61,     0,    20,     0,
      21,     0,    22,     0,    60,     3,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,   116,   122,   123,   124,   125,   126,   127,   128,   133,
     137,   145,   146,   158,   160,   167,   169,   171,   173,   180,
     182,   188,   190,   195,   202,   207,   209,   215,   216,   217,
     218,   219,   220,   221,   225,   227,   233,   235,   241,   243,
     245,   251,   257,   259,   265,   266,   267,   268,   269,   270,
     274,   276,   281,   283,   288,   290,   292,   297,   302,   304,
     309,   311,   316,   318,   323,   328,   330,   335,   337,   343,
     344,   345,   349
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TOK_NAME", "TOK_INTLIT", 
  "TOK_EMBEDDED_CODE", "\"{\"", "\"}\"", "\";\"", "\"->\"", "\"(\"", 
  "\")\"", "\"<\"", "\">\"", "\"*\"", "\"&\"", "\",\"", "\"=\"", "\":\"", 
  "\"class\"", "\"public\"", "\"private\"", "\"protected\"", 
  "\"verbatim\"", "\"impl_verbatim\"", "\"xml_verbatim\"", "\"ctor\"", 
  "\"dtor\"", "\"pure_virtual\"", "\"custom\"", "\"option\"", "\"new\"", 
  "\"enum\"", "StartSymbol", "Input", "Class", "NewOpt", "ClassBody", 
  "ClassMembersOpt", "CtorArgsOpt", "CtorArgs", "CtorArgList", "Arg", 
  "ArgWord", "ArgList", "CtorMembersOpt", "Annotation", "CustomCode", 
  "Embedded", "Public", "AccessMod", "StringList", "Verbatim", "Option", 
  "OptionArgs", "Enum", "EnumeratorSeq", "Enumerator", "BaseClassesOpt", 
  "BaseClassSeq", "BaseAccess", "BaseClass", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    33,    34,    34,    34,    34,    34,    34,    34,    35,
      35,    36,    36,    37,    37,    38,    38,    38,    38,    39,
      39,    40,    40,    41,    41,    42,    42,    43,    43,    43,
      43,    43,    43,    43,    44,    44,    45,    45,    46,    46,
      46,    47,    48,    48,    49,    49,    49,    49,    49,    49,
      50,    50,    51,    51,    52,    52,    52,    53,    54,    54,
      55,    55,    56,    56,    57,    58,    58,    59,    59,    60,
      60,    60,    61
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     1,     0,     2,     2,     2,     2,     2,     2,     6,
       7,     0,     1,     3,     1,     0,     6,     8,     2,     0,
       1,     2,     3,     1,     3,     1,     2,     1,     1,     3,
       1,     1,     1,     1,     1,     3,     0,     2,     2,     5,
       1,     3,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     3,     2,     2,     2,     4,     0,     2,
       5,     6,     1,     3,     1,     0,     2,     1,     3,     1,
       1,     1,     2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       2,     1,     8,     0,     0,     0,     0,     0,    12,     0,
       3,     0,     7,     4,     5,     6,     0,     0,    54,    55,
      56,     0,    58,     0,     0,    42,     0,    41,     0,     0,
      19,    43,    59,    57,    64,     0,    62,     0,    65,    20,
      60,     0,    27,    28,    21,     0,    30,    31,    32,    33,
       0,    23,    25,     0,     0,    65,    61,    63,    34,     0,
      22,     0,    26,    69,    70,    71,    66,     0,    67,    15,
      14,     9,     0,     0,    29,    24,     0,    72,     0,    10,
      35,    68,    13,     0,    44,    45,    46,    47,    48,    49,
      18,    40,    50,     0,    19,     0,     0,    38,    65,    20,
      52,     0,     0,     0,    51,     0,     0,    36,    16,    53,
      39,     0,    17,    37,     0,     0,     0
};

static const short yydefgoto[] =
{
     114,     1,    10,    11,    71,    78,    38,    39,    50,    58,
      52,    59,   111,    90,    91,    18,    92,    93,   101,    13,
      14,    28,    15,    35,    36,    54,    66,    67,    68
};

static const short yypact[] =
{
  -32768,    -3,-32768,    67,    67,    67,    29,    30,-32768,    60,
  -32768,    51,-32768,-32768,-32768,-32768,    44,    72,-32768,-32768,
  -32768,    67,-32768,    76,    83,-32768,    80,-32768,    11,    85,
      82,-32768,-32768,-32768,-32768,     2,-32768,    47,    73,    82,
  -32768,     1,-32768,-32768,-32768,    81,-32768,-32768,-32768,-32768,
      24,    81,-32768,   -10,    33,    73,-32768,-32768,    64,    77,
  -32768,    81,-32768,-32768,-32768,-32768,    78,    94,-32768,-32768,
  -32768,-32768,    33,    81,-32768,    81,   -10,-32768,    16,-32768,
  -32768,-32768,-32768,    96,-32768,-32768,-32768,-32768,-32768,-32768,
  -32768,-32768,    91,    69,    82,    99,     7,-32768,    73,-32768,
  -32768,    49,    98,    63,-32768,   101,    97,-32768,-32768,-32768,
  -32768,    27,-32768,-32768,   106,   107,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,    36,-32768,    15,   -37,-32768,   -30,
     -45,    37,-32768,     0,   111,    -4,-32768,-32768,-32768,-32768,
  -32768,-32768,-32768,-32768,    74,   -52,-32768,-32768,    38
};


#define	YYLAST		115


static const short yytable[] =
{
      19,    20,    55,    72,    34,     2,    62,    51,    56,    40,
      63,    64,    65,    62,    32,    25,   -11,    27,    41,    33,
       3,     4,     5,    82,   102,    83,     6,     7,     8,     9,
      62,    75,    21,    22,   112,    60,    84,    85,    86,    69,
      61,    70,    87,    88,    89,     6,   103,    84,    85,    86,
      42,    43,    25,    87,    88,    89,     6,    99,    44,    45,
     104,    46,    47,    23,    48,   105,    49,    42,    43,   107,
      24,   108,    16,    17,    96,    17,    45,    26,    46,    47,
      73,    48,    29,    49,    42,    43,    30,    31,    34,    97,
      74,    53,    37,    45,    76,    46,    47,    77,    48,    94,
      49,    95,   100,   106,   109,   110,   115,   116,    79,    98,
      80,   113,    12,     0,    81,    57
};

static const short yycheck[] =
{
       4,     5,    39,    55,     3,     8,    51,    37,     7,     7,
      20,    21,    22,    58,     3,     8,    19,    21,    16,     8,
      23,    24,    25,     7,    17,     9,    29,    30,    31,    32,
      75,    61,     3,     3,     7,    11,    20,    21,    22,     6,
      16,     8,    26,    27,    28,    29,    98,    20,    21,    22,
       3,     4,     8,    26,    27,    28,    29,    94,    11,    12,
      11,    14,    15,     3,    17,    16,    19,     3,     4,     6,
      19,     8,     5,     6,     5,     6,    12,     5,    14,    15,
      16,    17,     6,    19,     3,     4,     3,     7,     3,    93,
      13,    18,    10,    12,    16,    14,    15,     3,    17,     3,
      19,    10,     3,     5,     3,     8,     0,     0,    72,    94,
      73,   111,     1,    -1,    76,    41
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
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
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


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
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 117 "agrampar.y"
{ yyval.file = *((ASTSpecFile**)parseParam) = new ASTSpecFile(yyvsp[0].formList); ;
    break;}
case 2:
#line 122 "agrampar.y"
{ yyval.formList = new ASTList<ToplevelForm>; ;
    break;}
case 3:
#line 123 "agrampar.y"
{ (yyval.formList=yyvsp[-1].formList)->append(yyvsp[0].tfClass); ;
    break;}
case 4:
#line 124 "agrampar.y"
{ (yyval.formList=yyvsp[-1].formList)->append(yyvsp[0].verbatim); ;
    break;}
case 5:
#line 125 "agrampar.y"
{ (yyval.formList=yyvsp[-1].formList)->append(yyvsp[0].tfOption); ;
    break;}
case 6:
#line 126 "agrampar.y"
{ (yyval.formList=yyvsp[-1].formList)->append(yyvsp[0].tfEnum); ;
    break;}
case 7:
#line 127 "agrampar.y"
{ (yyval.formList=yyvsp[-1].formList)->append(new TF_custom(yyvsp[0].customCode)); ;
    break;}
case 8:
#line 128 "agrampar.y"
{ yyval.formList=yyvsp[-1].formList; ;
    break;}
case 9:
#line 134 "agrampar.y"
{ (yyval.tfClass=yyvsp[0].tfClass)->super->name = unbox(yyvsp[-3].str); 
           yyval.tfClass->super->args.steal(yyvsp[-2].ctorArgList); 
           yyval.tfClass->super->bases.steal(yyvsp[-1].baseClassList); ;
    break;}
case 10:
#line 138 "agrampar.y"
{ (yyval.tfClass=yyvsp[0].tfClass)->super->name = unbox(yyvsp[-4].str);
           yyval.tfClass->super->args.steal(yyvsp[-3].ctorArgList);
           yyval.tfClass->super->lastArgs.steal(yyvsp[-2].ctorArgList);
           yyval.tfClass->super->bases.steal(yyvsp[-1].baseClassList); ;
    break;}
case 11:
#line 145 "agrampar.y"
{;
    break;}
case 12:
#line 146 "agrampar.y"
{;
    break;}
case 13:
#line 159 "agrampar.y"
{ yyval.tfClass=yyvsp[-1].tfClass; ;
    break;}
case 14:
#line 161 "agrampar.y"
{ yyval.tfClass = new TF_class(new ASTClass("(placeholder)", NULL, NULL, NULL, NULL), NULL); ;
    break;}
case 15:
#line 168 "agrampar.y"
{ yyval.tfClass = new TF_class(new ASTClass("(placeholder)", NULL, NULL, NULL, NULL), NULL); ;
    break;}
case 16:
#line 170 "agrampar.y"
{ (yyval.tfClass=yyvsp[-5].tfClass)->ctors.append(new ASTClass(unbox(yyvsp[-3].str), yyvsp[-2].ctorArgList, NULL, yyvsp[-1].baseClassList, NULL)); ;
    break;}
case 17:
#line 172 "agrampar.y"
{ (yyval.tfClass=yyvsp[-7].tfClass)->ctors.append(new ASTClass(unbox(yyvsp[-5].str), yyvsp[-4].ctorArgList, NULL, yyvsp[-3].baseClassList, yyvsp[-1].userDeclList)); ;
    break;}
case 18:
#line 174 "agrampar.y"
{ (yyval.tfClass=yyvsp[-1].tfClass)->super->decls.append(yyvsp[0].annotation); ;
    break;}
case 19:
#line 181 "agrampar.y"
{ yyval.ctorArgList = new ASTList<CtorArg>; ;
    break;}
case 20:
#line 183 "agrampar.y"
{ yyval.ctorArgList = yyvsp[0].ctorArgList; ;
    break;}
case 21:
#line 189 "agrampar.y"
{ yyval.ctorArgList = new ASTList<CtorArg>; ;
    break;}
case 22:
#line 191 "agrampar.y"
{ yyval.ctorArgList = yyvsp[-1].ctorArgList; ;
    break;}
case 23:
#line 196 "agrampar.y"
{ yyval.ctorArgList = new ASTList<CtorArg>;
                 {
                   string tmp = unbox(yyvsp[0].str);
                   yyval.ctorArgList->append(parseCtorArg(tmp));
                 }
               ;
    break;}
case 24:
#line 203 "agrampar.y"
{ (yyval.ctorArgList=yyvsp[-2].ctorArgList)->append(parseCtorArg(unbox(yyvsp[0].str))); ;
    break;}
case 25:
#line 208 "agrampar.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 26:
#line 210 "agrampar.y"
{ yyval.str = appendStr(yyvsp[-1].str, yyvsp[0].str); ;
    break;}
case 27:
#line 215 "agrampar.y"
{ yyval.str = appendStr(yyvsp[0].str, box(" ")); ;
    break;}
case 28:
#line 216 "agrampar.y"
{ yyval.str = appendStr(yyvsp[0].str, box(" ")); ;
    break;}
case 29:
#line 217 "agrampar.y"
{ yyval.str = appendStr(box("<"), appendStr(yyvsp[-1].str, box(">"))); ;
    break;}
case 30:
#line 218 "agrampar.y"
{ yyval.str = box("*"); ;
    break;}
case 31:
#line 219 "agrampar.y"
{ yyval.str = box("&"); ;
    break;}
case 32:
#line 220 "agrampar.y"
{ yyval.str = box("="); ;
    break;}
case 33:
#line 221 "agrampar.y"
{ yyval.str = box("class "); ;
    break;}
case 34:
#line 226 "agrampar.y"
{ yyval.str = yyvsp[0].str; ;
    break;}
case 35:
#line 228 "agrampar.y"
{ yyval.str = appendStr(yyvsp[-2].str, appendStr(box(","), yyvsp[0].str)); ;
    break;}
case 36:
#line 234 "agrampar.y"
{ yyval.userDeclList = new ASTList<Annotation>; ;
    break;}
case 37:
#line 236 "agrampar.y"
{ (yyval.userDeclList=yyvsp[-1].userDeclList)->append(yyvsp[0].annotation); ;
    break;}
case 38:
#line 242 "agrampar.y"
{ yyval.annotation = new UserDecl(yyvsp[-1].accessMod, unbox(yyvsp[0].str), ""); ;
    break;}
case 39:
#line 244 "agrampar.y"
{ yyval.annotation = new UserDecl(yyvsp[-4].accessMod, unbox(yyvsp[-3].str), unbox(yyvsp[-1].str)); ;
    break;}
case 40:
#line 246 "agrampar.y"
{ yyval.annotation = yyvsp[0].customCode; ;
    break;}
case 41:
#line 252 "agrampar.y"
{ yyval.customCode = new CustomCode(unbox(yyvsp[-1].str), unbox(yyvsp[0].str)); ;
    break;}
case 42:
#line 258 "agrampar.y"
{ yyval.str = yyvsp[-1].str; ;
    break;}
case 43:
#line 260 "agrampar.y"
{ yyval.str = yyvsp[-1].str; ;
    break;}
case 44:
#line 265 "agrampar.y"
{ yyval.accessCtl = AC_PUBLIC; ;
    break;}
case 45:
#line 266 "agrampar.y"
{ yyval.accessCtl = AC_PRIVATE; ;
    break;}
case 46:
#line 267 "agrampar.y"
{ yyval.accessCtl = AC_PROTECTED; ;
    break;}
case 47:
#line 268 "agrampar.y"
{ yyval.accessCtl = AC_CTOR; ;
    break;}
case 48:
#line 269 "agrampar.y"
{ yyval.accessCtl = AC_DTOR; ;
    break;}
case 49:
#line 270 "agrampar.y"
{ yyval.accessCtl = AC_PUREVIRT; ;
    break;}
case 50:
#line 275 "agrampar.y"
{ yyval.accessMod = new AccessMod(yyvsp[0].accessCtl, NULL); ;
    break;}
case 51:
#line 277 "agrampar.y"
{ yyval.accessMod = new AccessMod(yyvsp[-3].accessCtl, yyvsp[-1].stringList); ;
    break;}
case 52:
#line 282 "agrampar.y"
{ yyval.stringList = new ASTList<string>(yyvsp[0].str); ;
    break;}
case 53:
#line 284 "agrampar.y"
{ (yyval.stringList=yyvsp[-2].stringList)->append(yyvsp[0].str); ;
    break;}
case 54:
#line 289 "agrampar.y"
{ yyval.verbatim = new TF_verbatim(unbox(yyvsp[0].str)); ;
    break;}
case 55:
#line 291 "agrampar.y"
{ yyval.verbatim = new TF_impl_verbatim(unbox(yyvsp[0].str)); ;
    break;}
case 56:
#line 293 "agrampar.y"
{ yyval.verbatim = new TF_xml_verbatim(unbox(yyvsp[0].str)); ;
    break;}
case 57:
#line 298 "agrampar.y"
{ yyval.tfOption = new TF_option(unbox(yyvsp[-2].str), yyvsp[-1].stringList); ;
    break;}
case 58:
#line 303 "agrampar.y"
{ yyval.stringList = new ASTList<string>; ;
    break;}
case 59:
#line 305 "agrampar.y"
{ (yyval.stringList=yyvsp[-1].stringList)->append(yyvsp[0].str); ;
    break;}
case 60:
#line 310 "agrampar.y"
{ yyval.tfEnum = new TF_enum(unbox(yyvsp[-3].str), yyvsp[-1].enumeratorList); ;
    break;}
case 61:
#line 312 "agrampar.y"
{ yyval.tfEnum = new TF_enum(unbox(yyvsp[-4].str), yyvsp[-2].enumeratorList); ;
    break;}
case 62:
#line 317 "agrampar.y"
{ yyval.enumeratorList = new ASTList<string>(yyvsp[0].enumerator); ;
    break;}
case 63:
#line 319 "agrampar.y"
{ (yyval.enumeratorList=yyvsp[-2].enumeratorList)->append(yyvsp[0].enumerator); ;
    break;}
case 64:
#line 324 "agrampar.y"
{ yyval.enumerator = yyvsp[0].str; ;
    break;}
case 65:
#line 329 "agrampar.y"
{ yyval.baseClassList = new ASTList<BaseClass>; ;
    break;}
case 66:
#line 331 "agrampar.y"
{ yyval.baseClassList = yyvsp[0].baseClassList; ;
    break;}
case 67:
#line 336 "agrampar.y"
{ yyval.baseClassList = new ASTList<BaseClass>(yyvsp[0].baseClass); ;
    break;}
case 68:
#line 338 "agrampar.y"
{ (yyval.baseClassList=yyvsp[-2].baseClassList)->append(yyvsp[0].baseClass); ;
    break;}
case 69:
#line 343 "agrampar.y"
{ yyval.accessCtl = AC_PUBLIC; ;
    break;}
case 70:
#line 344 "agrampar.y"
{ yyval.accessCtl = AC_PRIVATE; ;
    break;}
case 71:
#line 345 "agrampar.y"
{ yyval.accessCtl = AC_PROTECTED; ;
    break;}
case 72:
#line 350 "agrampar.y"
{ yyval.baseClass = new BaseClass(yyvsp[-1].accessCtl, unbox(yyvsp[0].str)); ;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 353 "agrampar.y"


/* ----------------- extra C code ------------------- */

