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
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/table.h"
#include <cstddef>
#include <cstring>
#include <set>
#include <strings.h>
#include <vector>

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

static void wildcard_fields(Table *table, std::vector<AttrExpr> &attrs)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    AttrExpr attr;
    attr.name = nullptr;
    attr.expr.type = EXPR_ATTR;

    // FIXME: Memory leak
    attr.expr.value.field = new Field(table, table_meta.field(i));

    attrs.emplace_back(attr);
  }
}

RC fill_expr(const Table *default_table, const std::unordered_map<std::string, Table *> &table_map,
    const UnionExpr &expr, bool allow_star)
{
  RC rc = RC::SUCCESS;

  /// Replace attr field
  if (expr.type == EXPR_ATTR) {
    auto &relattr = expr.value.attr;

    if (common::is_blank(relattr.attribute_name)) {
      LOG_ERROR("Attr name is blank");
      return RC::INVALID_ARGUMENT;
    }

    const Table *table = nullptr;
    if (common::is_blank(relattr.relation_name)) {
      if (allow_star && strcmp(relattr.attribute_name, "*") == 0) {
        expr.type = EXPR_RUNTIME_ATTR;
        return RC::SUCCESS;
      }

      std::set<const Table *> tables;
      for (auto &kv : table_map) {
        tables.emplace(kv.second);
      }
      if (tables.size() != 1) {
        // TODO: select a,b from t1,t2; Find a table.
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", relattr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      } else {
        table = default_table;
      }
    } else {
      auto it = table_map.find(relattr.relation_name);
      if (it == table_map.end()) {
        LOG_ERROR("Invalid table name: %s", relattr.relation_name);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      table = it->second;
    }

    const FieldMeta *field_meta = table->table_meta().field(relattr.attribute_name);
    if (nullptr == field_meta) {
      LOG_WARN("no such field. field=%s.%s", table->name(), relattr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }

    // FIXME: Memory leak
    expr.value.field = new Field(table, field_meta);
    return RC::SUCCESS;
  }

  if (expr.type == EXPR_FUNC) {
    auto &func = expr.value.func;

    bool agg_accept_star = false;
    bool is_agg = false;

    if (strcasecmp(func.name, "count") == 0) {
      agg_accept_star = true;
      is_agg = true;
    } else if (strcasecmp(func.name, "max") == 0 || strcasecmp(func.name, "min") == 0 ||
               strcasecmp(func.name, "avg") == 0 || strcasecmp(func.name, "sum") == 0) {
      is_agg = true;
    }

    if (is_agg) {
      expr.type = EXPR_AGG;
    }

    for (int i = 0; i < func.arg_num; i++) {
      rc = fill_expr(default_table, table_map, func.args[i], agg_accept_star);

      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to fill expr in func");
        return rc;
      }
    }
    return rc;
  }

  return rc;
}

/// Expand * attr and replace value.attr with value.field
static RC expand_attr(const std::vector<Table *> &tables, const std::unordered_map<std::string, Table *> &table_map,
    AttrExpr &attr, std::vector<AttrExpr> &attrs)
{
  if (attr.expr.type != EXPR_ATTR) {
    LOG_ERROR("Cannot expand non-single-attr attr expr");
    return RC::INTERNAL;
  }

  const auto &relation_attr = attr.expr.value.attr;

  if (common::is_blank(relation_attr.attribute_name)) {
    LOG_ERROR("Attr name is blank");
    return RC::INVALID_ARGUMENT;
  }

  // *
  if (common::is_blank(relation_attr.relation_name) && 0 == strcmp(relation_attr.attribute_name, "*")) {
    for (int i = tables.size() - 1; i >= 0; i--) {
      Table *table = tables[i];
      wildcard_fields(table, attrs);
    }
    return RC::SUCCESS;
  }

  // t.?
  if (!common::is_blank(relation_attr.relation_name)) {
    const char *table_name = relation_attr.relation_name;
    const char *field_name = relation_attr.attribute_name;

    // *.?
    if (0 == strcmp(table_name, "*")) {
      if (0 != strcmp(field_name, "*")) {
        // *.a
        LOG_WARN("invalid field name while table is *. attr=%s", field_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      // *.*
      for (Table *table : tables) {
        wildcard_fields(table, attrs);
      }
      return RC::SUCCESS;
    }

    // t.*
    if (0 == strcmp(field_name, "*")) {
      auto iter = table_map.find(table_name);
      if (iter == table_map.end()) {
        LOG_WARN("no such table in from list: %s", table_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = iter->second;
      wildcard_fields(table, attrs);
      return RC::SUCCESS;
    }
  }

  // a or t.a
  RC rc = fill_expr(tables[0], table_map, attr.expr);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to fill attr");
    return rc;
  }
  attrs.emplace_back(attr);

  return RC::SUCCESS;
}

RC SelectStmt::create(Db *db, const Selects &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }
  SelectStmt *select_stmt = new SelectStmt();
  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  for (size_t i = 0; i < select_sql.relation_num; i++) {
    const char *table_name = select_sql.relations[i];
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
  }

  // collect query fields in `select` statement
  std::vector<AttrExpr> attrs;
  RC rc = RC::SUCCESS;
  for (size_t i = 0; i < select_sql.attr_num; i++) {
    auto attr_expr = select_sql.attributes[i];

    switch (attr_expr.expr.type) {
      case EXPR_ATTR:
        rc = expand_attr(tables, table_map, attr_expr, attrs);
        break;
      default:
        rc = fill_expr(tables[0], table_map, attr_expr.expr);
        attrs.emplace_back(attr_expr);
        break;
    }

    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), attrs.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  rc = FilterStmt::create(db, default_table, &table_map, select_sql.conditions, select_sql.condition_num, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }
  // collect join infos int `from` statement
  FilterStmt *join_filter_stmt = nullptr;
  rc = FilterStmt::create(
      db, default_table, &table_map, select_sql.join_conditions, select_sql.join_condition_num, join_filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }
  std::unordered_map<std::string, FilterStmt *> table_join_filters;
  for (size_t i = 0; i < select_sql.relation_join_num; i++) {
    FilterStmt *inner_join_filter_stmt = nullptr;
    std::string table_name = select_sql.relation_join_list[i].relation_name;
    rc = FilterStmt::create(db,
        default_table,
        &table_map,
        select_sql.relation_join_list[i].conditions,
        select_sql.relation_join_list[i].condition_num,
        inner_join_filter_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
    table_join_filters.emplace(table_name, inner_join_filter_stmt);
  }
  // everything alright
  select_stmt->tables_.swap(tables);
  select_stmt->attrs_.swap(attrs);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->join_filter_stmt_ = join_filter_stmt;
  select_stmt->table_join_filters_.swap(table_join_filters);
  stmt = select_stmt;
  return RC::SUCCESS;
}
