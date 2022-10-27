/* A Bison parser, made by GNU Bison 3.7.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7"

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
  YYSYMBOL_INNER = 52,                     /* INNER  */
  YYSYMBOL_JOIN = 53,                      /* JOIN  */
  YYSYMBOL_NUMBER = 54,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 55,                     /* FLOAT  */
  YYSYMBOL_ID = 56,                        /* ID  */
  YYSYMBOL_PATH = 57,                      /* PATH  */
  YYSYMBOL_SSS = 58,                       /* SSS  */
  YYSYMBOL_STAR = 59,                      /* STAR  */
  YYSYMBOL_STRING_V = 60,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 61,                  /* $accept  */
  YYSYMBOL_commands = 62,                  /* commands  */
  YYSYMBOL_command = 63,                   /* command  */
  YYSYMBOL_exit = 64,                      /* exit  */
  YYSYMBOL_help = 65,                      /* help  */
  YYSYMBOL_sync = 66,                      /* sync  */
  YYSYMBOL_begin = 67,                     /* begin  */
  YYSYMBOL_commit = 68,                    /* commit  */
  YYSYMBOL_rollback = 69,                  /* rollback  */
  YYSYMBOL_drop_table = 70,                /* drop_table  */
  YYSYMBOL_show_tables = 71,               /* show_tables  */
  YYSYMBOL_desc_table = 72,                /* desc_table  */
  YYSYMBOL_show_index = 73,                /* show_index  */
  YYSYMBOL_create_index = 74,              /* create_index  */
  YYSYMBOL_create_index_unique = 75,       /* create_index_unique  */
  YYSYMBOL_create_index_attr_list = 76,    /* create_index_attr_list  */
  YYSYMBOL_drop_index = 77,                /* drop_index  */
  YYSYMBOL_create_table = 78,              /* create_table  */
  YYSYMBOL_attr_def_list = 79,             /* attr_def_list  */
  YYSYMBOL_attr_def = 80,                  /* attr_def  */
  YYSYMBOL_number = 81,                    /* number  */
  YYSYMBOL_type = 82,                      /* type  */
  YYSYMBOL_ID_get = 83,                    /* ID_get  */
  YYSYMBOL_attr_def_nullable = 84,         /* attr_def_nullable  */
  YYSYMBOL_insert = 85,                    /* insert  */
  YYSYMBOL_value_lists = 86,               /* value_lists  */
  YYSYMBOL_value_list = 87,                /* value_list  */
  YYSYMBOL_value = 88,                     /* value  */
  YYSYMBOL_delete = 89,                    /* delete  */
  YYSYMBOL_update = 90,                    /* update  */
  YYSYMBOL_update_set_list = 91,           /* update_set_list  */
  YYSYMBOL_select = 92,                    /* select  */
  YYSYMBOL_select_attr = 93,               /* select_attr  */
  YYSYMBOL_attr_list = 94,                 /* attr_list  */
  YYSYMBOL_rel_list = 95,                  /* rel_list  */
  YYSYMBOL_where = 96,                     /* where  */
  YYSYMBOL_condition_list = 97,            /* condition_list  */
  YYSYMBOL_condition = 98,                 /* condition  */
  YYSYMBOL_comOp = 99,                     /* comOp  */
  YYSYMBOL_expr = 100,                     /* expr  */
  YYSYMBOL_load_data = 101                 /* load_data  */
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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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
#define YYLAST   161

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  194

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   315


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
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   149,   149,   151,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   176,   181,   186,   192,   198,   204,   210,   216,
     222,   229,   236,   245,   246,   250,   254,   261,   268,   274,
     276,   280,   286,   294,   297,   298,   299,   300,   301,   304,
     311,   312,   313,   318,   326,   330,   340,   344,   351,   354,
     357,   360,   367,   377,   388,   396,   407,   419,   424,   429,
     434,   440,   442,   447,   452,   457,   464,   466,   470,   477,
     478,   483,   487,   493,   496,   506,   519,   520,   521,   522,
     523,   524,   525,   526,   530,   534,   538,   544
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
  "EQ", "LT", "GT", "LE", "GE", "NE", "NOT", "LIKE", "UNIQUE", "INNER",
  "JOIN", "NUMBER", "FLOAT", "ID", "PATH", "SSS", "STAR", "STRING_V",
  "$accept", "commands", "command", "exit", "help", "sync", "begin",
  "commit", "rollback", "drop_table", "show_tables", "desc_table",
  "show_index", "create_index", "create_index_unique",
  "create_index_attr_list", "drop_index", "create_table", "attr_def_list",
  "attr_def", "number", "type", "ID_get", "attr_def_nullable", "insert",
  "value_lists", "value_list", "value", "delete", "update",
  "update_set_list", "select", "select_attr", "attr_list", "rel_list",
  "where", "condition_list", "condition", "comOp", "expr", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315
};
#endif

