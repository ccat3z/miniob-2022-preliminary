/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 5 "yacc_sql.y"


#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
	char id[MAX_NUM];
} ParserContext;

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->ssql->sstr.insertion.value_num = 0;
  context->ssql->sstr.errors = str;
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)


#line 118 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_NULL_VALUE = 8,                 /* NULL_VALUE  */
  YYSYMBOL_NULLABLE = 9,                   /* NULLABLE  */
  YYSYMBOL_INDEX = 10,                     /* INDEX  */
  YYSYMBOL_SELECT = 11,                    /* SELECT  */
  YYSYMBOL_DESC = 12,                      /* DESC  */
  YYSYMBOL_SHOW = 13,                      /* SHOW  */
  YYSYMBOL_SYNC = 14,                      /* SYNC  */
  YYSYMBOL_INSERT = 15,                    /* INSERT  */
  YYSYMBOL_DELETE = 16,                    /* DELETE  */
  YYSYMBOL_UPDATE = 17,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 18,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 19,                    /* RBRACE  */
  YYSYMBOL_COMMA = 20,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 21,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 22,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 23,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 24,                     /* INT_T  */
  YYSYMBOL_STRING_T = 25,                  /* STRING_T  */
  YYSYMBOL_DATE_T = 26,                    /* DATE_T  */
  YYSYMBOL_FLOAT_T = 27,                   /* FLOAT_T  */
  YYSYMBOL_TEXT_T = 28,                    /* TEXT_T  */
  YYSYMBOL_HELP = 29,                      /* HELP  */
  YYSYMBOL_EXIT = 30,                      /* EXIT  */
  YYSYMBOL_DOT = 31,                       /* DOT  */
  YYSYMBOL_INTO = 32,                      /* INTO  */
  YYSYMBOL_VALUES = 33,                    /* VALUES  */
  YYSYMBOL_FROM = 34,                      /* FROM  */
  YYSYMBOL_WHERE = 35,                     /* WHERE  */
  YYSYMBOL_IS = 36,                        /* IS  */
  YYSYMBOL_AND = 37,                       /* AND  */
  YYSYMBOL_SET = 38,                       /* SET  */
  YYSYMBOL_ON = 39,                        /* ON  */
  YYSYMBOL_LOAD = 40,                      /* LOAD  */
  YYSYMBOL_DATA = 41,                      /* DATA  */
  YYSYMBOL_INFILE = 42,                    /* INFILE  */
  YYSYMBOL_EQ = 43,                        /* EQ  */
  YYSYMBOL_LT = 44,                        /* LT  */
  YYSYMBOL_GT = 45,                        /* GT  */
  YYSYMBOL_LE = 46,                        /* LE  */
  YYSYMBOL_GE = 47,                        /* GE  */
  YYSYMBOL_NE = 48,                        /* NE  */
  YYSYMBOL_NOT = 49,                       /* NOT  */
  YYSYMBOL_LIKE = 50,                      /* LIKE  */
  YYSYMBOL_UNIQUE = 51,                    /* UNIQUE  */
  YYSYMBOL_AS = 52,                        /* AS  */
  YYSYMBOL_ADD = 53,                       /* ADD  */
  YYSYMBOL_MINUS = 54,                     /* MINUS  */
  YYSYMBOL_DIV = 55,                       /* DIV  */
  YYSYMBOL_INNER = 56,                     /* INNER  */
  YYSYMBOL_JOIN = 57,                      /* JOIN  */
  YYSYMBOL_GROUP = 58,                     /* GROUP  */
  YYSYMBOL_ORDER = 59,                     /* ORDER  */
  YYSYMBOL_BY = 60,                        /* BY  */
  YYSYMBOL_HAVING = 61,                    /* HAVING  */
  YYSYMBOL_ASC = 62,                       /* ASC  */
  YYSYMBOL_IN = 63,                        /* IN  */
  YYSYMBOL_OR = 64,                        /* OR  */
  YYSYMBOL_EXISTS = 65,                    /* EXISTS  */
  YYSYMBOL_NUMBER = 66,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 67,                     /* FLOAT  */
  YYSYMBOL_ID = 68,                        /* ID  */
  YYSYMBOL_PATH = 69,                      /* PATH  */
  YYSYMBOL_SSS = 70,                       /* SSS  */
  YYSYMBOL_STAR = 71,                      /* STAR  */
  YYSYMBOL_STRING_V = 72,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 73,                  /* $accept  */
  YYSYMBOL_commands = 74,                  /* commands  */
  YYSYMBOL_command = 75,                   /* command  */
  YYSYMBOL_exit = 76,                      /* exit  */
  YYSYMBOL_help = 77,                      /* help  */
  YYSYMBOL_sync = 78,                      /* sync  */
  YYSYMBOL_begin = 79,                     /* begin  */
  YYSYMBOL_commit = 80,                    /* commit  */
  YYSYMBOL_rollback = 81,                  /* rollback  */
  YYSYMBOL_drop_table = 82,                /* drop_table  */
  YYSYMBOL_show_tables = 83,               /* show_tables  */
  YYSYMBOL_desc_table = 84,                /* desc_table  */
  YYSYMBOL_show_index = 85,                /* show_index  */
  YYSYMBOL_create_index = 86,              /* create_index  */
  YYSYMBOL_create_index_unique = 87,       /* create_index_unique  */
  YYSYMBOL_create_index_attr_list = 88,    /* create_index_attr_list  */
  YYSYMBOL_drop_index = 89,                /* drop_index  */
  YYSYMBOL_create_table = 90,              /* create_table  */
  YYSYMBOL_attr_def_list = 91,             /* attr_def_list  */
  YYSYMBOL_attr_def = 92,                  /* attr_def  */
  YYSYMBOL_number = 93,                    /* number  */
  YYSYMBOL_type = 94,                      /* type  */
  YYSYMBOL_ID_get = 95,                    /* ID_get  */
  YYSYMBOL_attr_def_nullable = 96,         /* attr_def_nullable  */
  YYSYMBOL_insert = 97,                    /* insert  */
  YYSYMBOL_value_lists = 98,               /* value_lists  */
  YYSYMBOL_value_list = 99,                /* value_list  */
  YYSYMBOL_value = 100,                    /* value  */
  YYSYMBOL_pos_value = 101,                /* pos_value  */
  YYSYMBOL_delete = 102,                   /* delete  */
  YYSYMBOL_update = 103,                   /* update  */
  YYSYMBOL_update_set_list = 104,          /* update_set_list  */
  YYSYMBOL_select = 105,                   /* select  */
  YYSYMBOL_select_stmt = 106,              /* select_stmt  */
  YYSYMBOL_select_attr = 107,              /* select_attr  */
  YYSYMBOL_attr_list = 108,                /* attr_list  */
  YYSYMBOL_rel_list = 109,                 /* rel_list  */
  YYSYMBOL_where = 110,                    /* where  */
  YYSYMBOL_group_by = 111,                 /* group_by  */
  YYSYMBOL_having = 112,                   /* having  */
  YYSYMBOL_order_by = 113,                 /* order_by  */
  YYSYMBOL_order_by_list = 114,            /* order_by_list  */
  YYSYMBOL_order_direct = 115,             /* order_direct  */
  YYSYMBOL_condition_list = 116,           /* condition_list  */
  YYSYMBOL_condition = 117,                /* condition  */
  YYSYMBOL_comOp = 118,                    /* comOp  */
  YYSYMBOL_expr = 119,                     /* expr  */
  YYSYMBOL_expr_list = 120,                /* expr_list  */
  YYSYMBOL_load_data = 121                 /* load_data  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
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
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   242

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  73
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  126
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  237

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   327


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   174,   174,   176,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   201,   206,   211,   217,   223,   229,   235,   241,
     247,   254,   261,   270,   271,   275,   279,   286,   293,   299,
     301,   305,   311,   319,   322,   323,   324,   325,   326,   329,
     336,   337,   338,   343,   351,   355,   365,   369,   376,   379,
     382,   388,   391,   394,   397,   404,   414,   425,   433,   444,
     452,   479,   484,   489,   497,   502,   509,   511,   515,   522,
     523,   528,   529,   535,   536,   542,   543,   549,   556,   565,
     566,   567,   571,   576,   581,   588,   591,   601,   611,   622,
     636,   637,   638,   639,   640,   641,   642,   643,   644,   645,
     649,   654,   658,   663,   667,   672,   676,   680,   684,   688,
     692,   696,   701,   706,   712,   717,   725
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "NULL_VALUE", "NULLABLE", "INDEX", "SELECT",
  "DESC", "SHOW", "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE",
  "COMMA", "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T",
  "DATE_T", "FLOAT_T", "TEXT_T", "HELP", "EXIT", "DOT", "INTO", "VALUES",
  "FROM", "WHERE", "IS", "AND", "SET", "ON", "LOAD", "DATA", "INFILE",
  "EQ", "LT", "GT", "LE", "GE", "NE", "NOT", "LIKE", "UNIQUE", "AS", "ADD",
  "MINUS", "DIV", "INNER", "JOIN", "GROUP", "ORDER", "BY", "HAVING", "ASC",
  "IN", "OR", "EXISTS", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR",
  "STRING_V", "$accept", "commands", "command", "exit", "help", "sync",
  "begin", "commit", "rollback", "drop_table", "show_tables", "desc_table",
  "show_index", "create_index", "create_index_unique",
  "create_index_attr_list", "drop_index", "create_table", "attr_def_list",
  "attr_def", "number", "type", "ID_get", "attr_def_nullable", "insert",
  "value_lists", "value_list", "value", "pos_value", "delete", "update",
  "update_set_list", "select", "select_stmt", "select_attr", "attr_list",
  "rel_list", "where", "group_by", "having", "order_by", "order_by_list",
  "order_direct", "condition_list", "condition", "comOp", "expr",
  "expr_list", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-152)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -152,   146,  -152,    -3,    24,    35,   -52,    47,    19,     5,
      -7,   -35,    38,    42,    52,    73,    81,    51,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,    95,  -152,    32,  -152,
     109,    53,    54,  -152,    -4,    35,  -152,  -152,     1,  -152,
    -152,  -152,   100,    90,    55,   122,   125,    96,  -152,    61,
      65,    97,  -152,  -152,  -152,  -152,  -152,    94,  -152,   123,
      72,   139,   140,  -152,   126,    63,   128,   -42,    35,    28,
      35,    76,    80,    35,    35,    35,  -152,    35,  -152,  -152,
      85,   116,   119,    88,   101,   104,   135,  -152,  -152,  -152,
      35,  -152,   145,  -152,  -152,  -152,   -10,  -152,   -42,   -42,
    -152,  -152,   162,   155,    20,   182,   147,   119,   159,  -152,
     172,    87,   127,  -152,  -152,   130,   136,   119,  -152,    27,
     191,   131,    35,  -152,   -20,   134,  -152,    35,   196,   194,
     104,   183,  -152,  -152,  -152,  -152,  -152,     2,   186,   -10,
     133,   148,     4,   188,   189,  -152,  -152,    35,   -29,    20,
      20,    -2,  -152,  -152,  -152,  -152,  -152,  -152,   -14,  -152,
    -152,    35,    84,  -152,   142,   172,   205,  -152,   149,   203,
    -152,   144,  -152,   174,   154,   156,  -152,  -152,   198,    27,
     -29,  -152,  -152,  -152,   208,  -152,  -152,   -29,    88,   216,
    -152,  -152,  -152,   201,  -152,   202,   204,    20,    35,    20,
     166,   155,  -152,  -152,  -152,  -152,     3,   144,   218,   -10,
    -152,  -152,   167,  -152,  -152,  -152,  -152,  -152,  -152,    35,
    -152,     6,  -152,  -152,   206,    35,  -152
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    14,    12,
      13,     8,     5,     7,     6,     4,     0,    19,     0,    34,
       0,     0,     0,    61,     0,     0,    62,    63,   111,    64,
     110,   115,    74,     0,    71,     0,     0,     0,    24,     0,
       0,     0,    25,    26,    27,    23,    22,     0,    69,     0,
       0,     0,     0,   121,     0,   124,     0,   116,     0,     0,
       0,     0,     0,     0,     0,     0,    72,     0,    30,    29,
       0,     0,    79,     0,     0,     0,     0,    28,    37,   123,
       0,   122,     0,   113,   112,    75,    76,    73,   117,   118,
     120,   119,     0,     0,     0,     0,     0,    79,     0,    49,
      39,     0,     0,   125,   114,     0,     0,    79,    31,     0,
       0,     0,     0,    80,    92,     0,    65,     0,     0,     0,
       0,     0,    44,    45,    46,    47,    48,    50,     0,    76,
       0,    81,     0,     0,    56,    58,    53,     0,    98,     0,
       0,     0,   100,   101,   102,   103,   104,   105,     0,   106,
     108,     0,    67,    66,     0,    39,     0,    52,     0,     0,
      42,     0,    77,     0,     0,    83,    59,    60,    54,     0,
      99,    93,    94,    96,     0,   107,   109,    95,     0,     0,
      40,    38,    43,     0,    51,    35,     0,     0,     0,     0,
      85,     0,    57,    97,    68,   126,    50,     0,     0,    76,
      82,    84,     0,    70,    55,    41,    36,    32,    78,     0,
      86,    89,    91,    90,    87,     0,    88
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,    11,  -152,  -152,    56,    89,
    -152,  -152,  -152,    16,  -152,    22,    45,  -152,  -124,  -152,
    -152,    37,  -152,   192,  -152,   157,  -147,   -96,  -152,  -152,
    -152,     7,  -152,  -151,  -152,  -152,    -5,   -77,  -152
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    40,   206,    30,    31,   141,   120,
     203,   147,   121,   180,    32,   130,   153,   154,    51,    33,
      34,   117,    35,    36,    52,    53,   127,   115,   185,   210,
     223,   230,   234,   133,   134,   171,   135,    76,    37
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      54,   102,   182,    38,    43,   155,   193,     5,   191,   192,
     125,   177,   177,    85,    44,    73,    55,   159,   232,    78,
     178,   138,    58,   123,    83,    84,    85,    60,    43,    87,
      41,   151,    79,    61,    42,    43,   195,    59,    44,    75,
      77,    62,    87,    43,   160,    63,   126,   194,    39,   196,
      45,   179,   179,    44,    56,    64,   219,    57,   221,    83,
      84,    85,    46,    47,    48,   155,    49,    50,   233,   131,
     186,   187,   228,    75,    45,    54,    65,    87,   108,   109,
     110,   152,   111,   100,    66,   132,    46,    47,    48,    45,
      49,    50,    67,    46,    47,    75,   103,    49,    68,   104,
      69,    46,    47,    48,   198,    49,    50,    82,    83,    84,
      85,   142,   143,   144,   145,   146,    83,    84,    85,    70,
      80,    71,    72,    86,    81,    88,    87,   158,    89,    91,
      90,   220,   172,    92,    87,    93,    94,    83,    84,    85,
      96,    95,    97,    98,   106,    99,     2,   101,   107,   113,
       3,     4,   190,   112,   114,    87,   116,     5,     6,     7,
       8,     9,    10,    11,   124,   128,   197,    12,    13,    14,
     161,   118,   119,   129,   122,    15,    16,   162,   163,   164,
     165,   166,   167,   168,   169,   136,    17,    83,    84,    85,
     137,   139,   140,   150,   156,   148,   157,   170,   149,   173,
     174,   183,   176,    75,   181,    87,   184,   188,   201,   189,
     199,   204,   205,   207,   208,   202,   213,   209,   211,   215,
     216,   227,   217,   218,   231,   222,   235,   229,   226,   175,
     231,   200,   225,   224,   212,   214,    74,   105,     0,     0,
       0,     0,   236
};

static const yytype_int16 yycheck[] =
{
       5,    78,   149,     6,     8,   129,     8,    11,   159,   160,
      20,     9,     9,    55,    18,    19,    68,    37,    12,    18,
      18,   117,     3,   100,    53,    54,    55,    34,     8,    71,
       6,   127,    31,    68,    10,     8,    50,    32,    18,    44,
      45,     3,    71,     8,    64,     3,    56,    49,    51,    63,
      54,    49,    49,    18,     7,     3,   207,    10,   209,    53,
      54,    55,    66,    67,    68,   189,    70,    71,    62,    49,
      66,    67,   219,    78,    54,    80,     3,    71,    83,    84,
      85,    54,    87,    20,     3,    65,    66,    67,    68,    54,
      70,    71,    41,    66,    67,   100,    68,    70,     3,    71,
      68,    66,    67,    68,    20,    70,    71,    52,    53,    54,
      55,    24,    25,    26,    27,    28,    53,    54,    55,    10,
      20,    68,    68,    68,    34,     3,    71,   132,     3,    68,
      34,   208,   137,    68,    71,    38,    42,    53,    54,    55,
      68,    18,     3,     3,    68,    19,     0,    19,    68,    33,
       4,     5,   157,    68,    35,    71,    68,    11,    12,    13,
      14,    15,    16,    17,    19,     3,   171,    21,    22,    23,
      36,    70,    68,    18,    39,    29,    30,    43,    44,    45,
      46,    47,    48,    49,    50,     3,    40,    53,    54,    55,
      43,    32,    20,    57,     3,    68,    65,    63,    68,     3,
       6,    68,    19,   208,    18,    71,    58,    19,     3,    20,
      68,     8,    68,    39,    60,    66,     8,    61,    20,     3,
      19,     3,    20,    19,   229,    59,    20,    60,   217,   140,
     235,   175,   216,   211,   189,   198,    44,    80,    -1,    -1,
      -1,    -1,   235
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    74,     0,     4,     5,    11,    12,    13,    14,    15,
      16,    17,    21,    22,    23,    29,    30,    40,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      89,    90,    97,   102,   103,   105,   106,   121,     6,    51,
      87,     6,    10,     8,    18,    54,    66,    67,    68,    70,
      71,   101,   107,   108,   119,    68,     7,    10,     3,    32,
      34,    68,     3,     3,     3,     3,     3,    41,     3,    68,
      10,    68,    68,    19,   106,   119,   120,   119,    18,    31,
      20,    34,    52,    53,    54,    55,    68,    71,     3,     3,
      34,    68,    68,    38,    42,    18,    68,     3,     3,    19,
      20,    19,   120,    68,    71,   108,    68,    68,   119,   119,
     119,   119,    68,    33,    35,   110,    68,   104,    70,    68,
      92,    95,    39,   120,    19,    20,    56,   109,     3,    18,
      98,    49,    65,   116,   117,   119,     3,    43,   110,    32,
      20,    91,    24,    25,    26,    27,    28,    94,    68,    68,
      57,   110,    54,    99,   100,   101,     3,    65,   119,    37,
      64,    36,    43,    44,    45,    46,    47,    48,    49,    50,
      63,   118,   119,     3,     6,    92,    19,     9,    18,    49,
      96,    18,   109,    68,    58,   111,    66,    67,    19,    20,
     119,   116,   116,     8,    49,    50,    63,   119,    20,    68,
      91,     3,    66,    93,     8,    68,    88,    39,    60,    61,
     112,    20,    99,     8,   104,     3,    19,    20,    19,   116,
     120,   116,    59,   113,    98,    96,    88,     3,   109,    60,
     114,   119,    12,    62,   115,    20,   114
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    73,    74,    74,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    87,    88,    88,    89,    90,    91,
      91,    92,    92,    93,    94,    94,    94,    94,    94,    95,
      96,    96,    96,    97,    98,    98,    99,    99,   100,   100,
     100,   101,   101,   101,   101,   102,   103,   104,   104,   105,
     106,   107,   107,   107,   108,   108,   109,   109,   109,   110,
     110,   111,   111,   112,   112,   113,   113,   114,   114,   115,
     115,   115,   116,   116,   116,   117,   117,   117,   117,   117,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   120,   120,   121
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       3,     5,    10,     0,     1,     1,     3,     4,     8,     0,
       3,     6,     3,     1,     1,     1,     1,     1,     1,     1,
       0,     2,     1,     6,     3,     5,     1,     3,     1,     2,
       2,     1,     1,     1,     1,     5,     6,     3,     5,     2,
       9,     1,     2,     3,     1,     3,     0,     3,     6,     0,
       2,     0,     3,     0,     2,     0,     3,     2,     4,     0,
       1,     1,     1,     3,     3,     3,     3,     4,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       1,     1,     3,     3,     4,     1,     2,     3,     3,     3,
       3,     2,     3,     3,     1,     3,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void *scanner)
{
  YY_USE (yyvaluep);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 22: /* exit: EXIT SEMICOLON  */
