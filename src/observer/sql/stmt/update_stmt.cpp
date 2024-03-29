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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"
#include "storage/common/db.h"
#include "storage/common/field.h"
#include "storage/common/table.h"
#include "select_stmt.h"
#include <unordered_map>

RC UpdateStmt::create(Db *db, const Updates &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name;
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // Fill union expr
  {
    std::unordered_map<std::string, Table *> tables;
    tables[table->name()] = table;
    for (size_t i = 0; i < update.kv_num; i++) {
      RC rc = fill_expr(table, tables, update.kvs[i].value, false, db);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to fill value %d in update", i);
        return rc;
      }
    }
  }

  auto update_stmt = new UpdateStmt();
  update_stmt->table_ = table;
  for (int i = 0; i < update.kv_num; i++) {
    update_stmt->kvs_.emplace_back(update.kvs + i);
  }
  update_stmt->conditions_ = update.conditions;
  update_stmt->condition_num_ = update.condition_num;

  stmt = update_stmt;
  return RC::SUCCESS;
}

RC UpdateStmt::to_rid_select(Db *db, SelectStmt &stmt)
{
  FilterStmt *filter_stmt = nullptr;
  std::unordered_map<std::string, Table *> tables;
  tables[table_->name()] = table_;
  RC rc = FilterStmt::create(db, table_, &tables, conditions_, condition_num_, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  stmt.tables_.emplace_back(table_);
  stmt.filter_stmt_ = filter_stmt;

  AttrExpr expr;
  expr.name = nullptr;
  expr.expr.type = EXPR_ATTR;
  // FIXME: Memory leak
  expr.expr.hack.field = new Field(table_, table_->table_meta().page_field());
  stmt.attrs_.emplace_back(expr);

  return rc;
}