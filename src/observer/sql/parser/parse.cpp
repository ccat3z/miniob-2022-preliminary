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

#include <algorithm>
#include <cstddef>
#include <mutex>
#include <common/time/datetime.h>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <cmath>
#include <exception>
#include "sql/parser/parse.h"
#include "rc.h"
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"
#include "storage/default/large_block_pool.h"

RC parse(char *st, Query *sqln);

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

bool Value::try_cast(const AttrType &type) const
{
  if (this->type == type)
    return true;

  switch (this->type) {
    case INTS: {
      const auto &val = *(int *)data;
      switch (type) {
        case FLOATS:
          replace(float(val));
          break;
        case CHARS:
          replace(std::to_string(val));
          break;
        default:
          return false;
      }
    } break;
    case FLOATS: {
      const auto &val = *(float *)data;
      switch (type) {
        case INTS:
          replace(int(std::round(val)));
          break;
        case CHARS: {
          char str[10];
          std::snprintf(str, 10, "%.2g", val);
          replace((const char *)str);
          break;
        }
        default:
          return false;
      }
    } break;
    case CHARS: {
      const auto val = (char *)data;
      switch (type) {
        case INTS:
          replace(std::atoi(val));
          break;
        case FLOATS:
          replace((float)std::atof(val));
          break;
        case DATE: {
          common::Date date;
          if (!date.parse((char *)this->data))
            return false;

          replace(date.julian());
        } break;
        case TEXT: {
          auto &lbp = LargeBlockPool::instance();
          auto id = lbp->find_next_free();
          if (lbp->set(id, val, strlen(val) + 1) != RC::SUCCESS)
            return false;
          replace(id);
        } break;
        default:
          return false;
      }
    } break;
    case TYPE_NULL:
      // NULL can be anything
      break;
    default:
      return false;
  }

  this->type = type;
  return true;
}

void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name)
{
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  relation_attr->attribute_name = strdup(attribute_name);
}

void relation_attr_destroy(RelAttr *relation_attr)
{
  free(relation_attr->relation_name);
  free(relation_attr->attribute_name);
  relation_attr->relation_name = nullptr;
  relation_attr->attribute_name = nullptr;
}

void value_init_null(Value *value)
{
  value->type = TYPE_NULL;
  value->data = malloc(4);
  memset(value->data, 0, 4);
  value->is_null = true;
}

void value_init_integer(Value *value, int v)
{
  value->type = INTS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
  value->is_null = false;
}
void value_init_float(Value *value, float v)
{
  value->type = FLOATS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
  value->is_null = false;
}
void value_init_string(Value *value, const char *v)
{
  value->type = CHARS;
  value->data = strdup(v);
  value->is_null = false;
}

void value_destroy(Value *value)
{
  value->type = UNDEFINED;
  free(value->data);
  value->data = nullptr;
}

void expr_destroy(UnionExpr *expr)
{
  switch (expr->type) {
    case EXPR_ATTR:
      relation_attr_destroy(&expr->value.attr);
      break;
    case EXPR_VALUE:
      value_destroy(&expr->value.value);
      break;
    default:
      throw std::logic_error("Unreachable code");
  }
}

void condition_init(Condition *condition, CompOp comp, UnionExpr *left, UnionExpr *right)
{
  condition->left_expr = *left;
  condition->right_expr = *right;
  condition->comp = comp;
}

