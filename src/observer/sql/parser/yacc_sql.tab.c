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
  YYSYMBOL_NUMBER = 64,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 65,                     /* FLOAT  */
  YYSYMBOL_ID = 66,                        /* ID  */
  YYSYMBOL_PATH = 67,                      /* PATH  */
  YYSYMBOL_SSS = 68,                       /* SSS  */
  YYSYMBOL_STAR = 69,                      /* STAR  */
  YYSYMBOL_STRING_V = 70,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 71,                  /* $accept  */
  YYSYMBOL_commands = 72,                  /* commands  */
  YYSYMBOL_command = 73,                   /* command  */
  YYSYMBOL_exit = 74,                      /* exit  */
  YYSYMBOL_help = 75,                      /* help  */
  YYSYMBOL_sync = 76,                      /* sync  */
  YYSYMBOL_begin = 77,                     /* begin  */
  YYSYMBOL_commit = 78,                    /* commit  */
  YYSYMBOL_rollback = 79,                  /* rollback  */
  YYSYMBOL_drop_table = 80,                /* drop_table  */
  YYSYMBOL_show_tables = 81,               /* show_tables  */
  YYSYMBOL_desc_table = 82,                /* desc_table  */
  YYSYMBOL_show_index = 83,                /* show_index  */
  YYSYMBOL_create_index = 84,              /* create_index  */
  YYSYMBOL_create_index_unique = 85,       /* create_index_unique  */
  YYSYMBOL_create_index_attr_list = 86,    /* create_index_attr_list  */
  YYSYMBOL_drop_index = 87,                /* drop_index  */
  YYSYMBOL_create_table = 88,              /* create_table  */
  YYSYMBOL_attr_def_list = 89,             /* attr_def_list  */
  YYSYMBOL_attr_def = 90,                  /* attr_def  */
  YYSYMBOL_number = 91,                    /* number  */
  YYSYMBOL_type = 92,                      /* type  */
  YYSYMBOL_ID_get = 93,                    /* ID_get  */
  YYSYMBOL_attr_def_nullable = 94,         /* attr_def_nullable  */
  YYSYMBOL_insert = 95,                    /* insert  */
  YYSYMBOL_value_lists = 96,               /* value_lists  */
  YYSYMBOL_value_list = 97,                /* value_list  */
  YYSYMBOL_value = 98,                     /* value  */
  YYSYMBOL_pos_value = 99,                 /* pos_value  */
  YYSYMBOL_delete = 100,                   /* delete  */
  YYSYMBOL_update = 101,                   /* update  */
  YYSYMBOL_update_set_list = 102,          /* update_set_list  */
  YYSYMBOL_select = 103,                   /* select  */
  YYSYMBOL_select_stmt = 104,              /* select_stmt  */
  YYSYMBOL_select_attr = 105,              /* select_attr  */
  YYSYMBOL_attr_list = 106,                /* attr_list  */
  YYSYMBOL_rel_list = 107,                 /* rel_list  */
  YYSYMBOL_where = 108,                    /* where  */
  YYSYMBOL_group_by = 109,                 /* group_by  */
  YYSYMBOL_having = 110,                   /* having  */
  YYSYMBOL_order_by = 111,                 /* order_by  */
  YYSYMBOL_order_by_list = 112,            /* order_by_list  */
  YYSYMBOL_order_direct = 113,             /* order_direct  */
  YYSYMBOL_condition_list = 114,           /* condition_list  */
  YYSYMBOL_condition = 115,                /* condition  */
  YYSYMBOL_comOp = 116,                    /* comOp  */
  YYSYMBOL_expr = 117,                     /* expr  */
  YYSYMBOL_expr_list = 118,                /* expr_list  */
  YYSYMBOL_load_data = 119                 /* load_data  */
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
#define YYLAST   226

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  122
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  229

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   325


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
      65,    66,    67,    68,    69,    70
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   172,   172,   174,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   199,   204,   209,   215,   221,   227,   233,   239,
     245,   252,   259,   268,   269,   273,   277,   284,   291,   297,
     299,   303,   309,   317,   320,   321,   322,   323,   324,   327,
     334,   335,   336,   341,   349,   353,   363,   367,   374,   377,
     380,   386,   389,   392,   395,   402,   412,   423,   431,   442,
     450,   477,   482,   487,   495,   500,   507,   509,   513,   520,
     521,   526,   527,   533,   534,   540,   541,   547,   554,   563,
     564,   565,   569,   573,   579,   582,   592,   605,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   618,   623,   627,
     632,   636,   641,   645,   649,   653,   657,   661,   665,   668,
     674,   679,   687
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
  "IN", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR", "STRING_V",
  "$accept", "commands", "command", "exit", "help", "sync", "begin",
  "commit", "rollback", "drop_table", "show_tables", "desc_table",
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