#line 201 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1407 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 206 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1415 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 211 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1423 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 217 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1431 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 223 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1439 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 229 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1447 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 235 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1456 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 241 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1464 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 247 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1473 "yacc_sql.tab.c"
    break;

  case 31: /* show_index: SHOW INDEX FROM ID SEMICOLON  */
#line 254 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1482 "yacc_sql.tab.c"
    break;

  case 32: /* create_index: CREATE create_index_unique INDEX ID ON ID LBRACE create_index_attr_list RBRACE SEMICOLON  */
#line 262 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (const char **)(yyvsp[-2].list)->values, (yyvsp[-2].list)->len, (yyvsp[-8].boolean));
			list_free((yyvsp[-2].list));
		}
#line 1492 "yacc_sql.tab.c"
    break;

  case 33: /* create_index_unique: %empty  */
#line 270 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1498 "yacc_sql.tab.c"
    break;

  case 34: /* create_index_unique: UNIQUE  */
#line 271 "yacc_sql.y"
                 { (yyval.boolean) = true; }
#line 1504 "yacc_sql.tab.c"
    break;

  case 35: /* create_index_attr_list: ID  */
#line 275 "yacc_sql.y"
           {
		(yyval.list) = list_create(sizeof(char *), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].string));
	}
#line 1513 "yacc_sql.tab.c"
    break;

  case 36: /* create_index_attr_list: ID COMMA create_index_attr_list  */