void condition_destroy(Condition *condition)
{
  expr_destroy(&condition->left_expr);
  expr_destroy(&condition->right_expr);
}

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, size_t length, bool nullable)
{
  attr_info->name = strdup(name);
  attr_info->type = type;
  attr_info->length = length;
  attr_info->nullable = nullable;
}
void attr_info_destroy(AttrInfo *attr_info)
{
  free(attr_info->name);
  attr_info->name = nullptr;
}

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, RelAttr *rel_attr)
{
  selects->attributes[selects->attr_num++] = *rel_attr;
}
void selects_append_relation(Selects *selects, const char *relation_name)
{
  selects->relations[selects->relation_num++] = strdup(relation_name);
}
void selects_append_inner_join(
    Selects *selects, const char *relation_name, Condition conditions[], size_t condition_num)
{  // 将所有的conditions 变成inner—join-lists[i]
  assert(selects->relation_join_num <= sizeof(selects->relation_join_list) / sizeof(selects->relation_join_list[0]));
  selects->relation_join_num++;
  for (size_t i = 0; i < condition_num; i++) {
    selects->relation_join_list[selects->relation_join_num].conditions[i] = conditions[i];
  }
  selects->relation_join_list[selects->relation_join_num].inner_join = true;
  selects->relation_join_list[selects->relation_join_num].relation_name = strdup(relation_name);
  selects->relation_join_list[selects->relation_join_num].condition_num = condition_num;
}
void selects_append_join_conditions(Selects *selects, Condition conditions[], size_t condition_num)
{
  assert(condition_num <= sizeof(selects->join_conditions) / sizeof(selects->join_conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->join_conditions[i + selects->join_condition_num] = conditions[i];
  }
  selects->join_condition_num = selects->join_condition_num + condition_num;
}
void selects_append_conditions(Selects *selects, Condition conditions[], size_t condition_num)
{
  assert(condition_num <= sizeof(selects->conditions) / sizeof(selects->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->conditions[i] = conditions[i];
  }
  selects->condition_num = condition_num;
}

void selects_destroy(Selects *selects)
{
  for (size_t i = 0; i < selects->attr_num; i++) {
    relation_attr_destroy(&selects->attributes[i]);
  }
  selects->attr_num = 0;

  for (size_t i = 0; i < selects->relation_num; i++) {
    free(selects->relations[i]);
    selects->relations[i] = NULL;
  }
  selects->relation_num = 0;
  for (size_t i = 0; i < selects->join_condition_num; i++) {
    condition_destroy(&selects->join_conditions[i]);
  }
  selects->join_condition_num = 0;
  for (size_t i = 0; i < selects->condition_num; i++) {
    condition_destroy(&selects->conditions[i]);
  }
  selects->condition_num = 0;
}

void inserts_init(Inserts *inserts, const char *relation_name, Value values[], size_t value_num)
{
  assert(value_num <= sizeof(inserts->values) / sizeof(inserts->values[0]));

  inserts->relation_name = strdup(relation_name);
  for (size_t i = 0; i < value_num; i++) {
    inserts->values[i] = values[i];
  }
  inserts->value_num = value_num;
}
void inserts_destroy(Inserts *inserts)
{
  free(inserts->relation_name);
  inserts->relation_name = nullptr;

  for (size_t i = 0; i < inserts->value_num; i++) {
    value_destroy(&inserts->values[i]);
  }
  inserts->value_num = 0;
}

void deletes_init_relation(Deletes *deletes, const char *relation_name)
{
  deletes->relation_name = strdup(relation_name);
}

void deletes_set_conditions(Deletes *deletes, Condition conditions[], size_t condition_num)
{
  assert(condition_num <= sizeof(deletes->conditions) / sizeof(deletes->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    deletes->conditions[i] = conditions[i];
  }
  deletes->condition_num = condition_num;
}
void deletes_destroy(Deletes *deletes)
{
  for (size_t i = 0; i < deletes->condition_num; i++) {
    condition_destroy(&deletes->conditions[i]);
  }
  deletes->condition_num = 0;
  free(deletes->relation_name);
  deletes->relation_name = nullptr;
}

void updates_init(Updates *updates, const char *relation_name, KeyValue *kvs, int kv_num, Condition conditions[],
    size_t condition_num)
{
  updates->relation_name = strdup(relation_name);
  updates->kvs = (KeyValue *)malloc(sizeof(KeyValue) * kv_num);
  memcpy(updates->kvs, kvs, sizeof(KeyValue) * kv_num);
  updates->kv_num = kv_num;

  assert(condition_num <= sizeof(updates->conditions) / sizeof(updates->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    updates->conditions[i] = conditions[i];
  }
  updates->condition_num = condition_num;
}

void updates_destroy(Updates *updates)
{
  free(updates->relation_name);
  updates->relation_name = nullptr;

  for (int i = 0; i < updates->kv_num; i++) {
    free(updates->kvs[i].name);
    value_destroy(&updates->kvs[i].value);
  }
  free(updates->kvs);
  updates->kvs = nullptr;

  for (size_t i = 0; i < updates->condition_num; i++) {
    condition_destroy(&updates->conditions[i]);
  }
  updates->condition_num = 0;
}

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info)
{
  create_table->attributes[create_table->attribute_count++] = *attr_info;
}

void create_table_init_name(CreateTable *create_table, const char *relation_name)
{
  create_table->relation_name = strdup(relation_name);
}

void create_table_destroy(CreateTable *create_table)
{
  for (size_t i = 0; i < create_table->attribute_count; i++) {
    attr_info_destroy(&create_table->attributes[i]);
  }
  create_table->attribute_count = 0;
  free(create_table->relation_name);
  create_table->relation_name = nullptr;
}

void drop_table_init(DropTable *drop_table, const char *relation_name)
{
  drop_table->relation_name = strdup(relation_name);
}

void drop_table_destroy(DropTable *drop_table)
{
  free(drop_table->relation_name);
  drop_table->relation_name = nullptr;
}

void create_index_init(CreateIndex *create_index, const char *index_name, const char *relation_name,
    const char **attr_name, int attr_num, bool unique)
{
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
  create_index->attribute_name = (char **)malloc(sizeof(char *) * attr_num);
  for (int i = attr_num - 1; i >= 0; i--) {
    create_index->attribute_name[i] = strdup(attr_name[i]);
  }
  create_index->attribute_num = attr_num;
  create_index->unique = unique;
}

void create_index_destroy(CreateIndex *create_index)
{
  free(create_index->index_name);
  free(create_index->relation_name);
  for (int i = 0; i < create_index->attribute_num; i++) {
    free(create_index->attribute_name[i]);
  }

  create_index->index_name = nullptr;
  create_index->relation_name = nullptr;
  create_index->attribute_name = nullptr;
}

void drop_index_init(DropIndex *drop_index, const char *index_name)
{
  drop_index->index_name = strdup(index_name);
}

void drop_index_destroy(DropIndex *drop_index)
{
  free((char *)drop_index->index_name);
  drop_index->index_name = nullptr;
}

void desc_table_init(DescTable *desc_table, const char *relation_name)
{
  desc_table->relation_name = strdup(relation_name);
}

void desc_table_destroy(DescTable *desc_table)
{
  free((char *)desc_table->relation_name);
  desc_table->relation_name = nullptr;
}

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name)
{
  load_data->relation_name = strdup(relation_name);

  if (file_name[0] == '\'' || file_name[0] == '\"') {
    file_name++;
  }
  char *dup_file_name = strdup(file_name);
  int len = strlen(dup_file_name);
  if (dup_file_name[len - 1] == '\'' || dup_file_name[len - 1] == '\"') {
    dup_file_name[len - 1] = 0;
  }
  load_data->file_name = dup_file_name;
}

void load_data_destroy(LoadData *load_data)
{
  free((char *)load_data->relation_name);
  free((char *)load_data->file_name);
  load_data->relation_name = nullptr;
  load_data->file_name = nullptr;
}

void query_init(Query *query)
{
  query->flag = SCF_ERROR;
  memset(&query->sstr, 0, sizeof(query->sstr));
}

Query *query_create()
{
  Query *query = (Query *)malloc(sizeof(Query));
  if (nullptr == query) {
    LOG_ERROR("Failed to alloc memroy for query. size=%ld", sizeof(Query));
    return nullptr;
  }

  query_init(query);
  return query;
}

void query_reset(Query *query)
{
  switch (query->flag) {
    case SCF_SELECT: {
      selects_destroy(&query->sstr.selection);
    } break;
    case SCF_INSERT: {
      inserts_destroy(&query->sstr.insertion);
    } break;
    case SCF_DELETE: {
      deletes_destroy(&query->sstr.deletion);
    } break;
    case SCF_UPDATE: {
      updates_destroy(&query->sstr.update);
    } break;
    case SCF_CREATE_TABLE: {
      create_table_destroy(&query->sstr.create_table);
    } break;
    case SCF_DROP_TABLE: {
      drop_table_destroy(&query->sstr.drop_table);
    } break;
    case SCF_CREATE_INDEX: {
      create_index_destroy(&query->sstr.create_index);
    } break;
    case SCF_DROP_INDEX: {
      drop_index_destroy(&query->sstr.drop_index);
    } break;
    case SCF_SYNC: {

    } break;
    case SCF_SHOW_TABLES:
      break;

    case SCF_SHOW_INDEX:
    case SCF_DESC_TABLE: {
      desc_table_destroy(&query->sstr.desc_table);
    } break;

    case SCF_LOAD_DATA: {
      load_data_destroy(&query->sstr.load_data);
    } break;
    case SCF_CLOG_SYNC:
    case SCF_BEGIN:
    case SCF_COMMIT:
    case SCF_ROLLBACK:
    case SCF_HELP:
    case SCF_EXIT:
    case SCF_ERROR:
      break;
  }
}

void query_destroy(Query *query)
{
  query_reset(query);
  free(query);
}

List *list_create(size_t size, size_t max)
{
  List *l = (List *)malloc(sizeof(List));
  l->data = (char *)malloc(size * max);
  l->size = size;

  l->len = 0;
  l->cap = max;
  l->values = l->data + (l->size * l->cap);
  return l;
}

void list_prepend(List *list, void *value)
{
  if (list->len + 1 > list->cap) {
    throw std::out_of_range("overflow");
  }

  list->values -= list->size;
  list->len++;
  memcpy(list->values, value, list->size);
}

void list_prepend_list(List *list, List *append)
{
  if (list->len + append->len > list->cap) {
    throw std::out_of_range("overflow");
  }

  list->values -= append->size * append->len;
  list->len += append->len;
  memcpy(list->values, append->values, append->size * append->len);
}

void list_free(List *list)
{
  free(list->data);
  free(list);
}
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

////////////////////////////////////////////////////////////////////////////////

extern "C" int sql_parse(const char *st, Query *sqls);

RC parse(const char *st, Query *sqln)
{
  sql_parse(st, sqln);

  if (sqln->flag == SCF_ERROR)
    return SQL_SYNTAX;
  else
    return SUCCESS;
}