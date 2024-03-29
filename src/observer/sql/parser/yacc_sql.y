%code requires {
#include "sql/parser/parse_defs.h"
}

%{

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

%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
		NULL_VALUE
		NULLABLE
        INDEX
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
		DATE_T
        FLOAT_T
		TEXT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
		IS
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
		NOT
		LIKE
		UNIQUE
		AS
		ADD
		MINUS
		DIV
		INNER
		JOIN
		GROUP
		ORDER
		BY
		HAVING
		ASC
		IN
		OR
		EXISTS

%union {
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
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
//非终结符

%type <number> type;
%type <condition> condition;
%type <value> value;
%type <value> pos_value;
%type <number> number;
%type <boolean> create_index_unique;
%type <list> create_index_attr_list;
%type <list> value_list;
%type <list> value_lists;
%type <list> where;
%type <list> condition_list;
%type <list> attr_list;
%type <list> expr_list;
%type <attr> select_attr;
%type <comp_op> comOp;
%type <expr> expr;
%type <list> update_set_list;
%type <boolean> attr_def_nullable;
%type <select> select_stmt;
%type <list> order_by;
%type <list> order_by_list;
%type <boolean> order_direct;
%type <list> group_by;
%type <list> having;
%type <string> rel_as;

%left ADD MINUS
%left STAR DIV
%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| desc_table
	| create_index	
	| drop_index
	| show_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

show_index:
    SHOW INDEX FROM ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_INDEX;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $4);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE create_index_unique INDEX ID ON ID LBRACE create_index_attr_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6, (const char **)$8->values, $8->len, $2);
			list_free($8);
		}
    ;

create_index_unique:
	{ $$ = false; }
	| UNIQUE { $$ = true; }
	;

create_index_attr_list:
	ID {
		$$ = list_create(sizeof(char *), MAX_NUM);
		list_prepend($$, &$1);
	}
	| ID COMMA create_index_attr_list {
		$$ = $3;
		list_prepend($$, &$1);
	}
	;

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE attr_def_nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, $6);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
    |ID_get type attr_def_nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 4, $3);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
		}
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | DATE_T { $$=DATE; }
       | FLOAT_T { $$=FLOATS; }
	   | TEXT_T { $$=TEXT; }
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;
attr_def_nullable:
	{ $$ = false; }
	| NOT NULL_VALUE { $$ = false; }
	| NULLABLE { $$ = true; }
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES value_lists SEMICOLON 
	{
	  	CONTEXT->ssql->flag=SCF_INSERT;//"insert";
		inserts_init(&CONTEXT->ssql->sstr.insertion, $3, (Value *)$5->values, $5->len);
		list_free($5);
    }

value_lists:
	LBRACE value_list RBRACE {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		$$ = $2;
	}
	| LBRACE value_list RBRACE COMMA value_lists {
		CONTEXT->ssql->sstr.insertion.tuple_num++;
		$$ = $5;
		list_prepend_list($$, $2);
		list_free($2);
	}
	;

value_list:
    /* empty */
	value {
		$$ = list_create(sizeof(Value), MAX_NUM);
		list_prepend($$, &$1);
	}
    | value COMMA value_list  { 
		$$ = $3;
		list_prepend($$, &$1);
	}
    ;

value:
	pos_value {
		$$ = $1;
	}
    | MINUS NUMBER{	
  		value_init_integer(&$$, -$2);
	}
    | MINUS FLOAT{
  		value_init_float(&$$, -$2);
	}
    ;
    ;
pos_value:
	NULL_VALUE {
		value_init_null(&$$);
	}
    | NUMBER {	
  		value_init_integer(&$$, $1);
	}
    | FLOAT {
  		value_init_float(&$$, $1);
		}
    | SSS {
		$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&$$, $1);
	}
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					(Condition *) $4->values, $4->len);
			list_free($4);
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET update_set_list where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			updates_init(&CONTEXT->ssql->sstr.update, $2, (KeyValue *) $4->values, $4->len, 
					(Condition *) $5->values, $5->len);
			list_free($4);
			list_free($5);
		}
    ;