#define YYPACT_NINF (-146)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -146,   108,  -146,     3,    35,    18,   -50,    78,    72,    -8,
      65,    34,   102,   103,   106,   111,   112,    77,  -146,  -146,
    -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,
    -146,  -146,  -146,  -146,  -146,  -146,   123,  -146,    45,  -146,
     117,    67,    68,  -146,    -4,    18,  -146,  -146,     1,  -146,
    -146,  -146,   115,   113,   -32,   133,   149,   119,  -146,    88,
      89,   118,  -146,  -146,  -146,  -146,  -146,   116,  -146,   139,
      95,   159,   160,   145,   -11,   -38,    18,    38,    18,   100,
     101,    18,    18,    18,  -146,    18,  -146,  -146,   104,   135,
     134,   105,   107,   110,   138,  -146,  -146,  -146,  -146,    -2,
     153,  -146,  -146,  -146,    54,  -146,   -38,   -38,  -146,  -146,
     170,   156,    18,   175,   136,   134,   148,  -146,   161,    70,
     120,    18,  -146,   121,   125,   134,  -146,    25,   180,  -146,
     147,    96,  -146,    18,   182,   183,   110,   169,  -146,  -146,
    -146,  -146,  -146,    20,   172,  -146,    54,   126,   140,    27,
     174,   171,  -146,  -146,    18,    -3,  -146,  -146,  -146,  -146,
    -146,  -146,   -35,  -146,  -146,    18,    48,  -146,   128,   161,
     193,  -146,   137,   189,  -146,   141,  -146,   163,   143,   144,
    -146,  -146,   179,    25,  -146,  -146,   192,  -146,  -146,   -42,
     105,   201,  -146,  -146,  -146,   187,  -146,   188,   190,    18,
      18,    18,   151,   156,  -146,  -146,  -146,  -146,    21,   141,
     208,    54,  -146,  -146,   152,  -146,  -146,  -146,  -146,  -146,
    -146,    18,  -146,    -6,  -146,  -146,   194,    18,  -146
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
       0,     0,     0,    61,     0,     0,    62,    63,   108,    64,
     107,   112,    74,     0,    71,     0,     0,     0,    24,     0,
       0,     0,    25,    26,    27,    23,    22,     0,    69,     0,
       0,     0,     0,     0,     0,   113,     0,     0,     0,     0,
       0,     0,     0,     0,    72,     0,    30,    29,     0,     0,
      79,     0,     0,     0,     0,    28,    37,   119,   118,   120,
       0,   110,   109,    75,    76,    73,   114,   115,   117,   116,
       0,     0,     0,     0,     0,    79,     0,    49,    39,     0,
       0,     0,   111,     0,     0,    79,    31,     0,     0,    80,
      92,     0,    65,     0,     0,     0,     0,     0,    44,    45,
      46,    47,    48,    50,     0,   121,    76,     0,    81,     0,
       0,    56,    58,    53,     0,     0,    97,    98,    99,   100,
     101,   102,     0,   103,   105,     0,    67,    66,     0,    39,
       0,    52,     0,     0,    42,     0,    77,     0,     0,    83,
      59,    60,    54,     0,    93,    95,     0,   104,   106,    94,
       0,     0,    40,    38,    43,     0,    51,    35,     0,     0,
       0,     0,    85,     0,    57,    96,    68,   122,    50,     0,
       0,    76,    82,    84,     0,    70,    55,    41,    36,    32,
      78,     0,    86,    89,    91,    90,    87,     0,    88
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,  -146,
    -146,  -146,  -146,  -146,  -146,     4,  -146,  -146,    46,    81,
    -146,  -146,  -146,    10,  -146,    16,    37,  -146,  -124,  -146,
    -146,    31,  -146,   181,  -146,   146,  -145,   -90,  -146,  -146,
    -146,    -1,  -146,  -144,  -146,  -146,    -5,  -119,  -146
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    40,   198,    30,    31,   137,   118,
     195,   143,   119,   174,    32,   128,   150,   151,    51,    33,
      34,   115,    35,    36,    52,    53,   125,   113,   179,   202,
     215,   222,   226,   129,   130,   165,   131,   100,    37
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      54,   176,   145,   152,    43,   185,   224,     5,    98,    38,
     184,    81,    82,    83,    44,   187,    55,    83,   121,    76,
      80,    81,    82,    83,    59,   134,    43,    85,   188,   171,
     171,    85,    77,    43,    84,   148,    44,    85,   172,    74,
      75,    41,    81,    82,    83,    42,   186,    81,    82,    83,
      45,    81,    82,    83,    39,   211,   225,   213,    85,   152,
      46,    47,    48,    85,    49,    50,   220,    85,   190,   173,
     173,    99,    45,    54,   123,    58,   106,   107,   108,   149,
     109,   212,    46,    47,    48,    56,    49,    50,    57,    46,
      47,   180,   181,    49,   138,   139,   140,   141,   142,    60,
      61,    81,    82,    83,   101,    62,    63,   102,     2,    64,
     124,    69,     3,     4,    65,    66,    99,    85,    67,     5,
       6,     7,     8,     9,    10,    11,    68,    70,   166,    12,
      13,    14,   155,    71,    72,    78,    86,    15,    16,   156,
     157,   158,   159,   160,   161,   162,   163,    79,    17,    81,
      82,    83,    87,    88,    89,    90,    91,    93,    92,   164,
     189,    94,    95,    96,    97,    85,   104,   105,   111,   112,
     110,   114,   122,   126,   127,   116,   117,   120,   132,   133,
     135,   136,   147,   153,   154,   167,   144,   146,   170,   168,
     175,   183,   177,   182,   191,    99,   193,   196,   178,   203,
     205,   194,   199,   200,   207,   201,   208,   197,   209,   210,
     214,   219,   221,   218,   227,   192,   223,   169,   217,   216,
     204,   206,   223,     0,   103,    73,   228
};