#define YYPACT_NINF (-133)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -133,    59,  -133,     0,    73,   -45,   -37,    10,    30,     2,
       5,     4,    55,    63,    64,    66,    74,    37,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,    36,  -133,    76,
      39,    40,   -10,    77,    67,    90,    95,    68,  -133,    44,
      47,    69,  -133,  -133,  -133,  -133,  -133,    62,    87,    50,
     105,   106,     9,    31,  -133,  -133,    54,  -133,  -133,    56,
      78,    79,    57,    58,    61,    80,  -133,  -133,    -8,    77,
      77,    77,   -12,   112,   100,    -6,   117,    81,    79,    89,
    -133,   102,    17,    70,    35,  -133,  -133,  -133,  -133,    71,
      72,    79,  -133,    -1,   120,  -133,  -133,  -133,    97,  -133,
    -133,  -133,    92,   -18,  -133,    -1,   127,   125,    61,   113,
    -133,  -133,  -133,  -133,  -133,     6,   115,    77,    77,   -12,
      82,   131,   116,   119,  -133,    84,    -6,    -3,  -133,  -133,
    -133,  -133,  -133,  -133,    86,  -133,    -6,   121,  -133,    88,
     102,   134,  -133,    91,   135,  -133,    93,  -133,  -133,  -133,
     103,  -133,   126,    -1,  -133,  -133,  -133,   139,  -133,  -133,
      57,   145,  -133,  -133,  -133,   132,  -133,   130,   133,    -6,
     100,  -133,  -133,  -133,  -133,     7,    93,   150,   -12,  -133,
    -133,  -133,  -133,  -133
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     1,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    14,    12,
      13,     8,     5,     7,     6,     4,    19,     0,    34,     0,
       0,     0,    71,    71,     0,     0,     0,     0,    24,     0,
       0,     0,    25,    26,    27,    23,    22,     0,     0,     0,
       0,     0,     0,     0,    68,    67,     0,    30,    29,     0,
       0,    79,     0,     0,     0,     0,    28,    37,    71,    71,
      71,    71,    76,     0,     0,     0,     0,     0,    79,     0,
      49,    39,     0,     0,     0,    73,    72,    69,    70,     0,
       0,    79,    31,     0,     0,    58,    59,    60,    94,    61,
      96,    80,    81,     0,    62,     0,     0,     0,     0,     0,
      44,    45,    46,    47,    48,    50,     0,    71,    71,    76,
       0,     0,     0,    56,    53,     0,     0,     0,    86,    87,
      88,    89,    90,    91,     0,    92,     0,    64,    63,     0,
      39,     0,    52,     0,     0,    42,     0,    74,    75,    77,
       0,    66,    54,     0,    95,    82,    84,     0,    93,    83,
       0,     0,    40,    38,    43,     0,    51,    35,     0,     0,
       0,    57,    85,    65,    97,    50,     0,     0,    76,    55,
      41,    36,    32,    78
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,   -32,  -133,  -133,     8,    38,
    -133,  -133,  -133,   -30,  -133,   -23,    -4,  -102,  -133,  -133,
      -9,  -133,  -133,   -43,  -126,   -79,  -132,  -133,  -133,    14,
    -133
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    39,   178,    30,    31,   119,    91,
     175,   125,    92,   155,    32,   104,   132,   110,    33,    34,
      88,    35,    44,    64,   101,    86,   111,   112,   146,   113,
      36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      65,   133,   105,   159,   165,   166,    37,   105,    99,   116,
      62,    42,    62,   147,    43,   152,   152,    46,   137,    45,
      47,    63,   131,    94,   153,   138,   139,   140,   141,   142,
     143,   144,   145,    48,    49,    95,    96,    97,    98,    50,
     100,   120,   121,   122,   123,   124,   167,   188,   106,   107,
     108,    38,   109,   106,   107,   154,   154,   109,    52,     2,
      51,   133,   193,     3,     4,    78,    53,    54,    79,    55,
       5,     6,     7,     8,     9,    10,    11,    56,    57,    40,
      12,    13,    14,    41,   157,   158,    59,    80,    15,    16,
      81,   127,    58,    67,   128,    60,    61,    62,    68,    17,
      70,    66,    69,    71,    73,    74,    75,    72,    76,    77,
      82,    84,    83,    87,    85,   102,    89,    90,   103,    93,
     114,   117,   118,   134,   115,   130,   126,   129,   135,   136,
     148,   149,   151,   156,   161,   162,   168,   173,   160,   163,
     164,   170,   179,   176,   171,   174,   180,   182,   184,   177,
     186,   185,   187,   192,   191,   190,   150,   189,   172,   181,
     169,   183
};