update_set_list:
	ID EQ expr
	{
		$$ = list_create(sizeof(KeyValue), MAX_NUM);
		KeyValue kv;
		kv.name = $1;
		kv.value = $3;
		list_prepend($$, &kv);
	}
	| ID EQ expr COMMA update_set_list
	{
		$$ = $5;
		KeyValue kv;
		kv.name = $1;
		kv.value = $3;
		list_prepend($$, &kv);
	}
	;

select:
	select_stmt SEMICOLON
	{
		CONTEXT->ssql->sstr.selection = $1;
		CONTEXT->ssql->flag=SCF_SELECT;//"select";
	}
	;

select_stmt:				/*  select 语句的语法解析树*/
    SELECT attr_list
	{
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, (AttrExpr *) $2->values, $2->len);
		list_free($2);

		$$ = CONTEXT->ssql->sstr.selection;
  		CONTEXT->ssql->sstr.selection.relation_join_num = 0;
  		CONTEXT->ssql->sstr.selection.relation_num = 0;     
  		CONTEXT->ssql->sstr.selection.join_condition_num = 0;
	}
    | SELECT attr_list FROM ID rel_as rel_list where group_by having order_by
		{
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $4, $5);

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) $7->values, $7->len);
			list_free($7);

			selects_append_attribute(&CONTEXT->ssql->sstr.selection, (AttrExpr *) $2->values, $2->len);
			list_free($2);

			selects_append_havings(&CONTEXT->ssql->sstr.selection, (Condition *) $9->values, $9->len);
			list_free($9);

			selects_append_groups(&CONTEXT->ssql->sstr.selection, (UnionExpr *) $8->values, $8->len);
			list_free($8);

			selects_append_orders(&CONTEXT->ssql->sstr.selection, (OrderExpr *) $10->values, $10->len);
			list_free($10);

			$$ = CONTEXT->ssql->sstr.selection;
  			CONTEXT->ssql->sstr.selection.relation_join_num = 0;
  			CONTEXT->ssql->sstr.selection.relation_num = 0;     
  			CONTEXT->ssql->sstr.selection.join_condition_num = 0;
	}
	;

select_attr:
	expr
	{
		$$.expr = $1;
		$$.name = NULL;
	}
	| expr ID
	{
		$$.expr = $1;
		$$.name = $2;
	}
	| expr AS ID
	{
		$$.expr = $1;
		$$.name = $3;
	}
    ;

attr_list:
	select_attr
	{
		$$ = list_create(sizeof(AttrExpr), MAX_NUM);
		list_prepend($$, &$1);
	}
	| select_attr COMMA attr_list
	{
		$$ = $3;
		list_prepend($$, &$1);
	}
  	;

rel_list:
    /* empty */
    | COMMA ID rel_as rel_list {	
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $2, $3);
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, $2,NULL, 0);
	}
	| INNER JOIN ID rel_as ON condition_list rel_list{
		selects_append_relation(&CONTEXT->ssql->sstr.selection, $3, $4);
		selects_append_join_conditions(&CONTEXT->ssql->sstr.selection, (Condition *) $6->values, $6->len);
		selects_append_inner_join(&CONTEXT->ssql->sstr.selection, $3,(Condition *) $6->values, $6->len);
		list_free($6);
	};

rel_as:
	{ $$ = NULL; }
	| ID { $$ = $1; }
	| AS ID { $$ = $2; }
	;

where:
    /* empty */ { $$ = list_create(sizeof(Condition), MAX_NUM); } 
    | WHERE condition_list {	
		$$ = $2;
	}
    ;
group_by:
	{ $$ = list_create(sizeof(UnionExpr), 0); }
	| GROUP BY expr_list
	{
	  $$ = $3;
	}
	;
having:
	{ $$ = list_create(sizeof(Condition), 0); }
	| HAVING condition_list
	{
	  $$ = $2;
	}
	;
order_by:
	{ $$ = list_create(sizeof(OrderExpr), 0); }
	| ORDER BY order_by_list
	{
	  $$ = $3;
	}
	;