#line 279 "yacc_sql.y"
                                          {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].string));
	}
#line 1522 "yacc_sql.tab.c"
    break;

  case 37: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 287 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1531 "yacc_sql.tab.c"
    break;

  case 38: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 294 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
		}
#line 1540 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 301 "yacc_sql.y"
                                   {    }
#line 1546 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type LBRACE number RBRACE attr_def_nullable  */
#line 306 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1556 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type attr_def_nullable  */
#line 312 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1566 "yacc_sql.tab.c"
    break;

  case 43: /* number: NUMBER  */
#line 319 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1572 "yacc_sql.tab.c"
    break;

  case 44: /* type: INT_T  */
#line 322 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1578 "yacc_sql.tab.c"
    break;

  case 45: /* type: STRING_T  */
#line 323 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1584 "yacc_sql.tab.c"
    break;

  case 46: /* type: DATE_T  */
#line 324 "yacc_sql.y"
                { (yyval.number)=DATE; }
#line 1590 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 325 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1596 "yacc_sql.tab.c"
    break;

  case 48: /* type: TEXT_T  */
#line 326 "yacc_sql.y"
                    { (yyval.number)=TEXT; }
#line 1602 "yacc_sql.tab.c"
    break;

  case 49: /* ID_get: ID  */
#line 330 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1611 "yacc_sql.tab.c"
    break;

  case 50: /* attr_def_nullable: %empty  */