static const yytype_int16 yycheck[] =
{
       5,   146,   121,   127,     8,     8,    12,    11,    19,     6,
     154,    53,    54,    55,    18,    50,    66,    55,    20,    18,
      52,    53,    54,    55,    32,   115,     8,    69,    63,     9,
       9,    69,    31,     8,    66,   125,    18,    69,    18,    44,
      45,     6,    53,    54,    55,    10,    49,    53,    54,    55,
      54,    53,    54,    55,    51,   199,    62,   201,    69,   183,
      64,    65,    66,    69,    68,    69,   211,    69,    20,    49,
      49,    76,    54,    78,    20,     3,    81,    82,    83,    54,
      85,   200,    64,    65,    66,     7,    68,    69,    10,    64,
      65,    64,    65,    68,    24,    25,    26,    27,    28,    34,
      66,    53,    54,    55,    66,     3,     3,    69,     0,     3,
      56,    66,     4,     5,     3,     3,   121,    69,    41,    11,
      12,    13,    14,    15,    16,    17,     3,    10,   133,    21,
      22,    23,    36,    66,    66,    20,     3,    29,    30,    43,
      44,    45,    46,    47,    48,    49,    50,    34,    40,    53,
      54,    55,     3,    34,    66,    66,    38,    18,    42,    63,
     165,    66,     3,     3,    19,    69,    66,    66,    33,    35,
      66,    66,    19,     3,    18,    68,    66,    39,     3,    43,
      32,    20,    57,     3,    37,     3,    66,    66,    19,     6,
      18,    20,    66,    19,    66,   200,     3,     8,    58,    20,
       8,    64,    39,    60,     3,    61,    19,    66,    20,    19,
      59,     3,    60,   209,    20,   169,   221,   136,   208,   203,
     183,   190,   227,    -1,    78,    44,   227
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    72,     0,     4,     5,    11,    12,    13,    14,    15,
      16,    17,    21,    22,    23,    29,    30,    40,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      87,    88,    95,   100,   101,   103,   104,   119,     6,    51,
      85,     6,    10,     8,    18,    54,    64,    65,    66,    68,
      69,    99,   105,   106,   117,    66,     7,    10,     3,    32,
      34,    66,     3,     3,     3,     3,     3,    41,     3,    66,
      10,    66,    66,   104,   117,   117,    18,    31,    20,    34,
      52,    53,    54,    55,    66,    69,     3,     3,    34,    66,
      66,    38,    42,    18,    66,     3,     3,    19,    19,   117,
     118,    66,    69,   106,    66,    66,   117,   117,   117,   117,
      66,    33,    35,   108,    66,   102,    68,    66,    90,    93,
      39,    20,    19,    20,    56,   107,     3,    18,    96,   114,
     115,   117,     3,    43,   108,    32,    20,    89,    24,    25,
      26,    27,    28,    92,    66,   118,    66,    57,   108,    54,
      97,    98,    99,     3,    37,    36,    43,    44,    45,    46,
      47,    48,    49,    50,    63,   116,   117,     3,     6,    90,
      19,     9,    18,    49,    94,    18,   107,    66,    58,   109,
      64,    65,    19,    20,   114,     8,    49,    50,    63,   117,
      20,    66,    89,     3,    64,    91,     8,    66,    86,    39,
      60,    61,   110,    20,    97,     8,   102,     3,    19,    20,
      19,   114,   118,   114,    59,   111,    96,    94,    86,     3,
     107,    60,   112,   117,    12,    62,   113,    20,   112
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    71,    72,    72,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    85,    86,    86,    87,    88,    89,
      89,    90,    90,    91,    92,    92,    92,    92,    92,    93,
      94,    94,    94,    95,    96,    96,    97,    97,    98,    98,
      98,    99,    99,    99,    99,   100,   101,   102,   102,   103,
     104,   105,   105,   105,   106,   106,   107,   107,   107,   108,
     108,   109,   109,   110,   110,   111,   111,   112,   112,   113,
     113,   113,   114,   114,   115,   115,   115,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     118,   118,   119
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
       1,     1,     1,     3,     3,     3,     4,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     2,     1,     1,     3,
       3,     4,     1,     2,     3,     3,     3,     3,     3,     3,
       1,     3,     8
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
#line 199 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1398 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 204 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1406 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 209 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1414 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 215 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1422 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 221 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1430 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 227 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1438 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 233 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1447 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 239 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1455 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 245 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1464 "yacc_sql.tab.c"
    break;

  case 31: /* show_index: SHOW INDEX FROM ID SEMICOLON  */
#line 252 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1473 "yacc_sql.tab.c"
    break;

  case 32: /* create_index: CREATE create_index_unique INDEX ID ON ID LBRACE create_index_attr_list RBRACE SEMICOLON  */
#line 260 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (const char **)(yyvsp[-2].list)->values, (yyvsp[-2].list)->len, (yyvsp[-8].boolean));
			list_free((yyvsp[-2].list));
		}