static const yytype_uint8 yycheck[] =
{
      43,   103,     8,   129,   136,     8,     6,     8,    20,    88,
      20,    56,    20,   115,    59,     9,     9,     7,    36,    56,
      10,    31,   101,    31,    18,    43,    44,    45,    46,    47,
      48,    49,    50,     3,    32,    78,    79,    80,    81,    34,
      52,    24,    25,    26,    27,    28,    49,   179,    54,    55,
      56,    51,    58,    54,    55,    49,    49,    58,     3,     0,
      56,   163,   188,     4,     5,    56,     3,     3,    59,     3,
      11,    12,    13,    14,    15,    16,    17,     3,    41,     6,
      21,    22,    23,    10,   127,   128,    10,    56,    29,    30,
      59,    56,    56,     3,    59,    56,    56,    20,     3,    40,
      56,    34,    34,    56,    42,    18,    56,    38,     3,     3,
      56,    33,    56,    56,    35,     3,    58,    56,    18,    39,
       3,    32,    20,     3,    43,    53,    56,    56,    31,    37,
       3,     6,    19,    18,     3,    19,    50,     3,    56,    20,
      56,    20,    39,     8,    56,    54,    20,     8,     3,    56,
      20,    19,    19,     3,   186,   185,   118,   180,   150,   163,
     146,   170
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    62,     0,     4,     5,    11,    12,    13,    14,    15,
      16,    17,    21,    22,    23,    29,    30,    40,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      77,    78,    85,    89,    90,    92,   101,     6,    51,    75,
       6,    10,    56,    59,    93,    56,     7,    10,     3,    32,
      34,    56,     3,     3,     3,     3,     3,    41,    56,    10,
      56,    56,    20,    31,    94,    94,    34,     3,     3,    34,
      56,    56,    38,    42,    18,    56,     3,     3,    56,    59,
      56,    59,    56,    56,    33,    35,    96,    56,    91,    58,
      56,    80,    83,    39,    31,    94,    94,    94,    94,    20,
      52,    95,     3,    18,    86,     8,    54,    55,    56,    58,
      88,    97,    98,   100,     3,    43,    96,    32,    20,    79,
      24,    25,    26,    27,    28,    82,    56,    56,    59,    56,
      53,    96,    87,    88,     3,    31,    37,    36,    43,    44,
      45,    46,    47,    48,    49,    50,    99,    88,     3,     6,
      80,    19,     9,    18,    49,    84,    18,    94,    94,    95,
      56,     3,    19,    20,    56,    97,     8,    49,    50,   100,
      20,    56,    79,     3,    54,    81,     8,    56,    76,    39,
      20,    87,     8,    91,     3,    19,    20,    19,    97,    86,
      84,    76,     3,    95
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    61,    62,    62,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    75,    76,    76,    77,    78,    79,
      79,    80,    80,    81,    82,    82,    82,    82,    82,    83,
      84,    84,    84,    85,    86,    86,    87,    87,    88,    88,
      88,    88,    89,    90,    91,    91,    92,    93,    93,    93,
      93,    94,    94,    94,    94,    94,    95,    95,    95,    96,
      96,    97,    97,    98,    98,    98,    99,    99,    99,    99,
      99,    99,    99,    99,   100,   100,   100,   101
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       3,     5,    10,     0,     1,     1,     3,     4,     8,     0,
       3,     6,     3,     1,     1,     1,     1,     1,     1,     1,
       0,     2,     1,     6,     3,     5,     1,     3,     1,     1,
       1,     1,     5,     6,     3,     5,     7,     2,     2,     4,
       4,     0,     3,     3,     5,     5,     0,     3,     6,     0,
       2,     1,     3,     3,     3,     4,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     3,     1,     8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


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
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
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
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
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
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
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
#line 176 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1347 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 181 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1355 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 186 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1363 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 192 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1371 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 198 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1379 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 204 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1387 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 210 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1396 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 216 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1404 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 222 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1413 "yacc_sql.tab.c"
    break;

  case 31: /* show_index: SHOW INDEX FROM ID SEMICOLON  */
#line 229 "yacc_sql.y"
                                 {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1422 "yacc_sql.tab.c"
    break;

  case 32: /* create_index: CREATE create_index_unique INDEX ID ON ID LBRACE create_index_attr_list RBRACE SEMICOLON  */
#line 237 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (const char **)(yyvsp[-2].list)->values, (yyvsp[-2].list)->len, (yyvsp[-8].boolean));
			list_free((yyvsp[-2].list));
		}
#line 1432 "yacc_sql.tab.c"
    break;

  case 33: /* create_index_unique: %empty  */
#line 245 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1438 "yacc_sql.tab.c"
    break;

  case 34: /* create_index_unique: UNIQUE  */
#line 246 "yacc_sql.y"
                 { (yyval.boolean) = true; }
#line 1444 "yacc_sql.tab.c"
    break;

  case 35: /* create_index_attr_list: ID  */
#line 250 "yacc_sql.y"
           {
		(yyval.list) = list_create(sizeof(char *), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].string));
	}