#line 336 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1617 "yacc_sql.tab.c"
    break;

  case 51: /* attr_def_nullable: NOT NULL_VALUE  */
#line 337 "yacc_sql.y"
                         { (yyval.boolean) = false; }
#line 1623 "yacc_sql.tab.c"
    break;

  case 52: /* attr_def_nullable: NULLABLE  */
#line 338 "yacc_sql.y"
                   { (yyval.boolean) = true; }
#line 1629 "yacc_sql.tab.c"
    break;

  case 53: /* insert: INSERT INTO ID VALUES value_lists SEMICOLON  */
#line 344 "yacc_sql.y"
        {
	  	CONTEXT->ssql->flag=SCF_INSERT;//"insert";
		inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-3].string), (Value *)(yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		list_free((yyvsp[-1].list));
    }
#line 1639 "yacc_sql.tab.c"
    break;

  case 54: /* value_lists: LBRACE value_list RBRACE  */
#line 351 "yacc_sql.y"
                                 {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[-1].list);
	}
#line 1648 "yacc_sql.tab.c"
    break;

  case 55: /* value_lists: LBRACE value_list RBRACE COMMA value_lists  */
#line 355 "yacc_sql.y"
                                                     {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[0].list);
		list_prepend_list((yyval.list), (yyvsp[-3].list));
		list_free((yyvsp[-3].list));
	}
