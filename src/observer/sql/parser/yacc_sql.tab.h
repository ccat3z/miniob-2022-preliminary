/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "yacc_sql.y"

#include "sql/parser/parse_defs.h"

#line 53 "yacc_sql.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    CREATE = 259,                  /* CREATE  */
    DROP = 260,                    /* DROP  */
    TABLE = 261,                   /* TABLE  */
    TABLES = 262,                  /* TABLES  */
    NULL_VALUE = 263,              /* NULL_VALUE  */
    NULLABLE = 264,                /* NULLABLE  */
    INDEX = 265,                   /* INDEX  */
    SELECT = 266,                  /* SELECT  */
    DESC = 267,                    /* DESC  */
    SHOW = 268,                    /* SHOW  */
    SYNC = 269,                    /* SYNC  */
    INSERT = 270,                  /* INSERT  */
    DELETE = 271,                  /* DELETE  */
    UPDATE = 272,                  /* UPDATE  */
    LBRACE = 273,                  /* LBRACE  */
    RBRACE = 274,                  /* RBRACE  */
    COMMA = 275,                   /* COMMA  */
    TRX_BEGIN = 276,               /* TRX_BEGIN  */
    TRX_COMMIT = 277,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 278,            /* TRX_ROLLBACK  */
    INT_T = 279,                   /* INT_T  */
    STRING_T = 280,                /* STRING_T  */
    DATE_T = 281,                  /* DATE_T  */
    FLOAT_T = 282,                 /* FLOAT_T  */
    TEXT_T = 283,                  /* TEXT_T  */
    HELP = 284,                    /* HELP  */
    EXIT = 285,                    /* EXIT  */
    DOT = 286,                     /* DOT  */
    INTO = 287,                    /* INTO  */
    VALUES = 288,                  /* VALUES  */
    FROM = 289,                    /* FROM  */
    WHERE = 290,                   /* WHERE  */
    IS = 291,                      /* IS  */
    AND = 292,                     /* AND  */
    SET = 293,                     /* SET  */
    ON = 294,                      /* ON  */
    LOAD = 295,                    /* LOAD  */
    DATA = 296,                    /* DATA  */
    INFILE = 297,                  /* INFILE  */
    EQ = 298,                      /* EQ  */
    LT = 299,                      /* LT  */
    GT = 300,                      /* GT  */
    LE = 301,                      /* LE  */
    GE = 302,                      /* GE  */
    NE = 303,                      /* NE  */
    NOT = 304,                     /* NOT  */
    LIKE = 305,                    /* LIKE  */
    UNIQUE = 306,                  /* UNIQUE  */
    AS = 307,                      /* AS  */
    ADD = 308,                     /* ADD  */
    MINUS = 309,                   /* MINUS  */
    DIV = 310,                     /* DIV  */
    INNER = 311,                   /* INNER  */
    JOIN = 312,                    /* JOIN  */
    GROUP = 313,                   /* GROUP  */
    ORDER = 314,                   /* ORDER  */
    BY = 315,                      /* BY  */
    HAVING = 316,                  /* HAVING  */
    ASC = 317,                     /* ASC  */
    IN = 318,                      /* IN  */
    OR = 319,                      /* OR  */
    EXISTS = 320,                  /* EXISTS  */
    NUMBER = 321,                  /* NUMBER  */
    FLOAT = 322,                   /* FLOAT  */
    ID = 323,                      /* ID  */
    PATH = 324,                    /* PATH  */
    SSS = 325,                     /* SSS  */
    STAR = 326,                    /* STAR  */
    STRING_V = 327                 /* STRING_V  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 121 "yacc_sql.y"

  Condition condition;
  UnionExpr expr;
  Value value;
  char *string;
  int number;
  float floats;
	char *position;
  bool boolean;
  List *list;
  CompOp comp_op;
  AttrExpr attr;
  Selects select;

#line 157 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void *scanner);


#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
