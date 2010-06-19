
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
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "levcomp.ypp"


#include <map>
#include <algorithm>

#include "AppHdr.h"
#include "l_defs.h"
#include "libutil.h"
#include "mapdef.h"
#include "maps.h"
#include "stuff.h"

#define YYERROR_VERBOSE 1

int yylex();

extern int yylineno;

void yyerror(const char *e)
{
    if (strstr(e, lc_desfile.c_str()) == e)
        fprintf(stderr, "%s\n", e);
    else
        fprintf(stderr, "%s:%d: %s\n", lc_desfile.c_str(), yylineno, e);
    // Bail bail bail.
    end(1);
}

level_range set_range(const char *s, int start, int end)
{
    try
    {
        lc_range.set(s, start, end);
    }
    catch (const std::string &err)
    {
        yyerror(err.c_str());
    }
    return (lc_range);
}



/* Line 189 of yacc.c  */
#line 117 "levcomp.tab.c"

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
     DEFAULT_DEPTH = 258,
     SHUFFLE = 259,
     SUBST = 260,
     TAGS = 261,
     KFEAT = 262,
     KITEM = 263,
     KMONS = 264,
     KMASK = 265,
     KPROP = 266,
     NAME = 267,
     DEPTH = 268,
     ORIENT = 269,
     PLACE = 270,
     CHANCE = 271,
     WEIGHT = 272,
     MONS = 273,
     ITEM = 274,
     MARKER = 275,
     COLOUR = 276,
     PRELUDE = 277,
     MAIN = 278,
     VALIDATE = 279,
     VETO = 280,
     EPILOGUE = 281,
     NSUBST = 282,
     WELCOME = 283,
     LFLAGS = 284,
     BFLAGS = 285,
     LFLOORCOL = 286,
     LROCKCOL = 287,
     LFLOORTILE = 288,
     LROCKTILE = 289,
     FTILE = 290,
     RTILE = 291,
     TILE = 292,
     SUBVAULT = 293,
     FHEIGHT = 294,
     COMMA = 295,
     COLON = 296,
     PERC = 297,
     INTEGER = 298,
     CHARACTER = 299,
     STRING = 300,
     MAP_LINE = 301,
     MONSTER_NAME = 302,
     ITEM_INFO = 303,
     LUA_LINE = 304
   };
#endif
/* Tokens.  */
#define DEFAULT_DEPTH 258
#define SHUFFLE 259
#define SUBST 260
#define TAGS 261
#define KFEAT 262
#define KITEM 263
#define KMONS 264
#define KMASK 265
#define KPROP 266
#define NAME 267
#define DEPTH 268
#define ORIENT 269
#define PLACE 270
#define CHANCE 271
#define WEIGHT 272
#define MONS 273
#define ITEM 274
#define MARKER 275
#define COLOUR 276
#define PRELUDE 277
#define MAIN 278
#define VALIDATE 279
#define VETO 280
#define EPILOGUE 281
#define NSUBST 282
#define WELCOME 283
#define LFLAGS 284
#define BFLAGS 285
#define LFLOORCOL 286
#define LROCKCOL 287
#define LFLOORTILE 288
#define LROCKTILE 289
#define FTILE 290
#define RTILE 291
#define TILE 292
#define SUBVAULT 293
#define FHEIGHT 294
#define COMMA 295
#define COLON 296
#define PERC 297
#define INTEGER 298
#define CHARACTER 299
#define STRING 300
#define MAP_LINE 301
#define MONSTER_NAME 302
#define ITEM_INFO 303
#define LUA_LINE 304




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 45 "levcomp.ypp"

    int i;
    const char *text;
    raw_range range;