#line 1659 "yacc_sql.tab.c"
    break;

  case 56: /* value_list: value  */
#line 365 "yacc_sql.y"
              {
		(yyval.list) = list_create(sizeof(Value), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].value));
	}
#line 1668 "yacc_sql.tab.c"
    break;

  case 57: /* value_list: value COMMA value_list  */
#line 369 "yacc_sql.y"
                              { 
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].value));
	}
#line 1677 "yacc_sql.tab.c"
    break;

  case 58: /* value: pos_value  */
#line 376 "yacc_sql.y"
                  {
		(yyval.value) = (yyvsp[0].value);
	}
#line 1685 "yacc_sql.tab.c"
    break;

  case 59: /* value: MINUS NUMBER  */
#line 379 "yacc_sql.y"
                  {	
  		value_init_integer(&(yyval.value), -(yyvsp[0].number));
	}
#line 1693 "yacc_sql.tab.c"
    break;

  case 60: /* value: MINUS FLOAT  */
#line 382 "yacc_sql.y"
                 {
  		value_init_float(&(yyval.value), -(yyvsp[0].floats));
	}
#line 1701 "yacc_sql.tab.c"
    break;

  case 61: /* pos_value: NULL_VALUE  */
#line 388 "yacc_sql.y"
                   {
		value_init_null(&(yyval.value));
	}
#line 1709 "yacc_sql.tab.c"
    break;

  case 62: /* pos_value: NUMBER  */