#line 1483 "yacc_sql.tab.c"
    break;

  case 33: /* create_index_unique: %empty  */
#line 268 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1489 "yacc_sql.tab.c"
    break;

  case 34: /* create_index_unique: UNIQUE  */
#line 269 "yacc_sql.y"
                 { (yyval.boolean) = true; }
#line 1495 "yacc_sql.tab.c"
    break;

  case 35: /* create_index_attr_list: ID  */
#line 273 "yacc_sql.y"
           {
		(yyval.list) = list_create(sizeof(char *), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].string));
	}
#line 1504 "yacc_sql.tab.c"
    break;

  case 36: /* create_index_attr_list: ID COMMA create_index_attr_list  */
#line 277 "yacc_sql.y"
                                          {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].string));
	}
#line 1513 "yacc_sql.tab.c"
    break;

  case 37: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 285 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1522 "yacc_sql.tab.c"
    break;

  case 38: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 292 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
		}
#line 1531 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 299 "yacc_sql.y"
                                   {    }
#line 1537 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type LBRACE number RBRACE attr_def_nullable  */
#line 304 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1547 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type attr_def_nullable  */
#line 310 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1557 "yacc_sql.tab.c"
    break;

  case 43: /* number: NUMBER  */
#line 317 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1563 "yacc_sql.tab.c"
    break;

  case 44: /* type: INT_T  */
#line 320 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1569 "yacc_sql.tab.c"
    break;

  case 45: /* type: STRING_T  */
