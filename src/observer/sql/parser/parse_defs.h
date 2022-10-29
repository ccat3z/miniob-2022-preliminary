/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Meiyi
//

#ifndef __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
#define __OBSERVER_SQL_PARSER_PARSE_DEFS_H__

#include <stdbool.h>
#include <stddef.h>
#ifdef __cplusplus
#include <cstring>
#include <cstdlib>
#include <string>
#endif

#ifndef __cplusplus
#define mutable
#endif

#define MAX_NUM 20
#define MAX_REL_NAME 20
#define MAX_ATTR_NAME 20
#define MAX_ERROR_MESSAGE 20
#define MAX_DATA 50

//属性结构体
typedef struct {
  char *relation_name;   // relation name (may be NULL) 表名
  char *attribute_name;  // attribute name              属性名
} RelAttr;

typedef enum {
  EQUAL_TO,     //"="     0
  LESS_EQUAL,   //"<="    1
  NOT_EQUAL,    //"<>"    2
  LESS_THAN,    //"<"     3
  GREAT_EQUAL,  //">="    4
  GREAT_THAN,   //">"     5
  OP_LIKE,      //"like"  6
  OP_NOT_LIKE,  //"like"  6
  IS_NULL,
  IS_NOT_NULL,
  OP_IN,
  OP_NOT_IN,
  OP_EXISTS,
  OP_NOT_EXISTS,
  NO_OP
} CompOp;

//属性值类型
typedef enum { UNDEFINED, CHARS, INTS, FLOATS, DATE, TEXT, MIXED, TYPE_NULL } AttrType;

//属性值
typedef struct _Value {
#ifdef __cplusplus
  mutable AttrType type;  // type of value
  mutable void *data;     // value
  mutable bool is_null;

  // Cast value to type, precision may be lost.
  bool try_cast(const AttrType &type) const;

private:
  template <typename T>
  void replace(const T &v) const
  {
    free(data);
    T *new_data = (T *)malloc(sizeof(T));
    *new_data = v;
    data = new_data;
  }

  void replace(const char *v) const
  {
    free(data);
    size_t size = std::strlen(v) + 1;
    data = malloc(size);
    memcpy(data, v, size);
  }

  void replace(const std::string &v) const
  {
    replace(v.c_str());
  }
#else
  AttrType type;  // type of value
  void *data;     // value
  bool is_null;
#endif
} Value;

typedef struct {
  char *values;
  char *data;
  size_t cap;
  size_t size;
  size_t len;
} List;

typedef enum { EXPR_VALUE, EXPR_ATTR, EXPR_FUNC, EXPR_AGG, EXPR_RUNTIME_ATTR, EXPR_SELECT } UnionExprType;

struct _UnionExpr;

typedef struct {
  char *name;
  struct _UnionExpr *args;
  int arg_num;
} FuncExpr;

#ifdef __cplusplus
class Field;
class SelectStmt;
#endif

struct _Selects;
typedef struct _UnionExpr {
  union {
    Value value;
    RelAttr attr;
    FuncExpr func;
    struct _Selects *select;
  } value;

#ifdef __cplusplus
  union {
    mutable Field *field;
    mutable SelectStmt *select;
  } hack;  // HACK: Filled by exectuor
#else
  void *field;
#endif

  mutable UnionExprType type;

#ifdef __cplusplus
  std::string to_string();
#endif
} UnionExpr;

typedef struct _Condition {
#ifdef __cplusplus
  // TRUE if left-hand side is an attribute
  // 1时，操作符左边是属性名，0时，是属性值
  const int left_is_attr() const
  {
    return left_expr.type == EXPR_ATTR ? 1 : 0;
  }

  // left-hand side value if left_is_attr = FALSE
  const Value &left_value() const
  {
    return left_expr.value.value;
  }

  // left-hand side attribute
  const RelAttr &left_attr() const
  {
    return left_expr.value.attr;
  }

  // TRUE if right-hand side is an attribute
  // 1时，操作符右边是属性名，0时，是属性值
  const int right_is_attr() const
  {
    return right_expr.type == EXPR_ATTR ? 1 : 0;
  }

  // right-hand side attribute if right_is_attr = TRUE 右边的属性
  const RelAttr &right_attr() const
  {
    return right_expr.value.attr;
  }

  // right-hand side value if right_is_attr = FALSE
  const Value &right_value() const
  {
    return right_expr.value.value;
  }
#endif

  UnionExpr left_expr;
  CompOp comp;  // comparison operator
  UnionExpr right_expr;
  bool is_and;
} Condition;

typedef struct {
  UnionExpr expr;
  char *name;
} AttrExpr;

typedef struct _RelJoin {
  bool inner_join;
  char *relation_name;
  size_t condition_num;
  Condition conditions[MAX_NUM];
} RelJoin;

typedef struct {
  UnionExpr expr;
  bool asc;
} OrderExpr;

// struct of select
typedef struct _Selects {
  size_t attr_num;                // Length of attrs in Select clause
  AttrExpr attributes[MAX_NUM];   // attrs in Select clause
  size_t relation_join_num;       // length of join relations
  RelJoin relation_join_list[MAX_NUM];  // relations in From clause includes join
  Condition *join_conditions;           // condition in From clause for join
  size_t join_condition_num;
  size_t relation_num;            // Length of relations in Fro clause
  char *relations[MAX_NUM];       // relations in From clause
  char *rel_alias[MAX_NUM];       // relations in From clause
  size_t condition_num;           // Length of conditions in Where clause
  Condition *conditions;          // conditions in Where clause
  size_t group_num;
  UnionExpr *groups;
  size_t order_num;
  OrderExpr *orders;
  size_t having_num;
  Condition *havings;
} Selects;