#line 391 "yacc_sql.y"
             {	
  		value_init_integer(&(yyval.value), (yyvsp[0].number));
	}
#line 1717 "yacc_sql.tab.c"
    break;

  case 63: /* pos_value: FLOAT  */
#line 394 "yacc_sql.y"
            {
  		value_init_float(&(yyval.value), (yyvsp[0].floats));
		}
#line 1725 "yacc_sql.tab.c"
    break;

  case 64: /* pos_value: SSS  */
#line 397 "yacc_sql.y"
          {
		(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&(yyval.value), (yyvsp[0].string));
	}
#line 1734 "yacc_sql.tab.c"
    break;

  case 65: /* delete: DELETE FROM ID where SEMICOLON  */
#line 405 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1746 "yacc_sql.tab.c"
    break;

  case 66: /* update: UPDATE ID SET update_set_list where SEMICOLON  */
#line 415 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-4].string), (KeyValue *) (yyvsp[-2].list)->values, (yyvsp[-2].list)->len, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-2].list));
			list_free((yyvsp[-1].list));
		}
#line 1758 "yacc_sql.tab.c"
    break;

  case 67: /* update_set_list: ID EQ expr  */
#line 426 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(KeyValue), MAX_NUM);
		KeyValue kv;
		kv.name = (yyvsp[-2].string);
		kv.value = (yyvsp[0].expr);
		list_prepend((yyval.list), &kv);
	}
#line 1770 "yacc_sql.tab.c"
    break;

  case 68: /* update_set_list: ID EQ expr COMMA update_set_list  */
#line 434 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		KeyValue kv;
		kv.name = (yyvsp[-4].string);
		kv.value = (yyvsp[-2].expr);
		list_prepend((yyval.list), &kv);
	}
#line 1782 "yacc_sql.tab.c"
    break;

  case 69: /* select: select_stmt SEMICOLON  */
#line 445 "yacc_sql.y"
        {
		CONTEXT->ssql->sstr.selection = (yyvsp[-1].select);
		CONTEXT->ssql->flag=SCF_SELECT;//"select";
	}
#line 1791 "yacc_sql.tab.c"
    break;

  case 70: /* select_stmt: SELECT attr_list FROM ID rel_list where group_by having order_by  */
#line 453 "yacc_sql.y"
                {
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-5].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) (yyvsp[-3].list)->values, (yyvsp[-3].list)->len);
			list_free((yyvsp[-3].list));

			selects_append_attribute(&CONTEXT->ssql->sstr.selection, (AttrExpr *) (yyvsp[-7].list)->values, (yyvsp[-7].list)->len);
			list_free((yyvsp[-7].list));

			selects_append_havings(&CONTEXT->ssql->sstr.selection, (Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));

			selects_append_groups(&CONTEXT->ssql->sstr.selection, (UnionExpr *) (yyvsp[-2].list)->values, (yyvsp[-2].list)->len);
			list_free((yyvsp[-2].list));

			selects_append_orders(&CONTEXT->ssql->sstr.selection, (OrderExpr *) (yyvsp[0].list)->values, (yyvsp[0].list)->len);
			list_free((yyvsp[0].list));

			(yyval.select) = CONTEXT->ssql->sstr.selection;
  			CONTEXT->ssql->sstr.selection.relation_join_num = 0;
  			CONTEXT->ssql->sstr.selection.relation_num = 0;     
  			CONTEXT->ssql->sstr.selection.join_condition_num = 0;
	}
#line 1819 "yacc_sql.tab.c"
    break;

  case 71: /* select_attr: expr  */
#line 480 "yacc_sql.y"
        {
		(yyval.attr).expr = (yyvsp[0].expr);
		(yyval.attr).name = NULL;
	}
#line 1828 "yacc_sql.tab.c"
    break;

  case 72: /* select_attr: expr ID  */
#line 485 "yacc_sql.y"
        {
		(yyval.attr).expr = (yyvsp[-1].expr);
		(yyval.attr).name = (yyvsp[0].string);
	}
#line 1837 "yacc_sql.tab.c"
    break;

  case 73: /* select_attr: expr AS ID  */
#line 490 "yacc_sql.y"
        {
		(yyval.attr).expr = (yyvsp[-2].expr);
		(yyval.attr).name = (yyvsp[0].string);
	}
#line 1846 "yacc_sql.tab.c"
    break;

  case 74: /* attr_list: select_attr  */
#line 498 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(AttrExpr), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].attr));
	}
#line 1855 "yacc_sql.tab.c"
    break;

  case 75: /* attr_list: select_attr COMMA attr_list  */
#line 503 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].attr));
	}
#line 1864 "yacc_sql.tab.c"
    break;

  case 77: /* rel_list: COMMA ID rel_list  */
#line 511 "yacc_sql.y"
                        {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string),NULL, 0);
	}
#line 1873 "yacc_sql.tab.c"
    break;

  case 78: /* rel_list: INNER JOIN ID ON condition_list rel_list  */
#line 515 "yacc_sql.y"
                                                  {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
		selects_append_join_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string),(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		list_free((yyvsp[-1].list));
	}
#line 1884 "yacc_sql.tab.c"
    break;

  case 79: /* where: %empty  */
#line 522 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(Condition), MAX_NUM); }
#line 1890 "yacc_sql.tab.c"
    break;

  case 80: /* where: WHERE condition_list  */
