/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     FLOAT = 259,
     STRING = 260,
     FIELDNAME = 261,
     NAME = 262,
     TYPENAME = 263,
     NAME_OR_INT = 264,
     STRUCT = 265,
     CLASS = 266,
     SIZEOF = 267,
     COLONCOLON = 268,
     ERROR = 269,
     VARIABLE = 270,
     THIS = 271,
     TRUEKEYWORD = 272,
     FALSEKEYWORD = 273,
     ABOVE_COMMA = 274,
     ASSIGN = 275,
     NOT = 276,
     OR = 277,
     XOR = 278,
     ANDAND = 279,
     NOTEQUAL = 280,
     GEQ = 281,
     LEQ = 282,
     MOD = 283,
     DIV = 284,
     RSH = 285,
     LSH = 286,
     DECREMENT = 287,
     INCREMENT = 288,
     UNARY = 289,
     ARROW = 290,
     BLOCKNAME = 291
   };
#endif
/* Tokens.  */
#define INT 258
#define FLOAT 259
#define STRING 260
#define FIELDNAME 261
#define NAME 262
#define TYPENAME 263
#define NAME_OR_INT 264
#define STRUCT 265
#define CLASS 266
#define SIZEOF 267
#define COLONCOLON 268
#define ERROR 269
#define VARIABLE 270
#define THIS 271
#define TRUEKEYWORD 272
#define FALSEKEYWORD 273
#define ABOVE_COMMA 274
#define ASSIGN 275
#define NOT 276
#define OR 277
#define XOR 278
#define ANDAND 279
#define NOTEQUAL 280
#define GEQ 281
#define LEQ 282
#define MOD 283
#define DIV 284
#define RSH 285
#define LSH 286
#define DECREMENT 287
#define INCREMENT 288
#define UNARY 289
#define ARROW 290
#define BLOCKNAME 291




/* Copy the first part of user declarations.  */
#line 46 "p-exp.y"


#include "defs.h"
#include "gdb_string.h"
#include <ctype.h>
#include "expression.h"
#include "value.h"
#include "parser-defs.h"
#include "language.h"
#include "p-lang.h"
#include "bfd.h" /* Required by objfiles.h.  */
#include "symfile.h" /* Required by objfiles.h.  */
#include "objfiles.h" /* For have_full_symbols and have_partial_symbols */
#include "block.h"

/* Remap normal yacc parser interface names (yyparse, yylex, yyerror, etc),
   as well as gratuitiously global symbol names, so we can have multiple
   yacc generated parsers in gdb.  Note that these are only the variables
   produced by yacc.  If other parser generators (bison, byacc, etc) produce
   additional global names that conflict at link time, then those parser
   generators need to be fixed instead of adding those names to this list. */

#define	yymaxdepth pascal_maxdepth
#define	yyparse	pascal_parse
#define	yylex	pascal_lex
#define	yyerror	pascal_error
#define	yylval	pascal_lval
#define	yychar	pascal_char
#define	yydebug	pascal_debug
#define	yypact	pascal_pact	
#define	yyr1	pascal_r1			
#define	yyr2	pascal_r2			
#define	yydef	pascal_def		
#define	yychk	pascal_chk		
#define	yypgo	pascal_pgo		
#define	yyact	pascal_act
#define	yyexca	pascal_exca
#define yyerrflag pascal_errflag
#define yynerrs	pascal_nerrs
#define	yyps	pascal_ps
#define	yypv	pascal_pv
#define	yys	pascal_s
#define	yy_yys	pascal_yys
#define	yystate	pascal_state
#define	yytmp	pascal_tmp
#define	yyv	pascal_v
#define	yy_yyv	pascal_yyv
#define	yyval	pascal_val
#define	yylloc	pascal_lloc
#define yyreds	pascal_reds		/* With YYDEBUG defined */
#define yytoks	pascal_toks		/* With YYDEBUG defined */
#define yyname	pascal_name		/* With YYDEBUG defined */
#define yyrule	pascal_rule		/* With YYDEBUG defined */
#define yylhs	pascal_yylhs
#define yylen	pascal_yylen
#define yydefred pascal_yydefred
#define yydgoto	pascal_yydgoto
#define yysindex pascal_yysindex
#define yyrindex pascal_yyrindex
#define yygindex pascal_yygindex
#define yytable	 pascal_yytable
#define yycheck	 pascal_yycheck

#ifndef YYDEBUG
#define	YYDEBUG 1		/* Default to yydebug support */
#endif

#define YYFPRINTF parser_fprintf

int yyparse (void);

static int yylex (void);

void
yyerror (char *);

static char * uptok (char *, int);


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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 130 "p-exp.y"
{
    LONGEST lval;
    struct {
      LONGEST val;
      struct type *type;
    } typed_val_int;
    struct {
      DOUBLEST dval;
      struct type *type;
    } typed_val_float;
    struct symbol *sym;
    struct type *tval;
    struct stoken sval;
    struct ttype tsym;
    struct symtoken ssym;
    int voidval;
    struct block *bval;
    enum exp_opcode opcode;
    struct internalvar *ivar;

    struct type **tvec;
    int *ivec;
  }
/* Line 187 of yacc.c.  */
#line 271 "p-exp.c.tmp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 154 "p-exp.y"

/* YYSTYPE gets defined by %union */
static int
parse_number (char *, int, int, YYSTYPE *);

static struct type *current_type;
static int leftdiv_is_integer;
static void push_current_type (void);
static void pop_current_type (void);
static int search_field;