/* Line 214 of yacc.c  */
#line 259 "levcomp.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 271 "levcomp.tab.c"

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
#define YYLAST   113

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  87
/* YYNRULES -- Number of rules.  */
#define YYNRULES  162
/* YYNRULES -- Number of states.  */
#define YYNSTATES  193

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   304

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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      19,    22,    25,    26,    29,    31,    33,    36,    38,    40,
      42,    44,    46,    48,    50,    52,    54,    56,    58,    60,
      62,    64,    66,    68,    70,    72,    74,    76,    78,    80,
      82,    84,    86,    88,    90,    92,    94,    96,    98,   100,
     102,   104,   106,   108,   111,   112,   115,   117,   120,   121,
     124,   126,   129,   130,   133,   135,   138,   139,   142,   144,
     147,   148,   151,   153,   156,   157,   160,   162,   164,   167,
     169,   172,   174,   177,   179,   182,   184,   187,   190,   192,
     196,   198,   201,   202,   205,   207,   210,   213,   216,   219,
     221,   224,   226,   229,   231,   234,   236,   239,   242,   244,
     248,   250,   253,   255,   259,   261,   264,   266,   270,   272,
     275,   277,   281,   283,   285,   289,   291,   294,   296,   300,
     302,   305,   307,   311,   313,   315,   318,   322,   324,   326,
     328,   331,   335,   337,   339,   342,   344,   347,   353,   358,
     362,   365,   368,   370,   373,   376,   378,   381,   383,   385,
     388,   390,   394
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      51,     0,    -1,    52,    -1,    -1,    52,    53,    -1,    55,
      -1,    57,    -1,    44,    -1,    56,    -1,    62,    -1,     3,
      45,    -1,    60,    58,    -1,    -1,    58,    59,    -1,    61,
      -1,   131,    -1,    12,    45,    -1,   125,    -1,   126,    -1,
     127,    -1,   128,    -1,   129,    -1,   130,    -1,   122,    -1,
     119,    -1,    93,    -1,   116,    -1,   113,    -1,    94,    -1,
      95,    -1,    96,    -1,    97,    -1,    98,    -1,   102,    -1,
     105,    -1,   108,    -1,    85,    -1,    88,    -1,    91,    -1,
      92,    -1,    80,    -1,    82,    -1,    81,    -1,    83,    -1,
      84,    -1,    99,    -1,   134,    -1,    65,    -1,    74,    -1,
      68,    -1,    71,    -1,    77,    -1,    54,    -1,    23,    63,
      -1,    -1,    63,    64,    -1,    49,    -1,    23,    66,    -1,
      -1,    66,    67,    -1,    49,    -1,    24,    69,    -1,    -1,
      69,    70,    -1,    49,    -1,    25,    72,    -1,    -1,    72,
      73,    -1,    49,    -1,    22,    75,    -1,    -1,    75,    76,
      -1,    49,    -1,    26,    78,    -1,    -1,    78,    79,    -1,
      49,    -1,     7,    -1,     7,    45,    -1,     9,    -1,     9,
      45,    -1,     8,    -1,     8,    45,    -1,    10,    -1,    10,
      45,    -1,    11,    -1,    11,    45,    -1,     4,    86,    -1,
      87,    -1,    86,    40,    87,    -1,    48,    -1,     6,    89,
      -1,    -1,    89,    90,    -1,    45,    -1,    29,    45,    -1,
      30,    45,    -1,    20,    45,    -1,    21,   111,    -1,    31,
      -1,    31,    45,    -1,    32,    -1,    32,    45,    -1,    33,
      -1,    33,    45,    -1,    34,    -1,    34,    45,    -1,    39,
     100,    -1,   101,    -1,   100,    40,   101,    -1,    48,    -1,
      35,   103,    -1,   104,    -1,   103,    40,   104,    -1,    48,
      -1,    36,   106,    -1,   107,    -1,   106,    40,   107,    -1,
      48,    -1,    37,   109,    -1,   110,    -1,   109,    40,   110,
      -1,    48,    -1,   112,    -1,   111,    40,   112,    -1,    48,
      -1,    27,   114,    -1,   115,    -1,   114,    40,   115,    -1,
      48,    -1,     5,   117,    -1,   118,    -1,   118,    40,   117,
      -1,    48,    -1,    19,    -1,    19,   120,    -1,   120,    40,
     121,    -1,   121,    -1,    48,    -1,    18,    -1,    18,   123,
      -1,   124,    40,   123,    -1,   124,    -1,    47,    -1,    15,
      45,    -1,    13,    -1,    13,    45,    -1,    16,    43,    41,
      43,    42,    -1,    16,    43,    41,    43,    -1,    16,    43,
      42,    -1,    16,    43,    -1,    17,    43,    -1,    14,    -1,
      14,    45,    -1,    28,    45,    -1,   132,    -1,   132,   133,
      -1,   133,    -1,    46,    -1,    38,   135,    -1,   136,    -1,
     135,    40,   136,    -1,    45,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    67,    67,    70,    71,    74,    75,    78,    84,    85,
      88,    98,   118,   119,   122,   123,   126,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   188,   190,   191,   194,   199,   201,   202,
     205,   210,   212,   213,   216,   221,   223,   224,   227,   232,
     234,   235,   238,   243,   245,   246,   249,   254,   255,   263,
     264,   272,   273,   281,   282,   290,   291,   299,   302,   303,
     306,   314,   317,   318,   321,   330,   339,   348,   381,   384,
     385,   393,   394,   402,   403,   412,   413,   422,   424,   425,
     428,   437,   440,   441,   444,   453,   456,   457,   460,   469,
     472,   473,   476,   486,   487,   490,   499,   502,   503,   506,
     515,   518,   519,   522,   531,   532,   535,   536,   539,   547,
     548,   551,   552,   555,   564,   573,   574,   583,   590,   597,
     604,   612,   620,   621,   630,   639,   642,   643,   646,   655,
     658,   659,   662
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DEFAULT_DEPTH", "SHUFFLE", "SUBST",
  "TAGS", "KFEAT", "KITEM", "KMONS", "KMASK", "KPROP", "NAME", "DEPTH",
  "ORIENT", "PLACE", "CHANCE", "WEIGHT", "MONS", "ITEM", "MARKER",
  "COLOUR", "PRELUDE", "MAIN", "VALIDATE", "VETO", "EPILOGUE", "NSUBST",
  "WELCOME", "LFLAGS", "BFLAGS", "LFLOORCOL", "LROCKCOL", "LFLOORTILE",
  "LROCKTILE", "FTILE", "RTILE", "TILE", "SUBVAULT", "FHEIGHT", "COMMA",
  "COLON", "PERC", "INTEGER", "CHARACTER", "STRING", "MAP_LINE",
  "MONSTER_NAME", "ITEM_INFO", "LUA_LINE", "$accept", "file",
  "definitions", "definition", "error_seq", "def", "defdepth", "level",
  "map_specs", "map_spec", "name", "metaline", "global_lua",
  "global_lua_lines", "global_lua_line", "main_lua", "main_lua_lines",
  "main_lua_line", "validate_lua", "validate_lua_lines",
  "validate_lua_line", "veto_lua", "veto_lua_lines", "veto_lua_line",
  "prelude_lua", "prelude_lua_lines", "prelude_lua_line", "epilogue_lua",
  "epilogue_lua_lines", "epilogue_lua_line", "kfeat", "kmons", "kitem",
  "kmask", "kprop", "shuffle", "shuffle_specifiers", "shuffle_spec",
  "tags", "tagstrings", "tagstring", "lflags", "bflags", "marker",
  "colour", "lfloorcol", "lrockcol", "lfloortile", "lrocktile", "fheight",
  "fheight_specifiers", "fheight_specifier", "ftile", "ftile_specifiers",
  "ftile_specifier", "rtile", "rtile_specifiers", "rtile_specifier",
  "tile", "tile_specifiers", "tile_specifier", "colour_specifiers",
  "colour_specifier", "nsubst", "nsubst_specifiers", "nsubst_spec",
  "subst", "subst_specifiers", "subst_spec", "items", "item_specifiers",
  "item_specifier", "mons", "mnames", "mname", "place", "depth", "chance",
  "weight", "orientation", "welcome", "map_def", "map_lines", "map_line",
  "subvault", "subvault_specifiers", "subvault_specifier", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    50,    51,    52,    52,    53,    53,    54,    55,    55,
      56,    57,    58,    58,    59,    59,    60,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    62,    63,    63,    64,    65,    66,    66,
      67,    68,    69,    69,    70,    71,    72,    72,    73,    74,
      75,    75,    76,    77,    78,    78,    79,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    84,    85,    86,    86,
      87,    88,    89,    89,    90,    91,    92,    93,    94,    95,
      95,    96,    96,    97,    97,    98,    98,    99,   100,   100,
     101,   102,   103,   103,   104,   105,   106,   106,   107,   108,
     109,   109,   110,   111,   111,   112,   113,   114,   114,   115,
     116,   117,   117,   118,   119,   119,   120,   120,   121,   122,
     122,   123,   123,   124,   125,   126,   126,   127,   127,   127,
     127,   128,   129,   129,   130,   131,   132,   132,   133,   134,
     135,   135,   136
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       2,     2,     0,     2,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     0,     2,     1,     2,     0,     2,
       1,     2,     0,     2,     1,     2,     0,     2,     1,     2,
       0,     2,     1,     2,     0,     2,     1,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     2,     1,     3,
       1,     2,     0,     2,     1,     2,     2,     2,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     2,     1,     3,
       1,     2,     1,     3,     1,     2,     1,     3,     1,     2,
       1,     3,     1,     1,     3,     1,     2,     1,     3,     1,
       2,     1,     3,     1,     1,     2,     3,     1,     1,     1,
       2,     3,     1,     1,     2,     1,     2,     5,     4,     3,
       2,     2,     1,     2,     2,     1,     2,     1,     1,     2,
       1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,    54,     4,     5,     8,
       6,    12,     9,    10,    16,    53,    11,    56,    55,     0,
       0,    92,    77,    81,    79,    83,    85,   145,   152,     0,
       0,     0,   139,   134,     0,     0,    70,    58,    62,    66,
      74,     0,     0,     0,     0,    99,   101,   103,   105,     0,
       0,     0,     0,     0,     7,   158,    52,    13,    14,    47,
      49,    50,    48,    51,    40,    42,    41,    43,    44,    36,
      37,    38,    39,    25,    28,    29,    30,    31,    32,    45,
      33,    34,    35,    27,    26,    24,    23,    17,    18,    19,
      20,    21,    22,    15,   155,   157,    46,    90,    87,    88,
     133,   130,   131,    91,    78,    82,    80,    84,    86,   146,
     153,   144,   150,   151,   143,   140,   142,   138,   135,   137,
      97,   125,    98,   123,    69,    57,    61,    65,    73,   129,
     126,   127,   154,    95,    96,   100,   102,   104,   106,   114,
     111,   112,   118,   115,   116,   122,   119,   120,   162,   159,
     160,   110,   107,   108,   156,     0,     0,    94,    93,     0,
     149,     0,     0,     0,    72,    71,    60,    59,    64,    63,
      68,    67,    76,    75,     0,     0,     0,     0,     0,     0,
      89,   132,   148,   141,   136,   124,   128,   113,   117,   121,
     161,   109,   147
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     7,    56,     8,     9,    10,    16,    57,
      11,    58,    12,    15,    18,    59,   125,   167,    60,   126,
     169,    61,   127,   171,    62,   124,   165,    63,   128,   173,
      64,    65,    66,    67,    68,    69,    98,    99,    70,   103,
     158,    71,    72,    73,    74,    75,    76,    77,    78,    79,
     152,   153,    80,   140,   141,    81,   143,   144,    82,   146,
     147,   122,   123,    83,   130,   131,    84,   101,   102,    85,
     118,   119,    86,   115,   116,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,   149,   150
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -107
static const yytype_int8 yypact[] =
{
    -107,     8,    33,  -107,    -8,    -2,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,    -5,    -4,  -107,  -107,    -7,
      -1,  -107,     1,     3,     4,     5,     6,     7,     9,    10,
      14,    15,    12,    13,    17,    16,  -107,  -107,  -107,  -107,
    -107,    18,    20,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    34,    32,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,    35,  -107,  -107,  -107,    42,  -107,
    -107,  -107,    43,    39,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,    -3,  -107,  -107,  -107,    45,  -107,    46,  -107,
    -107,  -107,    47,  -107,    11,    40,    41,    44,    48,  -107,
      51,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
      52,  -107,  -107,    54,  -107,  -107,    55,  -107,  -107,    56,
    -107,  -107,    58,  -107,  -107,    -7,    -1,  -107,  -107,    31,
    -107,    12,    13,    16,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,    18,    28,    29,    30,    34,    32,
    -107,  -107,    21,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -102,  -107,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -106,  -107,  -107,  -100,  -107,  -107,   -88,  -107,  -107,
     -78,  -107,   -63,  -107,  -107,   -73,  -107,   -54,  -107,  -107,
    -107,   -59,  -107,   -57,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,    19,  -107,  -107,   -72
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      19,    20,    21,    22,    23,    24,    25,    26,     3,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,     4,    13,   159,   160,
      54,    97,    55,    14,    17,     5,   104,   100,   105,   106,
     107,   108,   109,   180,   110,   111,     6,   112,   113,   114,
     164,   117,   120,   192,   121,   132,   129,   133,   134,   135,
     136,   137,   138,   191,   182,   187,   139,   142,   145,   148,
     151,    55,   155,   156,   157,   161,   162,   163,   188,   166,
     168,   174,   175,   170,   176,   177,   178,   172,   179,   189,
     185,   186,   181,   184,   183,     0,   190,     0,     0,     0,
       0,     0,     0,   154
};