#line 523 "yacc_sql.y"
                           {	
		(yyval.list) = (yyvsp[0].list);
	}
#line 1898 "yacc_sql.tab.c"
    break;

  case 81: /* group_by: %empty  */
#line 528 "yacc_sql.y"
        { (yyval.list) = list_create(sizeof(UnionExpr), 0); }
#line 1904 "yacc_sql.tab.c"
    break;

  case 82: /* group_by: GROUP BY expr_list  */
#line 530 "yacc_sql.y"
        {
	  (yyval.list) = (yyvsp[0].list);
	}
#line 1912 "yacc_sql.tab.c"
    break;

  case 83: /* having: %empty  */
#line 535 "yacc_sql.y"
        { (yyval.list) = list_create(sizeof(Condition), 0); }
#line 1918 "yacc_sql.tab.c"
    break;

  case 84: /* having: HAVING condition_list  */
#line 537 "yacc_sql.y"
        {
	  (yyval.list) = (yyvsp[0].list);
	}
#line 1926 "yacc_sql.tab.c"
    break;

  case 85: /* order_by: %empty  */
#line 542 "yacc_sql.y"
        { (yyval.list) = list_create(sizeof(OrderExpr), 0); }
#line 1932 "yacc_sql.tab.c"
    break;

  case 86: /* order_by: ORDER BY order_by_list  */
#line 544 "yacc_sql.y"
        {
	  (yyval.list) = (yyvsp[0].list);
	}
#line 1940 "yacc_sql.tab.c"
    break;

  case 87: /* order_by_list: expr order_direct  */
#line 549 "yacc_sql.y"
                          {
		(yyval.list) = list_create(sizeof(OrderExpr), MAX_NUM);
		OrderExpr order;
		order.expr = (yyvsp[-1].expr);
		order.asc = (yyvsp[0].boolean);
		list_prepend((yyval.list), &order);
	}
#line 1952 "yacc_sql.tab.c"
    break;

  case 88: /* order_by_list: expr order_direct COMMA order_by_list  */
#line 556 "yacc_sql.y"
                                                {
		(yyval.list) = (yyvsp[0].list);
		OrderExpr order;
		order.expr = (yyvsp[-3].expr);
		order.asc = (yyvsp[-2].boolean);
		list_prepend((yyval.list), &order);
	}
#line 1964 "yacc_sql.tab.c"
    break;

  case 89: /* order_direct: %empty  */
#line 565 "yacc_sql.y"
        { (yyval.boolean) = true; }
#line 1970 "yacc_sql.tab.c"
    break;

  case 90: /* order_direct: ASC  */
#line 566 "yacc_sql.y"
              { (yyval.boolean) = true; }
#line 1976 "yacc_sql.tab.c"
    break;

  case 91: /* order_direct: DESC  */
#line 567 "yacc_sql.y"
               { (yyval.boolean) = false; }
#line 1982 "yacc_sql.tab.c"
    break;

  case 92: /* condition_list: condition  */
#line 571 "yacc_sql.y"
                  {
		(yyval.list) = list_create(sizeof(Condition), MAX_NUM);
		(yyvsp[0].condition).is_and = true;
		list_prepend((yyval.list), &(yyvsp[0].condition));
	}
#line 1992 "yacc_sql.tab.c"
    break;

  case 93: /* condition_list: condition AND condition_list  */
#line 576 "yacc_sql.y"
                                   {
		(yyval.list) = (yyvsp[0].list);
		(yyvsp[-2].condition).is_and = true;
		list_prepend((yyval.list), &(yyvsp[-2].condition));
	}
#line 2002 "yacc_sql.tab.c"
    break;

  case 94: /* condition_list: condition OR condition_list  */
#line 581 "yacc_sql.y"
                                  {
		(yyval.list) = (yyvsp[0].list);
		(yyvsp[-2].condition).is_and = false;
		list_prepend((yyval.list), &(yyvsp[-2].condition));
	}
#line 2012 "yacc_sql.tab.c"
    break;

  case 95: /* condition: expr comOp expr  */
#line 588 "yacc_sql.y"
                        {
		condition_init(&(yyval.condition), (yyvsp[-1].comp_op), &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2020 "yacc_sql.tab.c"
    break;

  case 96: /* condition: expr IS NULL_VALUE  */
#line 591 "yacc_sql.y"
                             {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), IS_NULL, &(yyvsp[-2].expr), &null_expr);
	}
#line 2035 "yacc_sql.tab.c"
    break;

  case 97: /* condition: expr IS NOT NULL_VALUE  */
#line 601 "yacc_sql.y"
                                 {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), IS_NOT_NULL, &(yyvsp[-3].expr), &null_expr);
	}
#line 2050 "yacc_sql.tab.c"
    break;

  case 98: /* condition: EXISTS expr  */
#line 612 "yacc_sql.y"
        {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), OP_EXISTS, &null_expr, &(yyvsp[0].expr));
	}
#line 2065 "yacc_sql.tab.c"
    break;

  case 99: /* condition: NOT EXISTS expr  */
#line 623 "yacc_sql.y"
        {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), OP_NOT_EXISTS, &null_expr, &(yyvsp[0].expr));
	}
#line 2080 "yacc_sql.tab.c"
    break;

  case 100: /* comOp: EQ  */
#line 636 "yacc_sql.y"
             { (yyval.comp_op) = EQUAL_TO; }
