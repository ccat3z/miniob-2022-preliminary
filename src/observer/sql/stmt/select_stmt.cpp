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
#include "sql/expr/expression.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/field_meta.h"
#include "storage/common/table.h"
#include <cstddef>
#include <cstring>
#include <memory>
#include <set>
#include <strings.h>
#include <unordered_map>
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
    attr.expr.hack.field = new Field(table, table_meta.field(i));

    attrs.emplace_back(attr);
  }
}

RC fill_expr(const Table *default_table, const std::unordered_map<std::string, Table *> &table_map,
    const UnionExpr &expr, bool allow_star, Db *db)
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
        for (auto can_table : tables) {
          if (table == can_table)
            continue;

          auto field_meta = can_table->table_meta().field(relattr.attribute_name);
          if (!field_meta) {
            continue;
          }

          if (table != nullptr) {
            LOG_WARN("Match multiple keys. I do not know the attr's table. attr=%s", relattr.attribute_name);
            if (default_table != nullptr) {
              table = default_table;
              break;
            } else {
              return RC::SCHEMA_FIELD_MISSING;
            }
          }

          table = can_table;
        }

        if (!table) {
          LOG_WARN("invalid. I do not know the attr's table. attr=%s", relattr.attribute_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
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
    expr.hack.field = new Field(table, field_meta);
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
      rc = fill_expr(default_table, table_map, func.args[i], agg_accept_star, db);

      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to fill expr in func");
        return rc;
      }
    }
    return rc;
  }

  if (expr.type == EXPR_SELECT) {
    if (db == nullptr) {
      LOG_ERROR("Cannot build select expr without db");
      return RC::INTERNAL;
    }

    auto ctx = std::make_shared<SelectCtx>();
    ctx->table_map = table_map;

    Stmt *stmt;
    RC rc = SelectStmt::create(db, *expr.value.select, stmt, ctx);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to create stmt for select expr");
      return rc;
    }
    expr.hack.select = (SelectStmt *)stmt;
  }

  return rc;
}

RC walk_expr(const UnionExpr &expr, std::function<RC(const UnionExpr &expr)> walk)
{
  RC rc = RC::SUCCESS;
  switch (expr.type) {
    case EXPR_AGG:
    case EXPR_FUNC: {
      for (int i = 0; rc == RC::SUCCESS && i < expr.value.func.arg_num; i++) {
        rc = walk_expr(expr.value.func.args[i], walk);
      }
      if (rc != RC::SUCCESS) {
        return rc;
      }
    } break;
    default:
      break;
  }

  return walk(expr);
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
    if (!common::is_blank(attr.name)) {
      LOG_ERROR("Cannot set alias on * column");
      return RC::INVALID_ARGUMENT;
    }
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
      if (!common::is_blank(attr.name)) {
        LOG_ERROR("Cannot set alias on * column");
        return RC::INVALID_ARGUMENT;
      }
      for (Table *table : tables) {
        wildcard_fields(table, attrs);
      }
      return RC::SUCCESS;
    }

    // t.*
    if (0 == strcmp(field_name, "*")) {
      if (!common::is_blank(attr.name)) {
        LOG_ERROR("Cannot set alias on * column");
        return RC::INVALID_ARGUMENT;
      }

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

RC SelectStmt::create(Db *db, const Selects &select_sql, Stmt *&stmt, std::shared_ptr<SelectCtx> ctx)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }
  SelectStmt *select_stmt = new SelectStmt();
  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  std::set<std::string> table_names;
  std::unordered_map<const Table *, std::string> alias_dict;

  if (ctx) {
    table_map = ctx->table_map;
  }

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

    auto alias = select_sql.rel_alias[i];
    auto display_table_name = alias ? alias : table_name;
    if (table_names.count(display_table_name) > 0) {
      LOG_ERROR("Duplicated table alias");
      return RC::INVALID_ARGUMENT;
    }

    table_names.emplace(display_table_name);
    alias_dict[table] = display_table_name;
    table_map[display_table_name] = table;
  }

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
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
        rc = fill_expr(default_table, table_map, attr_expr.expr, false, db);
        attrs.emplace_back(attr_expr);
        break;
    }

    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  // Set alias
  for (auto &attr : attrs) {
    walk_expr(attr.expr, [&](const UnionExpr &expr) {
      if (expr.type != EXPR_ATTR) {
        return RC::SUCCESS;
      }

      auto &field = expr.hack.field;
      auto it = alias_dict.find(field->table());
      if (it != alias_dict.end()) {
        field->set_alias(it->second);
      }
      return RC::SUCCESS;
    });
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), attrs.size());

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
  // collect one_table_filter from filter_stmt
  std::unordered_map<std::string, FilterStmt *> one_table_filters;
  const std::vector<FilterUnit *> units = filter_stmt->filter_units();
  for (size_t i = 0; i < units.size(); i++) {
    // 判断filter_stmt 中每个unit 是否独属于一个table，如果是，则加入one_table_filters
    FilterUnit *filter_unit = units[i];
    Expression *left = filter_unit->left();
    Expression *right = filter_unit->right();
    if (!((left->type() == ExprType::FIELD && right->type() == ExprType::VALUE) ||
            (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD))) {
      continue;
    }
    std::string table_name;
    if (left->type() == ExprType::FIELD) {
      FieldExpr *field_expr = (FieldExpr *)(left);
      table_name = field_expr->table_name();
    } else {
      FieldExpr *field_expr = (FieldExpr *)(right);
      table_name = field_expr->table_name();
    }
    if (one_table_filters.find(table_name) == one_table_filters.end()) {
      FilterStmt *tmp_stmt = new FilterStmt();
      tmp_stmt->add_filter_units(filter_unit);
      one_table_filters.emplace(table_name, tmp_stmt);
    } else {
      one_table_filters[table_name]->add_filter_units(filter_unit);
    }
  }

  // Group by
  for (size_t i = 0; i < select_sql.group_num; i++) {
    rc = fill_expr(default_table, table_map, select_sql.groups[i]);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to fill group by expr");
      return rc;
    }
    select_stmt->groups_.emplace_back(select_sql.groups[i]);
  }

  // Having
  if (select_sql.having_num != 0) {
    rc = FilterStmt::create(
        db, default_table, &table_map, select_sql.havings, select_sql.having_num, select_stmt->having_filter_);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt for having");
      return rc;
    }

    // Extract agg funcs
    for (size_t i = 0; i < select_sql.having_num; i++) {
      auto cond = select_sql.havings[i];
      auto left = cond.left_expr;
      auto right = cond.right_expr;

      walk_expr(left, [&](const UnionExpr &expr) {
        if (expr.type == EXPR_AGG)
          select_stmt->extra_exprs_.emplace_back(expr);
        return RC::SUCCESS;
      });
      walk_expr(right, [&](const UnionExpr &expr) {
        if (expr.type == EXPR_AGG)
          select_stmt->extra_exprs_.emplace_back(expr);
        return RC::SUCCESS;
      });
    }
  }

  // Order by
  for (size_t i = 0; i < select_sql.order_num; i++) {
    rc = fill_expr(default_table, table_map, select_sql.orders[i].expr);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to fill order by expr");
      return rc;
    }
    select_stmt->orders_.emplace_back(select_sql.orders[i]);
  }

  // everything alright
  select_stmt->tables_.swap(tables);
  select_stmt->attrs_.swap(attrs);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->join_filter_stmt_ = join_filter_stmt;
  select_stmt->table_join_filters_.swap(table_join_filters);
  select_stmt->one_table_filters_.swap(one_table_filters);
  stmt = select_stmt;
  return RC::SUCCESS;
}