#line 321 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1575 "yacc_sql.tab.c"
    break;

  case 46: /* type: DATE_T  */
#line 322 "yacc_sql.y"
                { (yyval.number)=DATE; }
#line 1581 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 323 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1587 "yacc_sql.tab.c"
    break;

  case 48: /* type: TEXT_T  */
#line 324 "yacc_sql.y"
                    { (yyval.number)=TEXT; }
#line 1593 "yacc_sql.tab.c"
    break;

  case 49: /* ID_get: ID  */
#line 328 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1602 "yacc_sql.tab.c"
    break;

  case 50: /* attr_def_nullable: %empty  */
#line 334 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1608 "yacc_sql.tab.c"
    break;

  case 51: /* attr_def_nullable: NOT NULL_VALUE  */
#line 335 "yacc_sql.y"
                         { (yyval.boolean) = false; }
#line 1614 "yacc_sql.tab.c"
    break;

  case 52: /* attr_def_nullable: NULLABLE  */
#line 336 "yacc_sql.y"
                   { (yyval.boolean) = true; }
#line 1620 "yacc_sql.tab.c"
    break;

  case 53: /* insert: INSERT INTO ID VALUES value_lists SEMICOLON  */
#line 342 "yacc_sql.y"
        {
	  	CONTEXT->ssql->flag=SCF_INSERT;//"insert";
		inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-3].string), (Value *)(yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		list_free((yyvsp[-1].list));
    }
#line 1630 "yacc_sql.tab.c"
    break;

  case 54: /* value_lists: LBRACE value_list RBRACE  */
#line 349 "yacc_sql.y"
                                 {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[-1].list);
	}
#line 1639 "yacc_sql.tab.c"
    break;

  case 55: /* value_lists: LBRACE value_list RBRACE COMMA value_lists  */
#line 353 "yacc_sql.y"
                                                     {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[0].list);
		list_prepend_list((yyval.list), (yyvsp[-3].list));
		list_free((yyvsp[-3].list));
	}
#line 1650 "yacc_sql.tab.c"
    break;

  case 56: /* value_list: value  */
#line 363 "yacc_sql.y"
              {
		(yyval.list) = list_create(sizeof(Value), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].value));
	}
#line 1659 "yacc_sql.tab.c"
    break;

  case 57: /* value_list: value COMMA value_list  */
#line 367 "yacc_sql.y"
                              { 
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].value));
	}
#line 1668 "yacc_sql.tab.c"
    break;

  case 58: /* value: pos_value  */
#line 374 "yacc_sql.y"
                  {
		(yyval.value) = (yyvsp[0].value);
	}
#line 1676 "yacc_sql.tab.c"
    break;

  case 59: /* value: MINUS NUMBER  */
#line 377 "yacc_sql.y"
                  {	
  		value_init_integer(&(yyval.value), -(yyvsp[0].number));
	}
#line 1684 "yacc_sql.tab.c"
    break;

  case 60: /* value: MINUS FLOAT  */
#line 380 "yacc_sql.y"
                 {
  		value_init_float(&(yyval.value), -(yyvsp[0].floats));
	}
#line 1692 "yacc_sql.tab.c"
    break;

  case 61: /* pos_value: NULL_VALUE  */
#line 386 "yacc_sql.y"
                   {
		value_init_null(&(yyval.value));
	}
#line 1700 "yacc_sql.tab.c"
    break;

  case 62: /* pos_value: NUMBER  */
#line 389 "yacc_sql.y"
             {	
  		value_init_integer(&(yyval.value), (yyvsp[0].number));
	}
#line 1708 "yacc_sql.tab.c"
    break;

  case 63: /* pos_value: FLOAT  */
#line 392 "yacc_sql.y"
            {
  		value_init_float(&(yyval.value), (yyvsp[0].floats));
		}
#line 1716 "yacc_sql.tab.c"
    break;

  case 64: /* pos_value: SSS  */
#line 395 "yacc_sql.y"
          {
		(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&(yyval.value), (yyvsp[0].string));
	}
#line 1725 "yacc_sql.tab.c"
    break;

  case 65: /* delete: DELETE FROM ID where SEMICOLON  */
#line 403 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1737 "yacc_sql.tab.c"
    break;

  case 66: /* update: UPDATE ID SET update_set_list where SEMICOLON  */