#line 1453 "yacc_sql.tab.c"
    break;

  case 36: /* create_index_attr_list: ID COMMA create_index_attr_list  */
#line 254 "yacc_sql.y"
                                          {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].string));
	}
#line 1462 "yacc_sql.tab.c"
    break;

  case 37: /* drop_index: DROP INDEX ID SEMICOLON  */
#line 262 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-1].string));
		}
#line 1471 "yacc_sql.tab.c"
    break;

  case 38: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 269 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
		}
#line 1480 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 276 "yacc_sql.y"
                                   {    }
#line 1486 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type LBRACE number RBRACE attr_def_nullable  */
#line 281 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-4].number), (yyvsp[-2].number), (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1496 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type attr_def_nullable  */
#line 287 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-1].number), 4, (yyvsp[0].boolean));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
#line 1506 "yacc_sql.tab.c"
    break;

  case 43: /* number: NUMBER  */
#line 294 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1512 "yacc_sql.tab.c"
    break;

  case 44: /* type: INT_T  */
#line 297 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1518 "yacc_sql.tab.c"
    break;

  case 45: /* type: STRING_T  */
#line 298 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1524 "yacc_sql.tab.c"
    break;

  case 46: /* type: DATE_T  */
#line 299 "yacc_sql.y"
                { (yyval.number)=DATE; }