// struct of insert
typedef struct {
  char *relation_name;    // Relation to insert into
  size_t value_num;       // Length of values
  size_t tuple_num;       // Length of tuples
  Value values[MAX_NUM];  // values to insert
} Inserts;

// struct of delete
typedef struct {
  char *relation_name;            // Relation to delete from
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
} Deletes;

// struct of update
typedef struct {
  char *name;   // Attribute name
  UnionExpr value;  // Value
} KeyValue;

typedef struct {
  char *relation_name;            // Relation to update
  KeyValue *kvs;                  // Attribute to update
  int kv_num;
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
} Updates;

typedef struct {
  char *name;     // Attribute name
  AttrType type;  // Type of attribute
  size_t length;  // Length of attribute
  bool nullable;
} AttrInfo;

// struct of craete_table
typedef struct {
  char *relation_name;           // Relation name
  size_t attribute_count;        // Length of attribute
  AttrInfo attributes[MAX_NUM];  // attributes
} CreateTable;

// struct of drop_table
typedef struct {
  char *relation_name;  // Relation name
} DropTable;

// struct of create_index
typedef struct {
  char *index_name;      // Index name
  char *relation_name;   // Relation name
  char **attribute_name;  // Attribute name
  bool unique;
  int attribute_num;
} CreateIndex;

// struct of  drop_index
typedef struct {
  const char *index_name;  // Index name
} DropIndex;

typedef struct {
  const char *relation_name;
} DescTable;

typedef struct {
  const char *relation_name;
  const char *file_name;
} LoadData;

union Queries {
  Selects selection;
  Inserts insertion;
  Deletes deletion;
  Updates update;
  CreateTable create_table;
  DropTable drop_table;
  CreateIndex create_index;
  DropIndex drop_index;
  DescTable desc_table;
  LoadData load_data;
  char *errors;
};

// 修改yacc中相关数字编码为宏定义
enum SqlCommandFlag {
  SCF_ERROR = 0,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SHOW_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT
};
// struct of flag and sql_struct
typedef struct Query {
  enum SqlCommandFlag flag;
  union Queries sstr;
} Query;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name);
void relation_attr_destroy(RelAttr *relation_attr);

void value_init_null(Value *value);
void value_init_integer(Value *value, int v);
void value_init_float(Value *value, float v);
void value_init_string(Value *value, const char *v);
void value_destroy(Value *value);

void expr_init_selects(UnionExpr *expr, Selects *select);
void expr_destroy(UnionExpr *expr);
void func_init_1(FuncExpr *func, const char *name, UnionExpr *arg);
void func_init_2(FuncExpr *func, const char *name, UnionExpr *arg1, UnionExpr *arg2);
void func_init(FuncExpr *func, const char *name, UnionExpr *args, size_t length);
void func_destroy(FuncExpr *func);

void condition_init(Condition *condition, CompOp comp, UnionExpr *left, UnionExpr *right);
void condition_destroy(Condition *condition);

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, size_t length, bool nullable);
void attr_info_destroy(AttrInfo *attr_info);

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, AttrExpr *rel_attr, size_t attr_len);
void selects_append_relation(Selects *selects, const char *relation_name, const char *alias);
void selects_append_conditions(Selects *selects, Condition conditions[], size_t condition_num);
void selects_append_havings(Selects *selects, Condition conditions[], size_t size);
void selects_append_orders(Selects *selects, OrderExpr exprs[], size_t size);
void selects_append_groups(Selects *selects, UnionExpr exprs[], size_t size);
void selects_destroy(Selects *selects);

void inserts_init(Inserts *inserts, const char *relation_name, Value values[], size_t value_num);
void inserts_destroy(Inserts *inserts);

void deletes_init_relation(Deletes *deletes, const char *relation_name);
void deletes_set_conditions(Deletes *deletes, Condition conditions[], size_t condition_num);
void deletes_destroy(Deletes *deletes);

void updates_init(Updates *updates, const char *relation_name, KeyValue *kvs, int kv_num, Condition conditions[],
    size_t condition_num);
void updates_destroy(Updates *updates);

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info);
void create_table_init_name(CreateTable *create_table, const char *relation_name);
void create_table_destroy(CreateTable *create_table);

void drop_table_init(DropTable *drop_table, const char *relation_name);
void drop_table_destroy(DropTable *drop_table);

void create_index_init(CreateIndex *create_index, const char *index_name, const char *relation_name,
    const char **attr_name, int attr_num, bool unique);
void create_index_destroy(CreateIndex *create_index);

void drop_index_init(DropIndex *drop_index, const char *index_name);
void drop_index_destroy(DropIndex *drop_index);

void desc_table_init(DescTable *desc_table, const char *relation_name);
void desc_table_destroy(DescTable *desc_table);

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name);
void load_data_destroy(LoadData *load_data);

void query_init(Query *query);
Query *query_create();  // create and init
void query_reset(Query *query);
void query_destroy(Query *query);  // reset and delete

List *list_create(size_t size, size_t max);
void list_prepend(List *list, void *value);
void list_prepend_list(List *list, List *append);
void list_free(List *list);
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