/* Line 216 of yacc.c.  */
#line 295 "p-exp.c.tmp"

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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or xmalloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
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
       && ! ((defined YYMALLOC || defined xmalloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC xmalloc
#   if ! defined xmalloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *xmalloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   358

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  73
/* YYNRULES -- Number of states.  */
#define YYNSTATES  122

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   291

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      46,    50,    39,    37,    19,    38,    44,    40,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      28,    26,    29,     2,    36,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    45,     2,    51,    48,     2,     2,     2,     2,     2,
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
      15,    16,    17,    18,    20,    21,    22,    23,    24,    25,
      27,    30,    31,    32,    33,    34,    35,    41,    42,    43,
      47,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    19,
      22,    25,    28,    31,    36,    41,    42,    47,    48,    54,
      55,    61,    62,    64,    68,    73,    77,    81,    82,    87,
      91,    95,    99,   103,   107,   111,   115,   119,   123,   127,
     131,   135,   139,   143,   147,   151,   153,   155,   157,   159,
     161,   163,   165,   170,   172,   174,   176,   180,   184,   188,
     190,   193,   195,   197,   199,   202,   204,   207,   210,   212,
     214,   216,   218,   220
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,    -1,    54,    55,    -1,    57,    -1,    56,
      -1,    68,    -1,    58,    -1,    57,    19,    58,    -1,    58,
      48,    -1,    36,    58,    -1,    38,    58,    -1,    22,    58,
      -1,    42,    46,    58,    50,    -1,    41,    46,    58,    50,
      -1,    -1,    58,    44,    59,     6,    -1,    -1,    58,    45,
      60,    57,    51,    -1,    -1,    58,    46,    61,    62,    50,
      -1,    -1,    58,    -1,    62,    19,    58,    -1,    68,    46,
      58,    50,    -1,    46,    57,    50,    -1,    58,    39,    58,
      -1,    -1,    58,    40,    63,    58,    -1,    58,    33,    58,
      -1,    58,    32,    58,    -1,    58,    37,    58,    -1,    58,
      38,    58,    -1,    58,    35,    58,    -1,    58,    34,    58,
      -1,    58,    26,    58,    -1,    58,    27,    58,    -1,    58,
      31,    58,    -1,    58,    30,    58,    -1,    58,    28,    58,
      -1,    58,    29,    58,    -1,    58,    25,    58,    -1,    58,
      24,    58,    -1,    58,    23,    58,    -1,    58,    21,    58,
      -1,    17,    -1,    18,    -1,     3,    -1,     9,    -1,     4,
      -1,    65,    -1,    15,    -1,    12,    46,    68,    50,    -1,
       5,    -1,    16,    -1,    49,    -1,    64,    13,    70,    -1,
      64,    13,    70,    -1,    69,    13,    70,    -1,    66,    -1,
      13,    70,    -1,    71,    -1,    69,    -1,    67,    -1,    48,
      69,    -1,     8,    -1,    10,    70,    -1,    11,    70,    -1,
       7,    -1,    49,    -1,     8,    -1,     9,    -1,     7,    -1,
      49,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   234,   234,   234,   242,   243,   246,   253,   254,   259,
     265,   271,   275,   279,   283,   287,   287,   302,   300,   330,
     327,   342,   343,   345,   349,   364,   370,   374,   374,   394,
     398,   402,   406,   410,   414,   418,   424,   430,   436,   442,
     448,   454,   458,   462,   466,   470,   477,   484,   492,   504,
     512,   515,   519,   527,   552,   579,   596,   607,   623,   638,
     639,   673,   746,   757,   761,   763,   765,   768,   776,   777,
     778,   779,   782,   783
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "FLOAT", "STRING", "FIELDNAME",
  "NAME", "TYPENAME", "NAME_OR_INT", "STRUCT", "CLASS", "SIZEOF",
  "COLONCOLON", "ERROR", "VARIABLE", "THIS", "TRUEKEYWORD", "FALSEKEYWORD",
  "','", "ABOVE_COMMA", "ASSIGN", "NOT", "OR", "XOR", "ANDAND", "'='",
  "NOTEQUAL", "'<'", "'>'", "GEQ", "LEQ", "MOD", "DIV", "RSH", "LSH",
  "'@'", "'+'", "'-'", "'*'", "'/'", "DECREMENT", "INCREMENT", "UNARY",
  "'.'", "'['", "'('", "ARROW", "'^'", "BLOCKNAME", "')'", "']'",
  "$accept", "start", "@1", "normal_start", "type_exp", "exp1", "exp",
  "@2", "@3", "@4", "arglist", "@5", "block", "variable", "qualified_name",
  "ptype", "type", "typebase", "name", "name_not_typename", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,    44,
     274,   275,   276,   277,   278,   279,    61,   280,    60,    62,
     281,   282,   283,   284,   285,   286,    64,    43,    45,    42,
      47,   287,   288,   289,    46,    91,    40,   290,    94,   291,
      41,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    52,    54,    53,    55,    55,    56,    57,    57,    58,
      58,    58,    58,    58,    58,    59,    58,    60,    58,    61,
      58,    62,    62,    62,    58,    58,    58,    63,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    64,    64,    65,    66,    65,
      65,    65,    67,    68,    69,    69,    69,    69,    70,    70,
      70,    70,    71,    71
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     3,     2,
       2,     2,     2,     4,     4,     0,     4,     0,     5,     0,
       5,     0,     1,     3,     4,     3,     3,     0,     4,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     4,     1,     1,     1,     3,     3,     3,     1,
       2,     1,     1,     1,     2,     1,     2,     2,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,    47,    49,    53,    72,    65,    48,
       0,     0,     0,     0,    51,    54,    45,    46,     0,     0,
       0,     0,     0,     0,     0,    73,     3,     5,     4,     7,
       0,    50,    59,    63,     6,    62,    61,    68,    70,    71,
      69,    66,    67,     0,    60,    12,     0,    10,    11,     0,
       0,     0,    64,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    27,    15,    17,    19,     9,     0,     0,     0,     0,
      62,     0,     0,    25,     8,    44,    43,    42,    41,    35,
      36,    39,    40,    38,    37,    30,    29,    34,    33,    31,
      32,    26,     0,     0,     0,    21,    57,     0,    58,    52,
      14,    13,    28,    16,     0,    22,     0,    24,    18,     0,
      20,    23
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    26,    27,    28,    29,   103,   104,   105,
     116,   102,    30,    31,    32,    33,    46,    35,    41,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -40
static const yytype_int16 yypact[] =
{
     -40,     6,    87,   -40,   -40,   -40,   -40,   -40,   -40,   -40,
       4,     4,   -39,     4,   -40,   -40,   -40,   -40,    87,    87,
      87,   -37,   -36,    87,     8,     2,   -40,   -40,    -2,   200,
      11,   -40,   -40,   -40,   -26,    15,   -40,   -40,   -40,   -40,
     -40,   -40,   -40,     8,   -40,    32,   -26,    32,    32,    87,
      87,     7,   -40,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,   -40,   -40,   -40,   -40,   -40,     4,    87,     4,   -29,
     -40,   116,   144,   -40,   200,   200,   225,   249,   272,   293,
     293,   310,   310,   310,   310,    27,    27,    27,    27,    67,
      67,    32,    87,    17,    87,    87,    20,   172,   -40,   -40,
     -40,   -40,    32,   -40,     3,   200,    10,   -40,   -40,    87,
     -40,   200
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -40,   -40,   -40,   -40,   -40,   -19,   -18,   -40,   -40,   -40,
     -40,   -40,   -40,   -40,   -40,   -40,    12,   -16,    -8,   -40
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -57
static const yytype_int8 yytable[] =
{
      45,    47,    48,    42,    51,    44,     3,    43,    52,    49,
      50,    37,    38,    39,    34,   -55,     8,    53,    10,    11,
      77,   109,    53,   113,    76,     0,    53,    80,    78,   119,
       0,    81,    82,   -56,     0,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,    40,   118,    79,    24,    83,     0,   107,
     120,     0,     0,     0,    68,    69,    70,    71,   106,     0,
     108,    72,    73,    74,     0,    75,    72,    73,    74,     0,
      75,     0,     0,     0,   112,   114,     0,   115,     0,     0,
       4,     5,     6,     0,     7,     8,     9,    10,    11,    12,
      13,   121,    14,    15,    16,    17,    70,    71,     0,    18,
       0,    72,    73,    74,     0,    75,     0,     0,     0,     0,
       0,     0,     0,    19,     0,    20,     0,     0,    21,    22,
       0,     0,     0,    23,     0,    24,    25,    54,     0,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,    69,    70,    71,     0,     0,     0,
      72,    73,    74,     0,    75,    54,   110,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,    71,     0,     0,     0,    72,    73,
      74,     0,    75,    54,   111,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,    71,     0,     0,     0,    72,    73,    74,     0,
      75,    54,   117,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
      71,     0,     0,     0,    72,    73,    74,     0,    75,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,    71,     0,     0,     0,    72,
      73,    74,     0,    75,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,    71,
       0,     0,     0,    72,    73,    74,     0,    75,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,    71,     0,     0,     0,    72,    73,    74,     0,
      75,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,    71,     0,     0,     0,    72,    73,    74,
       0,    75,    64,    65,    66,    67,     0,    68,    69,    70,
      71,     0,     0,     0,    72,    73,    74,     0,    75
};

static const yytype_int8 yycheck[] =
{
      18,    19,    20,    11,    23,    13,     0,    46,    24,    46,
      46,     7,     8,     9,     2,    13,     8,    19,    10,    11,
      46,    50,    19,     6,    13,    -1,    19,    43,    13,    19,
      -1,    49,    50,    13,    -1,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    49,    51,    43,    48,    50,    -1,    77,
      50,    -1,    -1,    -1,    37,    38,    39,    40,    76,    -1,
      78,    44,    45,    46,    -1,    48,    44,    45,    46,    -1,
      48,    -1,    -1,    -1,   102,   104,    -1,   105,    -1,    -1,
       3,     4,     5,    -1,     7,     8,     9,    10,    11,    12,
      13,   119,    15,    16,    17,    18,    39,    40,    -1,    22,
      -1,    44,    45,    46,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    36,    -1,    38,    -1,    -1,    41,    42,
      -1,    -1,    -1,    46,    -1,    48,    49,    21,    -1,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    38,    39,    40,    -1,    -1,    -1,
      44,    45,    46,    -1,    48,    21,    50,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    38,    39,    40,    -1,    -1,    -1,    44,    45,
      46,    -1,    48,    21,    50,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      38,    39,    40,    -1,    -1,    -1,    44,    45,    46,    -1,
      48,    21,    50,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    -1,    48,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    38,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    -1,    48,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    38,    39,    40,
      -1,    -1,    -1,    44,    45,    46,    -1,    48,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      38,    39,    40,    -1,    -1,    -1,    44,    45,    46,    -1,
      48,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    38,    39,    40,    -1,    -1,    -1,    44,    45,    46,
      -1,    48,    32,    33,    34,    35,    -1,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    53,    54,     0,     3,     4,     5,     7,     8,     9,
      10,    11,    12,    13,    15,    16,    17,    18,    22,    36,
      38,    41,    42,    46,    48,    49,    55,    56,    57,    58,
      64,    65,    66,    67,    68,    69,    71,     7,     8,     9,
      49,    70,    70,    46,    70,    58,    68,    58,    58,    46,
      46,    57,    69,    19,    21,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    37,    38,
      39,    40,    44,    45,    46,    48,    13,    46,    13,    68,
      69,    58,    58,    50,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    63,    59,    60,    61,    70,    58,    70,    50,
      50,    50,    58,     6,    57,    58,    62,    50,    51,    19,
      50,    58
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
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



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
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to xreallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

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
	/* Give user a chance to xreallocate the stack.  Use copies of
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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
#line 234 "p-exp.y"
    { current_type = NULL;
		  search_field = 0;
		  leftdiv_is_integer = 0;
		}
    break;

  case 3:
#line 238 "p-exp.y"
    {}
    break;

  case 6:
#line 247 "p-exp.y"
    { write_exp_elt_opcode(OP_TYPE);
			  write_exp_elt_type((yyvsp[(1) - (1)].tval));
			  write_exp_elt_opcode(OP_TYPE);
			  current_type = (yyvsp[(1) - (1)].tval); }
    break;

  case 8:
#line 255 "p-exp.y"
    { write_exp_elt_opcode (BINOP_COMMA); }
    break;

  case 9:
#line 260 "p-exp.y"
    { write_exp_elt_opcode (UNOP_IND);
			  if (current_type) 
			    current_type = TYPE_TARGET_TYPE (current_type); }
    break;

  case 10:
#line 266 "p-exp.y"
    { write_exp_elt_opcode (UNOP_ADDR); 
			  if (current_type)
			    current_type = TYPE_POINTER_TYPE (current_type); }
    break;

  case 11:
#line 272 "p-exp.y"
    { write_exp_elt_opcode (UNOP_NEG); }
    break;

  case 12:
#line 276 "p-exp.y"
    { write_exp_elt_opcode (UNOP_LOGICAL_NOT); }
    break;

  case 13:
#line 280 "p-exp.y"
    { write_exp_elt_opcode (UNOP_PREINCREMENT); }
    break;

  case 14:
#line 284 "p-exp.y"
    { write_exp_elt_opcode (UNOP_PREDECREMENT); }
    break;

  case 15:
#line 287 "p-exp.y"
    { search_field = 1; }
    break;

  case 16:
#line 290 "p-exp.y"
    { write_exp_elt_opcode (STRUCTOP_STRUCT);
			  write_exp_string ((yyvsp[(4) - (4)].sval)); 
			  write_exp_elt_opcode (STRUCTOP_STRUCT);
			  search_field = 0; 
			  if (current_type)
			    { while (TYPE_CODE (current_type) == TYPE_CODE_PTR)
				current_type = TYPE_TARGET_TYPE (current_type);
			      current_type = lookup_struct_elt_type (
				current_type, (yyvsp[(4) - (4)].sval).ptr, 0); };
			 }
    break;

  case 17:
#line 302 "p-exp.y"
    { char *arrayname; 
			  int arrayfieldindex;
			  arrayfieldindex = is_pascal_string_type (
				current_type, NULL, NULL,
				NULL, NULL, &arrayname); 
			  if (arrayfieldindex) 
			    {
			      struct stoken stringsval;
			      stringsval.ptr = alloca (strlen (arrayname) + 1);
			      stringsval.length = strlen (arrayname);
			      strcpy (stringsval.ptr, arrayname);
			      current_type = TYPE_FIELD_TYPE (current_type,
				arrayfieldindex - 1); 
			      write_exp_elt_opcode (STRUCTOP_STRUCT);
			      write_exp_string (stringsval); 
			      write_exp_elt_opcode (STRUCTOP_STRUCT);
			    }
			  push_current_type ();  }
    break;

  case 18:
#line 321 "p-exp.y"
    { pop_current_type ();
			  write_exp_elt_opcode (BINOP_SUBSCRIPT);
			  if (current_type)
			    current_type = TYPE_TARGET_TYPE (current_type); }
    break;

  case 19:
#line 330 "p-exp.y"
    { push_current_type ();
			  start_arglist (); }
    break;

  case 20:
#line 333 "p-exp.y"
    { write_exp_elt_opcode (OP_FUNCALL);
			  write_exp_elt_longcst ((LONGEST) end_arglist ());
			  write_exp_elt_opcode (OP_FUNCALL); 
			  pop_current_type ();
			  if (current_type)
 	  		    current_type = TYPE_TARGET_TYPE (current_type);
			}
    break;

  case 22:
#line 344 "p-exp.y"
    { arglist_len = 1; }
    break;

  case 23:
#line 346 "p-exp.y"
    { arglist_len++; }
    break;

  case 24:
#line 350 "p-exp.y"
    { if (current_type)
			    {
			      /* Allow automatic dereference of classes.  */
			      if ((TYPE_CODE (current_type) == TYPE_CODE_PTR)
				  && (TYPE_CODE (TYPE_TARGET_TYPE (current_type)) == TYPE_CODE_CLASS)
				  && (TYPE_CODE ((yyvsp[(1) - (4)].tval)) == TYPE_CODE_CLASS))
				write_exp_elt_opcode (UNOP_IND);
			    }
			  write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type ((yyvsp[(1) - (4)].tval));
			  write_exp_elt_opcode (UNOP_CAST); 
			  current_type = (yyvsp[(1) - (4)].tval); }
    break;

  case 25:
#line 365 "p-exp.y"
    { }
    break;

  case 26:
#line 371 "p-exp.y"
    { write_exp_elt_opcode (BINOP_MUL); }
    break;

  case 27:
#line 374 "p-exp.y"
    {
			  if (current_type && is_integral_type (current_type))
			    leftdiv_is_integer = 1;
			}
    break;

  case 28:
#line 379 "p-exp.y"
    { 
			  if (leftdiv_is_integer && current_type
			      && is_integral_type (current_type))
			    {
			      write_exp_elt_opcode (UNOP_CAST);
			      write_exp_elt_type (builtin_type_long_double);
			      current_type = builtin_type_long_double;
			      write_exp_elt_opcode (UNOP_CAST);
			      leftdiv_is_integer = 0;
			    }

			  write_exp_elt_opcode (BINOP_DIV); 
			}
    break;

  case 29:
#line 395 "p-exp.y"
    { write_exp_elt_opcode (BINOP_INTDIV); }
    break;

  case 30:
#line 399 "p-exp.y"
    { write_exp_elt_opcode (BINOP_REM); }
    break;

  case 31:
#line 403 "p-exp.y"
    { write_exp_elt_opcode (BINOP_ADD); }
    break;

  case 32:
#line 407 "p-exp.y"
    { write_exp_elt_opcode (BINOP_SUB); }
    break;

  case 33:
#line 411 "p-exp.y"
    { write_exp_elt_opcode (BINOP_LSH); }
    break;

  case 34:
#line 415 "p-exp.y"
    { write_exp_elt_opcode (BINOP_RSH); }
    break;

  case 35:
#line 419 "p-exp.y"
    { write_exp_elt_opcode (BINOP_EQUAL); 
			  current_type = builtin_type_bool;
			}
    break;

  case 36:
#line 425 "p-exp.y"
    { write_exp_elt_opcode (BINOP_NOTEQUAL); 
			  current_type = builtin_type_bool;
			}
    break;

  case 37:
#line 431 "p-exp.y"
    { write_exp_elt_opcode (BINOP_LEQ); 
			  current_type = builtin_type_bool;
			}
    break;

  case 38:
#line 437 "p-exp.y"
    { write_exp_elt_opcode (BINOP_GEQ); 
			  current_type = builtin_type_bool;
			}
    break;

  case 39:
#line 443 "p-exp.y"
    { write_exp_elt_opcode (BINOP_LESS); 
			  current_type = builtin_type_bool;
			}
    break;

  case 40:
#line 449 "p-exp.y"
    { write_exp_elt_opcode (BINOP_GTR); 
			  current_type = builtin_type_bool;
			}
    break;

  case 41:
#line 455 "p-exp.y"
    { write_exp_elt_opcode (BINOP_BITWISE_AND); }
    break;

  case 42:
#line 459 "p-exp.y"
    { write_exp_elt_opcode (BINOP_BITWISE_XOR); }
    break;

  case 43:
#line 463 "p-exp.y"
    { write_exp_elt_opcode (BINOP_BITWISE_IOR); }
    break;

  case 44:
#line 467 "p-exp.y"
    { write_exp_elt_opcode (BINOP_ASSIGN); }
    break;

  case 45:
#line 471 "p-exp.y"
    { write_exp_elt_opcode (OP_BOOL);
			  write_exp_elt_longcst ((LONGEST) (yyvsp[(1) - (1)].lval));
			  current_type = builtin_type_bool;
			  write_exp_elt_opcode (OP_BOOL); }
    break;

  case 46:
#line 478 "p-exp.y"
    { write_exp_elt_opcode (OP_BOOL);
			  write_exp_elt_longcst ((LONGEST) (yyvsp[(1) - (1)].lval));
			  current_type = builtin_type_bool;
			  write_exp_elt_opcode (OP_BOOL); }
    break;

  case 47:
#line 485 "p-exp.y"
    { write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type ((yyvsp[(1) - (1)].typed_val_int).type);
			  current_type = (yyvsp[(1) - (1)].typed_val_int).type;
			  write_exp_elt_longcst ((LONGEST)((yyvsp[(1) - (1)].typed_val_int).val));
			  write_exp_elt_opcode (OP_LONG); }
    break;

  case 48:
#line 493 "p-exp.y"
    { YYSTYPE val;
			  parse_number ((yyvsp[(1) - (1)].ssym).stoken.ptr, (yyvsp[(1) - (1)].ssym).stoken.length, 0, &val);
			  write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (val.typed_val_int.type);
			  current_type = val.typed_val_int.type;
			  write_exp_elt_longcst ((LONGEST)val.typed_val_int.val);
			  write_exp_elt_opcode (OP_LONG);
			}
    break;

  case 49:
#line 505 "p-exp.y"
    { write_exp_elt_opcode (OP_DOUBLE);
			  write_exp_elt_type ((yyvsp[(1) - (1)].typed_val_float).type);
			  current_type = (yyvsp[(1) - (1)].typed_val_float).type;
			  write_exp_elt_dblcst ((yyvsp[(1) - (1)].typed_val_float).dval);
			  write_exp_elt_opcode (OP_DOUBLE); }
    break;

  case 52:
#line 520 "p-exp.y"
    { write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (builtin_type_int);
			  CHECK_TYPEDEF ((yyvsp[(3) - (4)].tval));
			  write_exp_elt_longcst ((LONGEST) TYPE_LENGTH ((yyvsp[(3) - (4)].tval)));
			  write_exp_elt_opcode (OP_LONG); }
    break;

  case 53:
#line 528 "p-exp.y"
    { /* C strings are converted into array constants with
			     an explicit null byte added at the end.  Thus
			     the array upper bound is the string length.
			     There is no such thing in C as a completely empty
			     string. */
			  char *sp = (yyvsp[(1) - (1)].sval).ptr; int count = (yyvsp[(1) - (1)].sval).length;
			  while (count-- > 0)
			    {
			      write_exp_elt_opcode (OP_LONG);
			      write_exp_elt_type (builtin_type_char);
			      write_exp_elt_longcst ((LONGEST)(*sp++));
			      write_exp_elt_opcode (OP_LONG);
			    }
			  write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (builtin_type_char);
			  write_exp_elt_longcst ((LONGEST)'\0');
			  write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_opcode (OP_ARRAY);
			  write_exp_elt_longcst ((LONGEST) 0);
			  write_exp_elt_longcst ((LONGEST) ((yyvsp[(1) - (1)].sval).length));
			  write_exp_elt_opcode (OP_ARRAY); }
    break;

  case 54:
#line 553 "p-exp.y"
    { 
			  struct value * this_val;
			  struct type * this_type;
			  write_exp_elt_opcode (OP_THIS);
			  write_exp_elt_opcode (OP_THIS); 
			  /* we need type of this */
			  this_val = value_of_this (0); 
			  if (this_val)
			    this_type = value_type (this_val);
			  else
			    this_type = NULL;
			  if (this_type)
			    {
			      if (TYPE_CODE (this_type) == TYPE_CODE_PTR)
				{
				  this_type = TYPE_TARGET_TYPE (this_type);
				  write_exp_elt_opcode (UNOP_IND);
				}
			    }
		
			  current_type = this_type;
			}
    break;

  case 55:
#line 580 "p-exp.y"
    {
			  if ((yyvsp[(1) - (1)].ssym).sym != 0)
			      (yyval.bval) = SYMBOL_BLOCK_VALUE ((yyvsp[(1) - (1)].ssym).sym);
			  else
			    {
			      struct symtab *tem =
				  lookup_symtab (copy_name ((yyvsp[(1) - (1)].ssym).stoken));
			      if (tem)
				(yyval.bval) = BLOCKVECTOR_BLOCK (BLOCKVECTOR (tem), STATIC_BLOCK);
			      else
				error ("No file or function \"%s\".",
				       copy_name ((yyvsp[(1) - (1)].ssym).stoken));
			    }
			}
    break;

  case 56:
#line 597 "p-exp.y"
    { struct symbol *tem
			    = lookup_symbol (copy_name ((yyvsp[(3) - (3)].sval)), (yyvsp[(1) - (3)].bval),
					     VAR_DOMAIN, (int *) NULL,
					     (struct symtab **) NULL);
			  if (!tem || SYMBOL_CLASS (tem) != LOC_BLOCK)
			    error ("No function \"%s\" in specified context.",
				   copy_name ((yyvsp[(3) - (3)].sval)));
			  (yyval.bval) = SYMBOL_BLOCK_VALUE (tem); }
    break;

  case 57:
#line 608 "p-exp.y"
    { struct symbol *sym;
			  sym = lookup_symbol (copy_name ((yyvsp[(3) - (3)].sval)), (yyvsp[(1) - (3)].bval),
					       VAR_DOMAIN, (int *) NULL,
					       (struct symtab **) NULL);
			  if (sym == 0)
			    error ("No symbol \"%s\" in specified context.",
				   copy_name ((yyvsp[(3) - (3)].sval)));

			  write_exp_elt_opcode (OP_VAR_VALUE);
			  /* block_found is set by lookup_symbol.  */
			  write_exp_elt_block (block_found);
			  write_exp_elt_sym (sym);
			  write_exp_elt_opcode (OP_VAR_VALUE); }
    break;

  case 58:
#line 624 "p-exp.y"
    {
			  struct type *type = (yyvsp[(1) - (3)].tval);
			  if (TYPE_CODE (type) != TYPE_CODE_STRUCT
			      && TYPE_CODE (type) != TYPE_CODE_UNION)
			    error ("`%s' is not defined as an aggregate type.",
				   TYPE_NAME (type));

			  write_exp_elt_opcode (OP_SCOPE);
			  write_exp_elt_type (type);
			  write_exp_string ((yyvsp[(3) - (3)].sval));
			  write_exp_elt_opcode (OP_SCOPE);
			}
    break;

  case 60:
#line 640 "p-exp.y"
    {
			  char *name = copy_name ((yyvsp[(2) - (2)].sval));
			  struct symbol *sym;
			  struct minimal_symbol *msymbol;

			  sym =
			    lookup_symbol (name, (const struct block *) NULL,
					   VAR_DOMAIN, (int *) NULL,
					   (struct symtab **) NULL);
			  if (sym)
			    {
			      write_exp_elt_opcode (OP_VAR_VALUE);
			      write_exp_elt_block (NULL);
			      write_exp_elt_sym (sym);
			      write_exp_elt_opcode (OP_VAR_VALUE);
			      break;
			    }

			  msymbol = lookup_minimal_symbol (name, NULL, NULL);
			  if (msymbol != NULL)
			    {
			      write_exp_msymbol (msymbol,
						 lookup_function_type (builtin_type_int),
						 builtin_type_int);
			    }
			  else
			    if (!have_full_symbols () && !have_partial_symbols ())
			      error ("No symbol table is loaded.  Use the \"file\" command.");
			    else
			      error ("No symbol \"%s\" in current context.", name);
			}
    break;

  case 61:
#line 674 "p-exp.y"
    { struct symbol *sym = (yyvsp[(1) - (1)].ssym).sym;

			  if (sym)
			    {
			      if (symbol_read_needs_frame (sym))
				{
				  if (innermost_block == 0
				      || contained_in (block_found,
						       innermost_block))
				    innermost_block = block_found;
				}

			      write_exp_elt_opcode (OP_VAR_VALUE);
			      /* We want to use the selected frame, not
				 another more inner frame which happens to
				 be in the same block.  */
			      write_exp_elt_block (NULL);
			      write_exp_elt_sym (sym);
			      write_exp_elt_opcode (OP_VAR_VALUE);
			      current_type = sym->type; }
			  else if ((yyvsp[(1) - (1)].ssym).is_a_field_of_this)
			    {
			      struct value * this_val;
			      struct type * this_type;
			      /* Object pascal: it hangs off of `this'.  Must
			         not inadvertently convert from a method call
				 to data ref.  */
			      if (innermost_block == 0
				  || contained_in (block_found,
						   innermost_block))
				innermost_block = block_found;
			      write_exp_elt_opcode (OP_THIS);
			      write_exp_elt_opcode (OP_THIS);
			      write_exp_elt_opcode (STRUCTOP_PTR);
			      write_exp_string ((yyvsp[(1) - (1)].ssym).stoken);
			      write_exp_elt_opcode (STRUCTOP_PTR);
			      /* we need type of this */
			      this_val = value_of_this (0); 
			      if (this_val)
				this_type = value_type (this_val);
			      else
				this_type = NULL;
			      if (this_type)
				current_type = lookup_struct_elt_type (
				  this_type,
				  copy_name ((yyvsp[(1) - (1)].ssym).stoken), 0);
			      else
				current_type = NULL; 
			    }
			  else
			    {
			      struct minimal_symbol *msymbol;
			      char *arg = copy_name ((yyvsp[(1) - (1)].ssym).stoken);

			      msymbol =
				lookup_minimal_symbol (arg, NULL, NULL);
			      if (msymbol != NULL)
				{
				  write_exp_msymbol (msymbol,
						     lookup_function_type (builtin_type_int),
						     builtin_type_int);
				}
			      else if (!have_full_symbols () && !have_partial_symbols ())
				error ("No symbol table is loaded.  Use the \"file\" command.");
			      else
				error ("No symbol \"%s\" in current context.",
				       copy_name ((yyvsp[(1) - (1)].ssym).stoken));
			    }
			}
    break;

  case 64:
#line 762 "p-exp.y"
    { (yyval.tval) = lookup_pointer_type ((yyvsp[(2) - (2)].tval)); }
    break;

  case 65:
#line 764 "p-exp.y"
    { (yyval.tval) = (yyvsp[(1) - (1)].tsym).type; }
    break;

  case 66:
#line 766 "p-exp.y"
    { (yyval.tval) = lookup_struct (copy_name ((yyvsp[(2) - (2)].sval)),
					      expression_context_block); }
    break;

  case 67:
#line 769 "p-exp.y"
    { (yyval.tval) = lookup_struct (copy_name ((yyvsp[(2) - (2)].sval)),
					      expression_context_block); }
    break;

  case 68:
#line 776 "p-exp.y"
    { (yyval.sval) = (yyvsp[(1) - (1)].ssym).stoken; }
    break;

  case 69:
#line 777 "p-exp.y"
    { (yyval.sval) = (yyvsp[(1) - (1)].ssym).stoken; }
    break;

  case 70:
#line 778 "p-exp.y"
    { (yyval.sval) = (yyvsp[(1) - (1)].tsym).stoken; }
    break;

  case 71:
#line 779 "p-exp.y"
    { (yyval.sval) = (yyvsp[(1) - (1)].ssym).stoken; }
    break;


/* Line 1267 of yacc.c.  */
#line 2247 "p-exp.c.tmp"
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
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
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


#line 793 "p-exp.y"


/* Take care of parsing a number (anything that starts with a digit).
   Set yylval and return the token type; update lexptr.
   LEN is the number of characters in it.  */

/*** Needs some error checking for the float case ***/

static int
parse_number (p, len, parsed_float, putithere)
     char *p;
     int len;
     int parsed_float;
     YYSTYPE *putithere;
{
  /* FIXME: Shouldn't these be unsigned?  We don't deal with negative values
     here, and we do kind of silly things like cast to unsigned.  */
  LONGEST n = 0;
  LONGEST prevn = 0;
  ULONGEST un;

  int i = 0;
  int c;
  int base = input_radix;
  int unsigned_p = 0;

  /* Number of "L" suffixes encountered.  */
  int long_p = 0;

  /* We have found a "L" or "U" suffix.  */
  int found_suffix = 0;

  ULONGEST high_bit;
  struct type *signed_type;
  struct type *unsigned_type;

  if (parsed_float)
    {
      /* It's a float since it contains a point or an exponent.  */
      char c;
      int num = 0;	/* number of tokens scanned by scanf */
      char saved_char = p[len];

      p[len] = 0;	/* null-terminate the token */
      num = sscanf (p, "%" DOUBLEST_SCAN_FORMAT "%c",
		    &putithere->typed_val_float.dval, &c);
      p[len] = saved_char;	/* restore the input stream */
      if (num != 1) 		/* check scanf found ONLY a float ... */
	return ERROR;
      /* See if it has `f' or `l' suffix (float or long double).  */

      c = tolower (p[len - 1]);

      if (c == 'f')
	putithere->typed_val_float.type = builtin_type_float;
      else if (c == 'l')
	putithere->typed_val_float.type = builtin_type_long_double;
      else if (isdigit (c) || c == '.')
	putithere->typed_val_float.type = builtin_type_double;
      else
	return ERROR;

      return FLOAT;
    }

  /* Handle base-switching prefixes 0x, 0t, 0d, 0 */
  if (p[0] == '0')
    switch (p[1])
      {
      case 'x':
      case 'X':
	if (len >= 3)
	  {
	    p += 2;
	    base = 16;
	    len -= 2;
	  }
	break;

      case 't':
      case 'T':
      case 'd':
      case 'D':
	if (len >= 3)
	  {
	    p += 2;
	    base = 10;
	    len -= 2;
	  }
	break;

      default:
	base = 8;
	break;
      }

  while (len-- > 0)
    {
      c = *p++;
      if (c >= 'A' && c <= 'Z')
	c += 'a' - 'A';
      if (c != 'l' && c != 'u')
	n *= base;
      if (c >= '0' && c <= '9')
	{
	  if (found_suffix)
	    return ERROR;
	  n += i = c - '0';
	}
      else
	{
	  if (base > 10 && c >= 'a' && c <= 'f')
	    {
	      if (found_suffix)
		return ERROR;
	      n += i = c - 'a' + 10;
	    }
	  else if (c == 'l')
	    {
	      ++long_p;
	      found_suffix = 1;
	    }
	  else if (c == 'u')
	    {
	      unsigned_p = 1;
	      found_suffix = 1;
	    }
	  else
	    return ERROR;	/* Char not a digit */
	}
      if (i >= base)
	return ERROR;		/* Invalid digit in this base */

      /* Portably test for overflow (only works for nonzero values, so make
	 a second check for zero).  FIXME: Can't we just make n and prevn
	 unsigned and avoid this?  */
      if (c != 'l' && c != 'u' && (prevn >= n) && n != 0)
	unsigned_p = 1;		/* Try something unsigned */

      /* Portably test for unsigned overflow.
	 FIXME: This check is wrong; for example it doesn't find overflow
	 on 0x123456789 when LONGEST is 32 bits.  */
      if (c != 'l' && c != 'u' && n != 0)
	{	
	  if ((unsigned_p && (ULONGEST) prevn >= (ULONGEST) n))
	    error ("Numeric constant too large.");
	}
      prevn = n;
    }

  /* An integer constant is an int, a long, or a long long.  An L
     suffix forces it to be long; an LL suffix forces it to be long
     long.  If not forced to a larger size, it gets the first type of
     the above that it fits in.  To figure out whether it fits, we
     shift it right and see whether anything remains.  Note that we
     can't shift sizeof (LONGEST) * HOST_CHAR_BIT bits or more in one
     operation, because many compilers will warn about such a shift
     (which always produces a zero result).  Sometimes gdbarch_int_bit
     or gdbarch_long_bit will be that big, sometimes not.  To deal with
     the case where it is we just always shift the value more than
     once, with fewer bits each time.  */

  un = (ULONGEST)n >> 2;
  if (long_p == 0
      && (un >> (gdbarch_int_bit (current_gdbarch) - 2)) == 0)
    {
      high_bit = ((ULONGEST)1) << (gdbarch_int_bit (current_gdbarch) - 1);

      /* A large decimal (not hex or octal) constant (between INT_MAX
	 and UINT_MAX) is a long or unsigned long, according to ANSI,
	 never an unsigned int, but this code treats it as unsigned
	 int.  This probably should be fixed.  GCC gives a warning on
	 such constants.  */

      unsigned_type = builtin_type_unsigned_int;
      signed_type = builtin_type_int;
    }
  else if (long_p <= 1
	   && (un >> (gdbarch_long_bit (current_gdbarch) - 2)) == 0)
    {
      high_bit = ((ULONGEST)1) << (gdbarch_long_bit (current_gdbarch) - 1);
      unsigned_type = builtin_type_unsigned_long;
      signed_type = builtin_type_long;
    }
  else
    {
      int shift;
      if (sizeof (ULONGEST) * HOST_CHAR_BIT
	  < gdbarch_long_long_bit (current_gdbarch))
	/* A long long does not fit in a LONGEST.  */
	shift = (sizeof (ULONGEST) * HOST_CHAR_BIT - 1);
      else
	shift = (gdbarch_long_long_bit (current_gdbarch) - 1);
      high_bit = (ULONGEST) 1 << shift;
      unsigned_type = builtin_type_unsigned_long_long;
      signed_type = builtin_type_long_long;
    }

   putithere->typed_val_int.val = n;

   /* If the high bit of the worked out type is set then this number
      has to be unsigned. */

   if (unsigned_p || (n & high_bit))
     {
       putithere->typed_val_int.type = unsigned_type;
     }
   else
     {
       putithere->typed_val_int.type = signed_type;
     }

   return INT;
}


struct type_push
{
  struct type *stored;
  struct type_push *next;
};

static struct type_push *tp_top = NULL;

static void
push_current_type (void)
{
  struct type_push *tpnew;
  tpnew = (struct type_push *) xmalloc (sizeof (struct type_push));
  tpnew->next = tp_top;
  tpnew->stored = current_type;
  current_type = NULL;
  tp_top = tpnew; 
}

static void
pop_current_type (void)
{
  struct type_push *tp = tp_top;
  if (tp)
    {
      current_type = tp->stored;
      tp_top = tp->next;
      xfree (tp);
    }
}

struct token
{
  char *operator;
  int token;
  enum exp_opcode opcode;
};

static const struct token tokentab3[] =
  {
    {"shr", RSH, BINOP_END},
    {"shl", LSH, BINOP_END},
    {"and", ANDAND, BINOP_END},
    {"div", DIV, BINOP_END},
    {"not", NOT, BINOP_END},
    {"mod", MOD, BINOP_END},
    {"inc", INCREMENT, BINOP_END},
    {"dec", DECREMENT, BINOP_END},
    {"xor", XOR, BINOP_END}
  };

static const struct token tokentab2[] =
  {
    {"or", OR, BINOP_END},
    {"<>", NOTEQUAL, BINOP_END},
    {"<=", LEQ, BINOP_END},
    {">=", GEQ, BINOP_END},
    {":=", ASSIGN, BINOP_END},
    {"::", COLONCOLON, BINOP_END} };

/* Allocate uppercased var */
/* make an uppercased copy of tokstart */
static char * uptok (tokstart, namelen)
  char *tokstart;
  int namelen;
{
  int i;
  char *uptokstart = (char *)xmalloc(namelen+1);
  for (i = 0;i <= namelen;i++)
    {
      if ((tokstart[i]>='a' && tokstart[i]<='z'))
        uptokstart[i] = tokstart[i]-('a'-'A');
      else
        uptokstart[i] = tokstart[i];
    }
  uptokstart[namelen]='\0';
  return uptokstart;
}
/* Read one token, getting characters through lexptr.  */


static int
yylex ()
{
  int c;
  int namelen;
  unsigned int i;
  char *tokstart;
  char *uptokstart;
  char *tokptr;
  char *p;
  int explen, tempbufindex;
  static char *tempbuf;
  static int tempbufsize;

 retry:

  prev_lexptr = lexptr;

  tokstart = lexptr;
  explen = strlen (lexptr);
  /* See if it is a special token of length 3.  */
  if (explen > 2)
    for (i = 0; i < sizeof (tokentab3) / sizeof (tokentab3[0]); i++)
      if (strncasecmp (tokstart, tokentab3[i].operator, 3) == 0
          && (!isalpha (tokentab3[i].operator[0]) || explen == 3
              || (!isalpha (tokstart[3]) && !isdigit (tokstart[3]) && tokstart[3] != '_')))
        {
          lexptr += 3;
          yylval.opcode = tokentab3[i].opcode;
          return tokentab3[i].token;
        }

  /* See if it is a special token of length 2.  */
  if (explen > 1)
  for (i = 0; i < sizeof (tokentab2) / sizeof (tokentab2[0]); i++)
      if (strncasecmp (tokstart, tokentab2[i].operator, 2) == 0
          && (!isalpha (tokentab2[i].operator[0]) || explen == 2
              || (!isalpha (tokstart[2]) && !isdigit (tokstart[2]) && tokstart[2] != '_')))
        {
          lexptr += 2;
          yylval.opcode = tokentab2[i].opcode;
          return tokentab2[i].token;
        }

  switch (c = *tokstart)
    {
    case 0:
      return 0;

    case ' ':
    case '\t':
    case '\n':
      lexptr++;
      goto retry;

    case '\'':
      /* We either have a character constant ('0' or '\177' for example)
	 or we have a quoted symbol reference ('foo(int,int)' in object pascal
	 for example). */
      lexptr++;
      c = *lexptr++;
      if (c == '\\')
	c = parse_escape (&lexptr);
      else if (c == '\'')
	error ("Empty character constant.");

      yylval.typed_val_int.val = c;
      yylval.typed_val_int.type = builtin_type_char;

      c = *lexptr++;
      if (c != '\'')
	{
	  namelen = skip_quoted (tokstart) - tokstart;
	  if (namelen > 2)
	    {
	      lexptr = tokstart + namelen;
	      if (lexptr[-1] != '\'')
		error ("Unmatched single quote.");
	      namelen -= 2;
              tokstart++;
              uptokstart = uptok(tokstart,namelen);
	      goto tryname;
	    }
	  error ("Invalid character constant.");
	}
      return INT;

    case '(':
      paren_depth++;
      lexptr++;
      return c;

    case ')':
      if (paren_depth == 0)
	return 0;
      paren_depth--;
      lexptr++;
      return c;

    case ',':
      if (comma_terminates && paren_depth == 0)
	return 0;
      lexptr++;
      return c;

    case '.':
      /* Might be a floating point number.  */
      if (lexptr[1] < '0' || lexptr[1] > '9')
	goto symbol;		/* Nope, must be a symbol. */
      /* FALL THRU into number case.  */

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
	/* It's a number.  */
	int got_dot = 0, got_e = 0, toktype;
	char *p = tokstart;
	int hex = input_radix > 10;

	if (c == '0' && (p[1] == 'x' || p[1] == 'X'))
	  {
	    p += 2;
	    hex = 1;
	  }
	else if (c == '0' && (p[1]=='t' || p[1]=='T' || p[1]=='d' || p[1]=='D'))
	  {
	    p += 2;
	    hex = 0;
	  }

	for (;; ++p)
	  {
	    /* This test includes !hex because 'e' is a valid hex digit
	       and thus does not indicate a floating point number when
	       the radix is hex.  */
	    if (!hex && !got_e && (*p == 'e' || *p == 'E'))
	      got_dot = got_e = 1;
	    /* This test does not include !hex, because a '.' always indicates
	       a decimal floating point number regardless of the radix.  */
	    else if (!got_dot && *p == '.')
	      got_dot = 1;
	    else if (got_e && (p[-1] == 'e' || p[-1] == 'E')
		     && (*p == '-' || *p == '+'))
	      /* This is the sign of the exponent, not the end of the
		 number.  */
	      continue;
	    /* We will take any letters or digits.  parse_number will
	       complain if past the radix, or if L or U are not final.  */
	    else if ((*p < '0' || *p > '9')
		     && ((*p < 'a' || *p > 'z')
				  && (*p < 'A' || *p > 'Z')))
	      break;
	  }
	toktype = parse_number (tokstart, p - tokstart, got_dot|got_e, &yylval);
        if (toktype == ERROR)
	  {
	    char *err_copy = (char *) alloca (p - tokstart + 1);

	    memcpy (err_copy, tokstart, p - tokstart);
	    err_copy[p - tokstart] = 0;
	    error ("Invalid number \"%s\".", err_copy);
	  }
	lexptr = p;
	return toktype;
      }

    case '+':
    case '-':
    case '*':
    case '/':
    case '|':
    case '&':
    case '^':
    case '~':
    case '!':
    case '@':
    case '<':
    case '>':
    case '[':
    case ']':
    case '?':
    case ':':
    case '=':
    case '{':
    case '}':
    symbol:
      lexptr++;
      return c;

    case '"':

      /* Build the gdb internal form of the input string in tempbuf,
	 translating any standard C escape forms seen.  Note that the
	 buffer is null byte terminated *only* for the convenience of
	 debugging gdb itself and printing the buffer contents when
	 the buffer contains no embedded nulls.  Gdb does not depend
	 upon the buffer being null byte terminated, it uses the length
	 string instead.  This allows gdb to handle C strings (as well
	 as strings in other languages) with embedded null bytes */

      tokptr = ++tokstart;
      tempbufindex = 0;

      do {
	/* Grow the static temp buffer if necessary, including allocating
	   the first one on demand. */
	if (tempbufindex + 1 >= tempbufsize)
	  {
	    tempbuf = (char *) xrealloc (tempbuf, tempbufsize += 64);
	  }

	switch (*tokptr)
	  {
	  case '\0':
	  case '"':
	    /* Do nothing, loop will terminate. */
	    break;
	  case '\\':
	    tokptr++;
	    c = parse_escape (&tokptr);
	    if (c == -1)
	      {
		continue;
	      }
	    tempbuf[tempbufindex++] = c;
	    break;
	  default:
	    tempbuf[tempbufindex++] = *tokptr++;
	    break;
	  }
      } while ((*tokptr != '"') && (*tokptr != '\0'));
      if (*tokptr++ != '"')
	{
	  error ("Unterminated string in expression.");
	}
      tempbuf[tempbufindex] = '\0';	/* See note above */
      yylval.sval.ptr = tempbuf;
      yylval.sval.length = tempbufindex;
      lexptr = tokptr;
      return (STRING);
    }

  if (!(c == '_' || c == '$'
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
    /* We must have come across a bad character (e.g. ';').  */
    error ("Invalid character '%c' in expression.", c);

  /* It's a name.  See how long it is.  */
  namelen = 0;
  for (c = tokstart[namelen];
       (c == '_' || c == '$' || (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '<');)
    {
      /* Template parameter lists are part of the name.
	 FIXME: This mishandles `print $a<4&&$a>3'.  */
      if (c == '<')
	{
	  int i = namelen;
	  int nesting_level = 1;
	  while (tokstart[++i])
	    {
	      if (tokstart[i] == '<')
		nesting_level++;
	      else if (tokstart[i] == '>')
		{
		  if (--nesting_level == 0)
		    break;
		}
	    }
	  if (tokstart[i] == '>')
	    namelen = i;
	  else
	    break;
	}

      /* do NOT uppercase internals because of registers !!! */
      c = tokstart[++namelen];
    }

  uptokstart = uptok(tokstart,namelen);

  /* The token "if" terminates the expression and is NOT
     removed from the input stream.  */
  if (namelen == 2 && uptokstart[0] == 'I' && uptokstart[1] == 'F')
    {
      free (uptokstart);
      return 0;
    }

  lexptr += namelen;

  tryname:

  /* Catch specific keywords.  Should be done with a data structure.  */
  switch (namelen)
    {
    case 6:
      if (strcmp (uptokstart, "OBJECT") == 0)
	{
	  free (uptokstart);
	  return CLASS;
	}
      if (strcmp (uptokstart, "RECORD") == 0)
	{
	  free (uptokstart);
	  return STRUCT;
	}
      if (strcmp (uptokstart, "SIZEOF") == 0)
	{
	  free (uptokstart);
	  return SIZEOF;
	}
      break;
    case 5:
      if (strcmp (uptokstart, "CLASS") == 0)
	{
	  free (uptokstart);
	  return CLASS;
	}
      if (strcmp (uptokstart, "FALSE") == 0)
	{
          yylval.lval = 0;
	  free (uptokstart);
          return FALSEKEYWORD;
        }
      break;
    case 4:
      if (strcmp (uptokstart, "TRUE") == 0)
	{
          yylval.lval = 1;
	  free (uptokstart);
  	  return TRUEKEYWORD;
        }
      if (strcmp (uptokstart, "SELF") == 0)
        {
          /* here we search for 'this' like
             inserted in FPC stabs debug info */
	  static const char this_name[] = "this";

	  if (lookup_symbol (this_name, expression_context_block,
			     VAR_DOMAIN, (int *) NULL,
			     (struct symtab **) NULL))
	    {
	      free (uptokstart);
	      return THIS;
	    }
	}
      break;
    default:
      break;
    }

  yylval.sval.ptr = tokstart;
  yylval.sval.length = namelen;

  if (*tokstart == '$')
    {
      /* $ is the normal prefix for pascal hexadecimal values
        but this conflicts with the GDB use for debugger variables
        so in expression to enter hexadecimal values
        we still need to use C syntax with 0xff  */
      write_dollar_variable (yylval.sval);
      free (uptokstart);
      return VARIABLE;
    }

  /* Use token-type BLOCKNAME for symbols that happen to be defined as
     functions or symtabs.  If this is not so, then ...
     Use token-type TYPENAME for symbols that happen to be defined
     currently as names of types; NAME for other symbols.
     The caller is not constrained to care about the distinction.  */
  {
    char *tmp = copy_name (yylval.sval);
    struct symbol *sym;
    int is_a_field_of_this = 0;
    int is_a_field = 0;
    int hextype;


    if (search_field && current_type)
      is_a_field = (lookup_struct_elt_type (current_type, tmp, 1) != NULL);	
    if (is_a_field)
      sym = NULL;
    else
      sym = lookup_symbol (tmp, expression_context_block,
			   VAR_DOMAIN,
			   &is_a_field_of_this,
			   (struct symtab **) NULL);
    /* second chance uppercased (as Free Pascal does).  */
    if (!sym && !is_a_field_of_this && !is_a_field)
      {
       for (i = 0; i <= namelen; i++)
         {
           if ((tmp[i] >= 'a' && tmp[i] <= 'z'))
             tmp[i] -= ('a'-'A');
         }
       if (search_field && current_type)
	 is_a_field = (lookup_struct_elt_type (current_type, tmp, 1) != NULL);	
       if (is_a_field)
	 sym = NULL;
       else
	 sym = lookup_symbol (tmp, expression_context_block,
                        VAR_DOMAIN,
                        &is_a_field_of_this,
                        (struct symtab **) NULL);
       if (sym || is_a_field_of_this || is_a_field)
         for (i = 0; i <= namelen; i++)
           {
             if ((tokstart[i] >= 'a' && tokstart[i] <= 'z'))
               tokstart[i] -= ('a'-'A');
           }
      }
    /* Third chance Capitalized (as GPC does).  */
    if (!sym && !is_a_field_of_this && !is_a_field)
      {
       for (i = 0; i <= namelen; i++)
         {
           if (i == 0)
             {
              if ((tmp[i] >= 'a' && tmp[i] <= 'z'))
                tmp[i] -= ('a'-'A');
             }
           else
           if ((tmp[i] >= 'A' && tmp[i] <= 'Z'))
             tmp[i] -= ('A'-'a');
          }
       if (search_field && current_type)
	 is_a_field = (lookup_struct_elt_type (current_type, tmp, 1) != NULL);	
       if (is_a_field)
	 sym = NULL;
       else
	 sym = lookup_symbol (tmp, expression_context_block,
                         VAR_DOMAIN,
                         &is_a_field_of_this,
                         (struct symtab **) NULL);
       if (sym || is_a_field_of_this || is_a_field)
          for (i = 0; i <= namelen; i++)
            {
              if (i == 0)
                {
                  if ((tokstart[i] >= 'a' && tokstart[i] <= 'z'))
                    tokstart[i] -= ('a'-'A');
                }
              else
                if ((tokstart[i] >= 'A' && tokstart[i] <= 'Z'))
                  tokstart[i] -= ('A'-'a');
            }
      }

    if (is_a_field)
      {
	tempbuf = (char *) xrealloc (tempbuf, namelen + 1);
	strncpy (tempbuf, tokstart, namelen); tempbuf [namelen] = 0;
	yylval.sval.ptr = tempbuf;
	yylval.sval.length = namelen; 
	free (uptokstart);
	return FIELDNAME;
      } 
    /* Call lookup_symtab, not lookup_partial_symtab, in case there are
       no psymtabs (coff, xcoff, or some future change to blow away the
       psymtabs once once symbols are read).  */
    if ((sym && SYMBOL_CLASS (sym) == LOC_BLOCK)
        || lookup_symtab (tmp))
      {
	yylval.ssym.sym = sym;
	yylval.ssym.is_a_field_of_this = is_a_field_of_this;
	free (uptokstart);
	return BLOCKNAME;
      }
    if (sym && SYMBOL_CLASS (sym) == LOC_TYPEDEF)
        {
#if 1
	  /* Despite the following flaw, we need to keep this code enabled.
	     Because we can get called from check_stub_method, if we don't
	     handle nested types then it screws many operations in any
	     program which uses nested types.  */
	  /* In "A::x", if x is a member function of A and there happens
	     to be a type (nested or not, since the stabs don't make that
	     distinction) named x, then this code incorrectly thinks we
	     are dealing with nested types rather than a member function.  */

	  char *p;
	  char *namestart;
	  struct symbol *best_sym;

	  /* Look ahead to detect nested types.  This probably should be
	     done in the grammar, but trying seemed to introduce a lot
	     of shift/reduce and reduce/reduce conflicts.  It's possible
	     that it could be done, though.  Or perhaps a non-grammar, but
	     less ad hoc, approach would work well.  */

	  /* Since we do not currently have any way of distinguishing
	     a nested type from a non-nested one (the stabs don't tell
	     us whether a type is nested), we just ignore the
	     containing type.  */

	  p = lexptr;
	  best_sym = sym;
	  while (1)
	    {
	      /* Skip whitespace.  */
	      while (*p == ' ' || *p == '\t' || *p == '\n')
		++p;
	      if (*p == ':' && p[1] == ':')
		{
		  /* Skip the `::'.  */
		  p += 2;
		  /* Skip whitespace.  */
		  while (*p == ' ' || *p == '\t' || *p == '\n')
		    ++p;
		  namestart = p;
		  while (*p == '_' || *p == '$' || (*p >= '0' && *p <= '9')
			 || (*p >= 'a' && *p <= 'z')
			 || (*p >= 'A' && *p <= 'Z'))
		    ++p;
		  if (p != namestart)
		    {
		      struct symbol *cur_sym;
		      /* As big as the whole rest of the expression, which is
			 at least big enough.  */
		      char *ncopy = alloca (strlen (tmp)+strlen (namestart)+3);
		      char *tmp1;

		      tmp1 = ncopy;
		      memcpy (tmp1, tmp, strlen (tmp));
		      tmp1 += strlen (tmp);
		      memcpy (tmp1, "::", 2);
		      tmp1 += 2;
		      memcpy (tmp1, namestart, p - namestart);
		      tmp1[p - namestart] = '\0';
		      cur_sym = lookup_symbol (ncopy, expression_context_block,
					       VAR_DOMAIN, (int *) NULL,
					       (struct symtab **) NULL);
		      if (cur_sym)
			{
			  if (SYMBOL_CLASS (cur_sym) == LOC_TYPEDEF)
			    {
			      best_sym = cur_sym;
			      lexptr = p;
			    }
			  else
			    break;
			}
		      else
			break;
		    }
		  else
		    break;
		}
	      else
		break;
	    }

	  yylval.tsym.type = SYMBOL_TYPE (best_sym);
#else /* not 0 */
	  yylval.tsym.type = SYMBOL_TYPE (sym);
#endif /* not 0 */
	  free (uptokstart);
	  return TYPENAME;
        }
    yylval.tsym.type
      = language_lookup_primitive_type_by_name (current_language,
						current_gdbarch, tmp);
    if (yylval.tsym.type != NULL)
      {
	free (uptokstart);
	return TYPENAME;
      }

    /* Input names that aren't symbols but ARE valid hex numbers,
       when the input radix permits them, can be names or numbers
       depending on the parse.  Note we support radixes > 16 here.  */
    if (!sym
        && ((tokstart[0] >= 'a' && tokstart[0] < 'a' + input_radix - 10)
            || (tokstart[0] >= 'A' && tokstart[0] < 'A' + input_radix - 10)))
      {
 	YYSTYPE newlval;	/* Its value is ignored.  */
	hextype = parse_number (tokstart, namelen, 0, &newlval);
	if (hextype == INT)
	  {
	    yylval.ssym.sym = sym;
	    yylval.ssym.is_a_field_of_this = is_a_field_of_this;
	    free (uptokstart);
	    return NAME_OR_INT;
	  }
      }

    free(uptokstart);
    /* Any other kind of symbol */
    yylval.ssym.sym = sym;
    yylval.ssym.is_a_field_of_this = is_a_field_of_this;
    return NAME;
  }
}

void
yyerror (msg)
     char *msg;
{
  if (prev_lexptr)
    lexptr = prev_lexptr;

  error ("A %s in expression, near `%s'.", (msg ? msg : "error"), lexptr);
}