#line 413 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-4].string), (KeyValue *) (yyvsp[-2].list)->values, (yyvsp[-2].list)->len, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-2].list));
			list_free((yyvsp[-1].list));
		}
#line 1749 "yacc_sql.tab.c"
    break;

  case 67: /* update_set_list: ID EQ expr  */
#line 424 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(KeyValue), MAX_NUM);
		KeyValue kv;
		kv.name = (yyvsp[-2].string);
		kv.value = (yyvsp[0].expr);
		list_prepend((yyval.list), &kv);
	}
#line 1761 "yacc_sql.tab.c"
    break;

  case 68: /* update_set_list: ID EQ expr COMMA update_set_list  */
#line 432 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		KeyValue kv;
		kv.name = (yyvsp[-4].string);
		kv.value = (yyvsp[-2].expr);
		list_prepend((yyval.list), &kv);
	}
#line 1773 "yacc_sql.tab.c"
    break;

  case 69: /* select: select_stmt SEMICOLON  */
#line 443 "yacc_sql.y"
        {
		CONTEXT->ssql->sstr.selection = (yyvsp[-1].select);
		CONTEXT->ssql->flag=SCF_SELECT;//"select";
	}
#line 1782 "yacc_sql.tab.c"
    break;

  case 70: /* select_stmt: SELECT attr_list FROM ID rel_list where group_by having order_by  */
#line 451 "yacc_sql.y"
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
#line 1810 "yacc_sql.tab.c"
    break;

  case 71: /* select_attr: expr  */
#line 478 "yacc_sql.y"
        {
		(yyval.attr).expr = (yyvsp[0].expr);
		(yyval.attr).name = NULL;
	}
#line 1819 "yacc_sql.tab.c"
    break;

  case 72: /* select_attr: expr ID  */
#line 483 "yacc_sql.y"
        {
		(yyval.attr).expr = (yyvsp[-1].expr);
		(yyval.attr).name = (yyvsp[0].string);
	}
#line 1828 "yacc_sql.tab.c"
    break;

  case 73: /* select_attr: expr AS ID  */
#line 488 "yacc_sql.y"
        {
		(yyval.attr).expr = (yyvsp[-2].expr);
		(yyval.attr).name = (yyvsp[0].string);
	}
#line 1837 "yacc_sql.tab.c"
    break;

  case 74: /* attr_list: select_attr  */
#line 496 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(AttrExpr), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].attr));
	}
#line 1846 "yacc_sql.tab.c"
    break;

  case 75: /* attr_list: select_attr COMMA attr_list  */
#line 501 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].attr));
	}
#line 1855 "yacc_sql.tab.c"
    break;

  case 77: /* rel_list: COMMA ID rel_list  */
#line 509 "yacc_sql.y"
                        {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string),NULL, 0);
	}
#line 1864 "yacc_sql.tab.c"
    break;

  case 78: /* rel_list: INNER JOIN ID ON condition_list rel_list  */
#line 513 "yacc_sql.y"
                                                  {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
		selects_append_join_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string),(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		list_free((yyvsp[-1].list));
	}
#line 1875 "yacc_sql.tab.c"
    break;

  case 79: /* where: %empty  */
#line 520 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(Condition), MAX_NUM); }
#line 1881 "yacc_sql.tab.c"
    break;

  case 80: /* where: WHERE condition_list  */
#line 521 "yacc_sql.y"
                           {	
		(yyval.list) = (yyvsp[0].list);
	}
#line 1889 "yacc_sql.tab.c"
    break;

  case 81: /* group_by: %empty  */
#line 526 "yacc_sql.y"
        { (yyval.list) = list_create(sizeof(UnionExpr), 0); }
#line 1895 "yacc_sql.tab.c"
    break;

  case 82: /* group_by: GROUP BY expr_list  */
#line 528 "yacc_sql.y"
        {
	  (yyval.list) = (yyvsp[0].list);
	}
#line 1903 "yacc_sql.tab.c"
    break;

  case 83: /* having: %empty  */
#line 533 "yacc_sql.y"
        { (yyval.list) = list_create(sizeof(Condition), 0); }
#line 1909 "yacc_sql.tab.c"
    break;

  case 84: /* having: HAVING condition_list  */