#line 1530 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 300 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1536 "yacc_sql.tab.c"
    break;

  case 48: /* type: TEXT_T  */
#line 301 "yacc_sql.y"
                    { (yyval.number)=TEXT; }
#line 1542 "yacc_sql.tab.c"
    break;

  case 49: /* ID_get: ID  */
#line 305 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1551 "yacc_sql.tab.c"
    break;

  case 50: /* attr_def_nullable: %empty  */
#line 311 "yacc_sql.y"
        { (yyval.boolean) = false; }
#line 1557 "yacc_sql.tab.c"
    break;

  case 51: /* attr_def_nullable: NOT NULL_VALUE  */
#line 312 "yacc_sql.y"
                         { (yyval.boolean) = false; }
#line 1563 "yacc_sql.tab.c"
    break;

  case 52: /* attr_def_nullable: NULLABLE  */
#line 313 "yacc_sql.y"
                   { (yyval.boolean) = true; }
#line 1569 "yacc_sql.tab.c"
    break;

  case 53: /* insert: INSERT INTO ID VALUES value_lists SEMICOLON  */
#line 319 "yacc_sql.y"
        {
	  	CONTEXT->ssql->flag=SCF_INSERT;//"insert";
		inserts_init(&CONTEXT->ssql->sstr.insertion, (yyvsp[-3].string), (Value *)(yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		list_free((yyvsp[-1].list));
    }
#line 1579 "yacc_sql.tab.c"
    break;

  case 54: /* value_lists: LBRACE value_list RBRACE  */
#line 326 "yacc_sql.y"
                                 {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[-1].list);
	}
#line 1588 "yacc_sql.tab.c"
    break;

  case 55: /* value_lists: LBRACE value_list RBRACE COMMA value_lists  */
#line 330 "yacc_sql.y"
                                                     {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		(yyval.list) = (yyvsp[0].list);
		list_prepend_list((yyval.list), (yyvsp[-3].list));
		list_free((yyvsp[-3].list));
	}
#line 1599 "yacc_sql.tab.c"
    break;

  case 56: /* value_list: value  */
#line 340 "yacc_sql.y"
              {
		(yyval.list) = list_create(sizeof(Value), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].value));
	}
#line 1608 "yacc_sql.tab.c"
    break;

  case 57: /* value_list: value COMMA value_list  */
#line 344 "yacc_sql.y"
                              { 
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].value));
	}
#line 1617 "yacc_sql.tab.c"
    break;

  case 58: /* value: NULL_VALUE  */
#line 351 "yacc_sql.y"
                   {
		value_init_null(&(yyval.value));
	}
#line 1625 "yacc_sql.tab.c"
    break;

  case 59: /* value: NUMBER  */
#line 354 "yacc_sql.y"
           {	
  		value_init_integer(&(yyval.value), (yyvsp[0].number));
	}
#line 1633 "yacc_sql.tab.c"
    break;

  case 60: /* value: FLOAT  */
#line 357 "yacc_sql.y"
          {
  		value_init_float(&(yyval.value), (yyvsp[0].floats));
	}
#line 1641 "yacc_sql.tab.c"
    break;

  case 61: /* value: SSS  */
#line 360 "yacc_sql.y"
         {
		(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&(yyval.value), (yyvsp[0].string));
	}
#line 1650 "yacc_sql.tab.c"
    break;

  case 62: /* delete: DELETE FROM ID where SEMICOLON  */
#line 368 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));
    }
#line 1662 "yacc_sql.tab.c"
    break;

  case 63: /* update: UPDATE ID SET update_set_list where SEMICOLON  */
