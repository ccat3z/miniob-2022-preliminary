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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>

#include "rc.h"
#include "sql/stmt/stmt.h"
#include "storage/common/field.h"
#include "unordered_map"
class FieldMeta;
class FilterStmt;
class Db;
class Table;

class SelectStmt : public Stmt
{
public:

  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override { return StmtType::SELECT; }
public:
  static RC create(Db *db, const Selects &select_sql, Stmt *&stmt);

public:
  const std::vector<Table *> &tables() const { return tables_; }
  const std::vector<Field> &query_fields() const { return query_fields_; }
  FilterStmt *filter_stmt() const { return filter_stmt_; }
  FilterStmt *join_filter_stmt() const
  {
    return join_filter_stmt_;
  }
  std::unordered_map<std::string, FilterStmt *> table_join_filters()
  {
    return table_join_filters_;  // 分别存放每个join 算子的filter条件，如果是没有inner join filter 为空
  }
  FilterStmt *get_table_join_filter(std::string table_name)
  {
    if (table_join_filters_.find(table_name) != table_join_filters_.end()) {
      return table_join_filters_[table_name];
    } else {
      return nullptr;
    }
  }
  FilterStmt *get_one_table_filter(std::string table_name)
  {
    if (one_table_filters_.find(table_name) != one_table_filters_.end()) {
      return one_table_filters_[table_name];
    } else {
      return nullptr;
    }
  }

private:
  std::vector<Field> query_fields_;
  std::vector<Table *> tables_;
  FilterStmt *filter_stmt_ = nullptr;
  FilterStmt *join_filter_stmt_ = nullptr;
  std::unordered_map<std::string, FilterStmt *> table_join_filters_;
  std::unordered_map<std::string, FilterStmt *> one_table_filters_;
};