static const yytype_int16 yycheck[] =
{
       4,     5,     6,     7,     8,     9,    10,    11,     0,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,     3,    45,    41,    42,
      44,    48,    46,    45,    49,    12,    45,    48,    45,    45,
      45,    45,    45,   155,    45,    45,    23,    43,    43,    47,
      49,    48,    45,    42,    48,    45,    48,    45,    45,    45,
      45,    45,    45,   179,    43,   175,    48,    48,    48,    45,
      48,    46,    40,    40,    45,    40,    40,    40,   176,    49,
      49,    40,    40,    49,    40,    40,    40,    49,    40,   177,
     163,   174,   156,   162,   161,    -1,   178,    -1,    -1,    -1,
      -1,    -1,    -1,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    51,    52,     0,     3,    12,    23,    53,    55,    56,
      57,    60,    62,    45,    45,    63,    58,    49,    64,     4,
       5,     6,     7,     8,     9,    10,    11,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    44,    46,    54,    59,    61,    65,
      68,    71,    74,    77,    80,    81,    82,    83,    84,    85,
      88,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     102,   105,   108,   113,   116,   119,   122,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,    48,    86,    87,
      48,   117,   118,    89,    45,    45,    45,    45,    45,    45,
      45,    45,    43,    43,    47,   123,   124,    48,   120,   121,
      45,    48,   111,   112,    75,    66,    69,    72,    78,    48,
     114,   115,    45,    45,    45,    45,    45,    45,    45,    48,
     103,   104,    48,   106,   107,    48,   109,   110,    45,   135,
     136,    48,   100,   101,   133,    40,    40,    45,    90,    41,
      42,    40,    40,    40,    49,    76,    49,    67,    49,    70,
      49,    73,    49,    79,    40,    40,    40,    40,    40,    40,
      87,   117,    43,   123,   121,   112,   115,   104,   107,   110,
     136,   101,    42
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



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
#line 67 "levcomp.ypp"
    { }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 70 "levcomp.ypp"
    {}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 71 "levcomp.ypp"
    {}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 74 "levcomp.ypp"
    {}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 75 "levcomp.ypp"
    {}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 79 "levcomp.ypp"
    {
                    yyerror("Unexpected character sequence.");
                }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 84 "levcomp.ypp"
    {}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 85 "levcomp.ypp"
    {}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 89 "levcomp.ypp"
    {
                    dgn_reset_default_depth();
                    std::string err = dgn_set_default_depth((yyvsp[(2) - (2)].text));
                    if (!err.empty())
                        yyerror(make_stringf("Bad default-depth: %s (%s)",
                                (yyvsp[(2) - (2)].text), err.c_str()).c_str());
                }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 99 "levcomp.ypp"
    {
                    lc_map.set_file(lc_desfile);

                    if (lc_run_global_prelude && !lc_global_prelude.empty())
                    {
                        lc_global_prelude.set_file(lc_desfile);
                        lc_run_global_prelude = false;
                        if (lc_global_prelude.load_call(dlua, NULL))
                            yyerror( lc_global_prelude.orig_error().c_str() );
                    }

                    std::string err =
                        lc_map.validate_map_def(lc_default_depths);
                    if (!err.empty())
                        yyerror(err.c_str());
                    add_parsed_map(lc_map);
                }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 118 "levcomp.ypp"
    { }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 119 "levcomp.ypp"
    { }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 122 "levcomp.ypp"
    { }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 123 "levcomp.ypp"
    { }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 127 "levcomp.ypp"
    {
                    lc_map.init();
                    lc_map.name = (yyvsp[(2) - (2)].text);

                    map_load_info_t::const_iterator i =
                        lc_loaded_maps.find((yyvsp[(2) - (2)].text));

                    if (i != lc_loaded_maps.end())
                    {
                        yyerror(
                            make_stringf(
                                "Map named '%s' already loaded at %s:%d",
                                (yyvsp[(2) - (2)].text),
                                i->second.filename.c_str(),
                                i->second.lineno).c_str() );
                    }

                    lc_map.place_loaded_from =
                        map_file_place(lc_desfile, yylineno);
                    lc_loaded_maps[(yyvsp[(2) - (2)].text)] = lc_map.place_loaded_from;
                }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 185 "levcomp.ypp"
    {}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 188 "levcomp.ypp"
    { }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 190 "levcomp.ypp"
    { }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 191 "levcomp.ypp"
    { }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 195 "levcomp.ypp"
    {
                    lc_global_prelude.add(yylineno, (yyvsp[(1) - (1)].text));
                }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 199 "levcomp.ypp"
    { }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 201 "levcomp.ypp"
    { }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 202 "levcomp.ypp"
    { }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 206 "levcomp.ypp"
    {
                    lc_map.main.add(yylineno, (yyvsp[(1) - (1)].text));
                }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 210 "levcomp.ypp"
    { }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 212 "levcomp.ypp"
    { }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 213 "levcomp.ypp"
    { }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 217 "levcomp.ypp"
    {
                    lc_map.validate.add(yylineno, (yyvsp[(1) - (1)].text));
                }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 221 "levcomp.ypp"
    { }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 223 "levcomp.ypp"
    { }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 224 "levcomp.ypp"
    { }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 228 "levcomp.ypp"
    {
                    lc_map.veto.add(yylineno, (yyvsp[(1) - (1)].text));
                }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 232 "levcomp.ypp"
    { }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 234 "levcomp.ypp"
    { }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 235 "levcomp.ypp"
    { }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 239 "levcomp.ypp"
    {
                    lc_map.prelude.add(yylineno, (yyvsp[(1) - (1)].text));
                }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 243 "levcomp.ypp"
    { }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 245 "levcomp.ypp"
    { }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 246 "levcomp.ypp"
    { }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 250 "levcomp.ypp"
    {
                    lc_map.epilogue.add(yylineno, (yyvsp[(1) - (1)].text));
                }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 254 "levcomp.ypp"
    { }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 256 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("kfeat(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 263 "levcomp.ypp"
    { }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 265 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("kmons(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 272 "levcomp.ypp"
    { }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 274 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("kitem(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 281 "levcomp.ypp"
    { }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 283 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("kmask(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 290 "levcomp.ypp"
    { }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 292 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("kprop(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 299 "levcomp.ypp"
    {}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 307 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("shuffle(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 314 "levcomp.ypp"
    {}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 322 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("tags(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 331 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("lflags(\"%s\")",
                                     quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 340 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("bflags(\"%s\")",
                                     quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 349 "levcomp.ypp"
    {
                    std::string key, arg;
                    int sep(0);

                    const std::string err =
                      mapdef_split_key_item((yyvsp[(2) - (2)].text), &key, &sep, &arg);

                    if (!err.empty())
                      yyerror(err.c_str());

                    // Special treatment for Lua markers.
                    if (arg.find("lua:") == 0)
                    {
                       arg = arg.substr(4);
                       lc_map.main.add(
                         yylineno,
                         make_stringf("lua_marker(\"%s\", function () "
                                      "  return %s "
                                      "end)",
                                      quote_lua_string(key).c_str(),
                                      arg.c_str()));
                    }
                    else
                    {
                       lc_map.main.add(
                         yylineno,
                         make_stringf("marker(\"%s\")",
                                      quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                    }
                }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 381 "levcomp.ypp"
    { }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 384 "levcomp.ypp"
    { }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 386 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("lfloorcol(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 393 "levcomp.ypp"
    { }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 395 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("lrockcol(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 402 "levcomp.ypp"
    { }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 404 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("lfloortile(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 412 "levcomp.ypp"
    { }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 414 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("lrocktile(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 429 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("fheight(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 445 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("ftile(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 461 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("rtile(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 477 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("tile(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 486 "levcomp.ypp"
    { }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 487 "levcomp.ypp"
    { }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 491 "levcomp.ypp"
    {
                      lc_map.main.add(
                          yylineno,
                          make_stringf("colour(\"%s\")",
                              quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                  }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 499 "levcomp.ypp"
    { }
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 502 "levcomp.ypp"
    { }
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 503 "levcomp.ypp"
    { }
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 507 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("nsubst(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 515 "levcomp.ypp"
    { }
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 523 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("subst(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 531 "levcomp.ypp"
    {}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 532 "levcomp.ypp"
    {}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 540 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("item(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 547 "levcomp.ypp"
    {}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 548 "levcomp.ypp"
    {}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 556 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("mons(\"%s\")",
                            quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 565 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("place(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 573 "levcomp.ypp"
    {}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 575 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("depth(\"%s\")",
                            quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 584 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("chance(%d, %d)", (yyvsp[(2) - (5)].i), (yyvsp[(4) - (5)].i) * 100));
                }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 591 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("chance(%d, %d)", (yyvsp[(2) - (4)].i), (yyvsp[(4) - (4)].i)));
                }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 598 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("chance(0, %d)", (yyvsp[(2) - (3)].i) * 100));
                }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 605 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("chance(0, %d)", (yyvsp[(2) - (2)].i)));
                }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 613 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("weight(%d)", (yyvsp[(2) - (2)].i)));
                }
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 620 "levcomp.ypp"
    {}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 622 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("orient(\"%s\")",
                                     quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 631 "levcomp.ypp"
    {
                    lc_map.main.add(
                        yylineno,
                        make_stringf("welcome(\"%s\")",
                                     quote_lua_string((yyvsp[(2) - (2)].text)).c_str()));
                }
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 647 "levcomp.ypp"
    {
                    lc_map.mapchunk.add(
                        yylineno,
                        make_stringf("map(\"%s\")",
                                     quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                }
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 663 "levcomp.ypp"
    {
                       lc_map.main.add(
                           yylineno,
                           make_stringf("subvault(\"%s\")",
                               quote_lua_string((yyvsp[(1) - (1)].text)).c_str()));
                   }
    break;



/* Line 1455 of yacc.c  */
#line 2610 "levcomp.tab.c"
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
#line 671 "levcomp.ypp"