#line 378 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-4].string), (KeyValue *) (yyvsp[-2].list)->values, (yyvsp[-2].list)->len, 
					(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-2].list));
			list_free((yyvsp[-1].list));
		}
#line 1674 "yacc_sql.tab.c"
    break;

  case 64: /* update_set_list: ID EQ value  */
#line 389 "yacc_sql.y"
        {
		(yyval.list) = list_create(sizeof(KeyValue), MAX_NUM);
		KeyValue kv;
		kv.name = (yyvsp[-2].string);
		kv.value = (yyvsp[0].value);
		list_prepend((yyval.list), &kv);
	}
#line 1686 "yacc_sql.tab.c"
    break;

  case 65: /* update_set_list: ID EQ value COMMA update_set_list  */
#line 397 "yacc_sql.y"
        {
		(yyval.list) = (yyvsp[0].list);
		KeyValue kv;
		kv.name = (yyvsp[-4].string);
		kv.value = (yyvsp[-2].value);
		list_prepend((yyval.list), &kv);
	}
#line 1698 "yacc_sql.tab.c"
    break;

  case 66: /* select: SELECT select_attr FROM ID rel_list where SEMICOLON  */
#line 408 "yacc_sql.y"
                {
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
			list_free((yyvsp[-1].list));

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
	}
#line 1711 "yacc_sql.tab.c"
    break;

  case 67: /* select_attr: STAR attr_list  */
#line 419 "yacc_sql.y"
                  {  
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1721 "yacc_sql.tab.c"
    break;

  case 68: /* select_attr: ID attr_list  */
#line 424 "yacc_sql.y"
                   {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1731 "yacc_sql.tab.c"
    break;

  case 69: /* select_attr: ID DOT ID attr_list  */
#line 429 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1741 "yacc_sql.tab.c"
    break;

  case 70: /* select_attr: ID DOT STAR attr_list  */
#line 434 "yacc_sql.y"
                               {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 1751 "yacc_sql.tab.c"
    break;

  case 72: /* attr_list: COMMA STAR attr_list  */
#line 442 "yacc_sql.y"
                              {
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	 }
#line 1761 "yacc_sql.tab.c"
    break;

  case 73: /* attr_list: COMMA ID attr_list  */
#line 447 "yacc_sql.y"
                         {
			RelAttr attr;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
      }
#line 1771 "yacc_sql.tab.c"
    break;

  case 74: /* attr_list: COMMA ID DOT ID attr_list  */
#line 452 "yacc_sql.y"
                                {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
  	  }
#line 1781 "yacc_sql.tab.c"
    break;

  case 75: /* attr_list: COMMA ID DOT STAR attr_list  */
#line 457 "yacc_sql.y"
                                     {
			RelAttr attr;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 1791 "yacc_sql.tab.c"
    break;

  case 77: /* rel_list: COMMA ID rel_list  */
#line 466 "yacc_sql.y"
                        {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string),NULL, 0);
	}
#line 1800 "yacc_sql.tab.c"
    break;

  case 78: /* rel_list: INNER JOIN ID ON condition_list rel_list  */
#line 470 "yacc_sql.y"
                                                  {
		selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string));
		selects_append_join_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, (yyvsp[-3].string),(Condition *) (yyvsp[-1].list)->values, (yyvsp[-1].list)->len);
		list_free((yyvsp[-1].list));
	}
#line 1811 "yacc_sql.tab.c"
    break;

  case 79: /* where: %empty  */
#line 477 "yacc_sql.y"
                { (yyval.list) = list_create(sizeof(Condition), MAX_NUM); }
#line 1817 "yacc_sql.tab.c"
    break;

  case 80: /* where: WHERE condition_list  */
#line 478 "yacc_sql.y"
                           {	
		(yyval.list) = (yyvsp[0].list);  
	}
#line 1825 "yacc_sql.tab.c"
    break;

  case 81: /* condition_list: condition  */