#line 535 "yacc_sql.y"
        {
	  (yyval.list) = (yyvsp[0].list);
	}
#line 1917 "yacc_sql.tab.c"
    break;

  case 85: /* order_by: %empty  */
#line 540 "yacc_sql.y"
        { (yyval.list) = list_create(sizeof(OrderExpr), 0); }
#line 1923 "yacc_sql.tab.c"
    break;

  case 86: /* order_by: ORDER BY order_by_list  */
#line 542 "yacc_sql.y"
        {
	  (yyval.list) = (yyvsp[0].list);
	}
#line 1931 "yacc_sql.tab.c"
    break;

  case 87: /* order_by_list: expr order_direct  */
#line 547 "yacc_sql.y"
                          {
		(yyval.list) = list_create(sizeof(OrderExpr), MAX_NUM);
		OrderExpr order;
		order.expr = (yyvsp[-1].expr);
		order.asc = (yyvsp[0].boolean);
		list_prepend((yyval.list), &order);
	}
#line 1943 "yacc_sql.tab.c"
    break;

  case 88: /* order_by_list: expr order_direct COMMA order_by_list  */
#line 554 "yacc_sql.y"
                                                {
		(yyval.list) = (yyvsp[0].list);
		OrderExpr order;
		order.expr = (yyvsp[-3].expr);
		order.asc = (yyvsp[-2].boolean);
		list_prepend((yyval.list), &order);
	}
#line 1955 "yacc_sql.tab.c"
    break;

  case 89: /* order_direct: %empty  */
#line 563 "yacc_sql.y"
        { (yyval.boolean) = true; }
#line 1961 "yacc_sql.tab.c"
    break;

  case 90: /* order_direct: ASC  */
#line 564 "yacc_sql.y"
              { (yyval.boolean) = true; }
#line 1967 "yacc_sql.tab.c"
    break;

  case 91: /* order_direct: DESC  */
#line 565 "yacc_sql.y"
               { (yyval.boolean) = false; }
#line 1973 "yacc_sql.tab.c"
    break;

  case 92: /* condition_list: condition  */
#line 569 "yacc_sql.y"
                  {
		(yyval.list) = list_create(sizeof(Condition), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].condition));
	}
#line 1982 "yacc_sql.tab.c"
    break;

  case 93: /* condition_list: condition AND condition_list  */
#line 573 "yacc_sql.y"
                                   {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].condition));
	}
#line 1991 "yacc_sql.tab.c"
    break;

  case 94: /* condition: expr comOp expr  */
#line 579 "yacc_sql.y"
                        {
		condition_init(&(yyval.condition), (yyvsp[-1].comp_op), &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 1999 "yacc_sql.tab.c"
    break;

  case 95: /* condition: expr IS NULL_VALUE  */
#line 582 "yacc_sql.y"
                             {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), IS_NULL, &(yyvsp[-2].expr), &null_expr);
	}
#line 2014 "yacc_sql.tab.c"
    break;

  case 96: /* condition: expr IS NOT NULL_VALUE  */
#line 592 "yacc_sql.y"
                                 {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), IS_NOT_NULL, &(yyvsp[-3].expr), &null_expr);
	}
#line 2029 "yacc_sql.tab.c"
    break;

  case 97: /* comOp: EQ  */
#line 605 "yacc_sql.y"
             { (yyval.comp_op) = EQUAL_TO; }
#line 2035 "yacc_sql.tab.c"
    break;

  case 98: /* comOp: LT  */
#line 606 "yacc_sql.y"
         { (yyval.comp_op) = LESS_THAN; }
#line 2041 "yacc_sql.tab.c"
    break;

  case 99: /* comOp: GT  */
#line 607 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_THAN; }
#line 2047 "yacc_sql.tab.c"
    break;

  case 100: /* comOp: LE  */
#line 608 "yacc_sql.y"
         { (yyval.comp_op) = LESS_EQUAL; }
#line 2053 "yacc_sql.tab.c"
    break;

  case 101: /* comOp: GE  */
#line 609 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_EQUAL; }
#line 2059 "yacc_sql.tab.c"
    break;

  case 102: /* comOp: NE  */
#line 610 "yacc_sql.y"
         { (yyval.comp_op) = NOT_EQUAL; }
#line 2065 "yacc_sql.tab.c"
    break;

  case 103: /* comOp: LIKE  */