#line 2086 "yacc_sql.tab.c"
    break;

  case 101: /* comOp: LT  */
#line 637 "yacc_sql.y"
         { (yyval.comp_op) = LESS_THAN; }
#line 2092 "yacc_sql.tab.c"
    break;

  case 102: /* comOp: GT  */
#line 638 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_THAN; }
#line 2098 "yacc_sql.tab.c"
    break;

  case 103: /* comOp: LE  */
#line 639 "yacc_sql.y"
         { (yyval.comp_op) = LESS_EQUAL; }
#line 2104 "yacc_sql.tab.c"
    break;

  case 104: /* comOp: GE  */
#line 640 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_EQUAL; }
#line 2110 "yacc_sql.tab.c"
    break;

  case 105: /* comOp: NE  */
#line 641 "yacc_sql.y"
         { (yyval.comp_op) = NOT_EQUAL; }
#line 2116 "yacc_sql.tab.c"
    break;

  case 106: /* comOp: LIKE  */
#line 642 "yacc_sql.y"
               { (yyval.comp_op) = OP_LIKE; }
#line 2122 "yacc_sql.tab.c"
    break;

  case 107: /* comOp: NOT LIKE  */
#line 643 "yacc_sql.y"
                   { (yyval.comp_op) = OP_NOT_LIKE; }
#line 2128 "yacc_sql.tab.c"
    break;

  case 108: /* comOp: IN  */
#line 644 "yacc_sql.y"
             { (yyval.comp_op) = OP_IN; }
#line 2134 "yacc_sql.tab.c"
    break;

  case 109: /* comOp: NOT IN  */
#line 645 "yacc_sql.y"
                 { (yyval.comp_op) = OP_NOT_IN; }
#line 2140 "yacc_sql.tab.c"
    break;

  case 110: /* expr: STAR  */
#line 650 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, NULL, "*");
	}
#line 2149 "yacc_sql.tab.c"
    break;

  case 111: /* expr: ID  */
#line 654 "yacc_sql.y"
             {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, NULL, (yyvsp[0].string));
	}
#line 2158 "yacc_sql.tab.c"
    break;

  case 112: /* expr: ID DOT STAR  */
#line 659 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, (yyvsp[-2].string), "*");
	}
#line 2167 "yacc_sql.tab.c"
    break;

  case 113: /* expr: ID DOT ID  */
#line 663 "yacc_sql.y"
                    {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, (yyvsp[-2].string), (yyvsp[0].string));
	}
#line 2176 "yacc_sql.tab.c"
    break;

  case 114: /* expr: ID LBRACE expr_list RBRACE  */
#line 668 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_FUNC;
		func_init(&(yyval.expr).value.func, (yyvsp[-3].string), (UnionExpr *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
	}
#line 2185 "yacc_sql.tab.c"
    break;

  case 115: /* expr: pos_value  */
#line 672 "yacc_sql.y"
                    {
		(yyval.expr).type = EXPR_VALUE;
		(yyval.expr).value.value = (yyvsp[0].value);
	}
#line 2194 "yacc_sql.tab.c"
    break;

  case 116: /* expr: MINUS expr  */
#line 676 "yacc_sql.y"
                     {
		(yyval.expr).type = EXPR_FUNC;
		func_init_1(&(yyval.expr).value.func, "neg", &(yyvsp[0].expr));
	}
#line 2203 "yacc_sql.tab.c"
    break;

  case 117: /* expr: expr ADD expr  */
#line 680 "yacc_sql.y"
                        {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "+", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2212 "yacc_sql.tab.c"
    break;

  case 118: /* expr: expr MINUS expr  */
#line 684 "yacc_sql.y"
                          {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "-", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2221 "yacc_sql.tab.c"
    break;

  case 119: /* expr: expr STAR expr  */
#line 688 "yacc_sql.y"
                         {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "*", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2230 "yacc_sql.tab.c"
    break;

  case 120: /* expr: expr DIV expr  */
#line 692 "yacc_sql.y"
                        {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "/", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2239 "yacc_sql.tab.c"
    break;

  case 121: /* expr: LBRACE RBRACE  */
#line 697 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_FUNC;
		func_init(&(yyval.expr).value.func, "tuple", NULL, 0);
	}
#line 2248 "yacc_sql.tab.c"
    break;

  case 122: /* expr: LBRACE expr_list RBRACE  */
#line 702 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_FUNC;
		func_init(&(yyval.expr).value.func, "tuple", (UnionExpr *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
	}
#line 2257 "yacc_sql.tab.c"
    break;

  case 123: /* expr: LBRACE select_stmt RBRACE  */
#line 706 "yacc_sql.y"
                                    {
		expr_init_selects(&(yyval.expr), &(yyvsp[-1].select));
	}
#line 2265 "yacc_sql.tab.c"
    break;

  case 124: /* expr_list: expr  */
#line 713 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(UnionExpr), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].expr));
	}
#line 2274 "yacc_sql.tab.c"
    break;

  case 125: /* expr_list: expr COMMA expr_list  */
#line 718 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].expr));
	}
#line 2283 "yacc_sql.tab.c"
    break;

  case 126: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 726 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2292 "yacc_sql.tab.c"
    break;


#line 2296 "yacc_sql.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, YY_("syntax error"));
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
                      yytoken, &yylval, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 731 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