#line 483 "yacc_sql.y"
                  {
		(yyval.list) = list_create(sizeof(Condition), MAX_NUM);
		list_prepend((yyval.list), &(yyvsp[0].condition));
	}
#line 1834 "yacc_sql.tab.c"
    break;

  case 82: /* condition_list: condition AND condition_list  */
#line 487 "yacc_sql.y"
                                   {
		(yyval.list) = (yyvsp[0].list);
		list_prepend((yyval.list), &(yyvsp[-2].condition));
	}
#line 1843 "yacc_sql.tab.c"
    break;

  case 83: /* condition: expr comOp expr  */
#line 493 "yacc_sql.y"
                        {
		condition_init(&(yyval.condition), (yyvsp[-1].comp_op), &(yyvsp[-2].expr), &(yyvsp[0].expr));
	}
#line 1851 "yacc_sql.tab.c"
    break;

  case 84: /* condition: expr IS NULL_VALUE  */
#line 496 "yacc_sql.y"
                             {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), IS_NULL, &(yyvsp[-2].expr), &null_expr);
	}
#line 1866 "yacc_sql.tab.c"
    break;

  case 85: /* condition: expr IS NOT NULL_VALUE  */
#line 506 "yacc_sql.y"
                                 {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&(yyval.condition), IS_NOT_NULL, &(yyvsp[-3].expr), &null_expr);
	}
#line 1881 "yacc_sql.tab.c"
    break;

  case 86: /* comOp: EQ  */
#line 519 "yacc_sql.y"
             { (yyval.comp_op) = EQUAL_TO; }
#line 1887 "yacc_sql.tab.c"
    break;

  case 87: /* comOp: LT  */
#line 520 "yacc_sql.y"
         { (yyval.comp_op) = LESS_THAN; }
#line 1893 "yacc_sql.tab.c"
    break;

  case 88: /* comOp: GT  */
#line 521 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_THAN; }
#line 1899 "yacc_sql.tab.c"
    break;

  case 89: /* comOp: LE  */
#line 522 "yacc_sql.y"
         { (yyval.comp_op) = LESS_EQUAL; }
#line 1905 "yacc_sql.tab.c"
    break;

  case 90: /* comOp: GE  */
#line 523 "yacc_sql.y"
         { (yyval.comp_op) = GREAT_EQUAL; }
#line 1911 "yacc_sql.tab.c"
    break;

  case 91: /* comOp: NE  */
#line 524 "yacc_sql.y"
         { (yyval.comp_op) = NOT_EQUAL; }
#line 1917 "yacc_sql.tab.c"
    break;

  case 92: /* comOp: LIKE  */
#line 525 "yacc_sql.y"
               { (yyval.comp_op) = OP_LIKE; }
#line 1923 "yacc_sql.tab.c"
    break;

  case 93: /* comOp: NOT LIKE  */
#line 526 "yacc_sql.y"
                   { (yyval.comp_op) = OP_NOT_LIKE; }
#line 1929 "yacc_sql.tab.c"
    break;

  case 94: /* expr: ID  */
#line 530 "yacc_sql.y"
           {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, NULL, (yyvsp[0].string));
	}
#line 1938 "yacc_sql.tab.c"
    break;

  case 95: /* expr: ID DOT ID  */
#line 534 "yacc_sql.y"
                    {
		(yyval.expr).type = EXPR_ATTR;
		relation_attr_init(&(yyval.expr).value.attr, (yyvsp[-2].string), (yyvsp[0].string));
	}
#line 1947 "yacc_sql.tab.c"
    break;

  case 96: /* expr: value  */
#line 538 "yacc_sql.y"
                {
		(yyval.expr).type = EXPR_VALUE;
		(yyval.expr).value.value = (yyvsp[0].value);
	}
#line 1956 "yacc_sql.tab.c"
    break;

  case 97: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 545 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 1965 "yacc_sql.tab.c"
    break;


#line 1969 "yacc_sql.tab.c"

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
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
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

#line 550 "yacc_sql.y"

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