order_by_list:
	expr order_direct {
		$$ = list_create(sizeof(OrderExpr), MAX_NUM);
		OrderExpr order;
		order.expr = $1;
		order.asc = $2;
		list_prepend($$, &order);
	}
	| expr order_direct COMMA order_by_list {
		$$ = $4;
		OrderExpr order;
		order.expr = $1;
		order.asc = $2;
		list_prepend($$, &order);
	}

order_direct:
	{ $$ = true; }
	| ASC { $$ = true; }
	| DESC { $$ = false; }
	;

condition_list:
	condition {
		$$ = list_create(sizeof(Condition), MAX_NUM);
		$1.is_and = true;
		list_prepend($$, &$1);
	}
    | condition AND condition_list {
		$$ = $3;
		$1.is_and = true;
		list_prepend($$, &$1);
	}
    | condition OR condition_list {
		$$ = $3;
		$1.is_and = false;
		list_prepend($$, &$1);
	}
    ;
condition:
	expr comOp expr {
		condition_init(&$$, $2, &$1, &$3);
	}
	| expr IS NULL_VALUE {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&$$, IS_NULL, &$1, &null_expr);
	}
	| expr IS NOT NULL_VALUE {
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&$$, IS_NOT_NULL, &$1, &null_expr);
	}
	| EXISTS expr
	{
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&$$, OP_EXISTS, &null_expr, &$2);
	}
	| NOT EXISTS expr
	{
		Value null;
		value_init_null(&null);

		UnionExpr null_expr;
		null_expr.type = EXPR_VALUE;
		null_expr.value.value = null;

		condition_init(&$$, OP_NOT_EXISTS, &null_expr, &$3);
	}
	;

comOp:
  	  EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
	| LIKE { $$ = OP_LIKE; }
	| NOT LIKE { $$ = OP_NOT_LIKE; }
	| IN { $$ = OP_IN; }
	| NOT IN { $$ = OP_NOT_IN; }
    ;

expr:
    STAR
	{
		$$.type = EXPR_ATTR;
		relation_attr_init(&$$.value.attr, NULL, "*");
	}
	| ID {
		$$.type = EXPR_ATTR;
		relation_attr_init(&$$.value.attr, NULL, $1);
	}
	| ID DOT STAR
	{
		$$.type = EXPR_ATTR;
		relation_attr_init(&$$.value.attr, $1, "*");
	}
	| ID DOT ID {
		$$.type = EXPR_ATTR;
		relation_attr_init(&$$.value.attr, $1, $3);
	}
	| ID LBRACE expr_list RBRACE
	{
		$$.type = EXPR_FUNC;
		func_init(&$$.value.func, $1, (UnionExpr *) $3->values, $3->len);
	}
	| pos_value {
		$$.type = EXPR_VALUE;
		$$.value.value = $1;
	}
	| MINUS expr {
		$$.type = EXPR_FUNC;
		func_init_1(&$$.value.func, "neg", &$2);
	}
	| expr ADD expr {
		$$.type = EXPR_FUNC;
		func_init_2(&$$.value.func, "+", &$1, &$3);
	}
	| expr MINUS expr {
		$$.type = EXPR_FUNC;
		func_init_2(&$$.value.func, "-", &$1, &$3);
	}
	| expr STAR expr {
		$$.type = EXPR_FUNC;
		func_init_2(&$$.value.func, "*", &$1, &$3);
	}
	| expr DIV expr {
		$$.type = EXPR_FUNC;
		func_init_2(&$$.value.func, "/", &$1, &$3);
	}
	| LBRACE RBRACE
	{
		$$.type = EXPR_FUNC;
		func_init(&$$.value.func, "tuple", NULL, 0);
	}
	| LBRACE expr_list RBRACE
	{
		$$.type = EXPR_FUNC;
		func_init(&$$.value.func, "tuple", (UnionExpr *) $2->values, $2->len);
	}
	| LBRACE select_stmt RBRACE {
		expr_init_selects(&$$, &$2);
	}
	;

expr_list:
	expr
	{
		$$ = list_create(sizeof(UnionExpr), MAX_NUM);
		list_prepend($$, &$1);
	}
	| expr COMMA expr_list
	{
		$$ = $3;
		list_prepend($$, &$1);
	}
	;

load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
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