#line 611 "yacc_sql.y"
               { (yyval.comp_op) = OP_LIKE; }
#line 2071 "yacc_sql.tab.c"
    break;

  case 104: /* comOp: NOT LIKE  */
#line 612 "yacc_sql.y"
                   { (yyval.comp_op) = OP_NOT_LIKE; }
#line 2077 "yacc_sql.tab.c"
    break;

  case 105: /* comOp: IN  */
#line 613 "yacc_sql.y"
             { (yyval.comp_op) = OP_IN; }
#line 2083 "yacc_sql.tab.c"
    break;

  case 106: /* comOp: NOT IN  */
#line 614 "yacc_sql.y"
                 { (yyval.comp_op) = OP_NOT_IN; }
#line 2089 "yacc_sql.tab.c"
    break;

  case 107: /* expr: STAR  */
#line 619 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, NULL, "*");
	}
#line 2098 "yacc_sql.tab.c"
    break;

  case 108: /* expr: ID  */
#line 623 "yacc_sql.y"
             {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, NULL, (yyvsp[0].string));
	}
#line 2107 "yacc_sql.tab.c"
    break;

  case 109: /* expr: ID DOT STAR  */
#line 628 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, (yyvsp[-2].string), "*");
	}
#line 2116 "yacc_sql.tab.c"
    break;

  case 110: /* expr: ID DOT ID  */
#line 632 "yacc_sql.y"
                    {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, (yyvsp[-2].string), (yyvsp[0].string));
	}
#line 2125 "yacc_sql.tab.c"
    break;

  case 111: /* expr: ID LBRACE expr_list RBRACE  */
#line 637 "yacc_sql.y"
        {
		(yyval.expr).type = EXPR_FUNC;
		func_init(&(yyval.expr).value.func, (yyvsp[-3].string), (UnionExpr *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
	}
#line 2134 "yacc_sql.tab.c"
    break;

  case 112: /* expr: pos_value  */
#line 641 "yacc_sql.y"
                    {
		(yyval.expr).type = EXPR_VALUE;
		(yyval.expr).value.value = (yyvsp[0].value);
	}
#line 2143 "yacc_sql.tab.c"
    break;

  case 113: /* expr: MINUS expr  */
#line 645 "yacc_sql.y"
                     {
		(yyval.expr).type = EXPR_FUNC;
		func_init_1(&(yyval.expr).value.func, "neg", &(yyvsp[0].expr));
	}
#line 2152 "yacc_sql.tab.c"
    break;

  case 114: /* expr: expr ADD expr  */
#line 649 "yacc_sql.y"
                        {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "+", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2161 "yacc_sql.tab.c"
    break;

  case 115: /* expr: expr MINUS expr  */
#line 653 "yacc_sql.y"
                          {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "-", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2170 "yacc_sql.tab.c"
    break;

  case 116: /* expr: expr STAR expr  */
#line 657 "yacc_sql.y"
                         {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "*", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2179 "yacc_sql.tab.c"
    break;

  case 117: /* expr: expr DIV expr  */
#line 661 "yacc_sql.y"
                        {
		(yyval.expr).type = EXPR_FUNC;
		func_init_2(&(yyval.expr).value.func, "/", &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 2188 "yacc_sql.tab.c"
    break;

  case 118: /* expr: LBRACE expr RBRACE  */
#line 665 "yacc_sql.y"
                             {
		(yyval.expr) = (yyvsp[-1].expr);
	}
#line 2196 "yacc_sql.tab.c"
    break;

  case 119: /* expr: LBRACE select_stmt RBRACE  */
#line 668 "yacc_sql.y"
                                    {
		expr_init_selects(&(yyval.expr), &(yyvsp[-1].select));
	}
#line 2204 "yacc_sql.tab.c"
    break;

  case 120: /* expr_list: expr  */
#line 675 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(UnionExpr), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].expr));
	}
#line 2213 "yacc_sql.tab.c"
    break;

  case 121: /* expr_list: expr COMMA expr_list  */
#line 680 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].expr));
	}
#line 2222 "yacc_sql.tab.c"
    break;

  case 122: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 688 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2231 "yacc_sql.tab.c"
    break;


#line 2235 "yacc_sql.tab.c"

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

#line 693 "yacc_sql.y"

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
