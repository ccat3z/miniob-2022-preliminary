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
// Created by Wangyunlai on 2021/5/14.
//

#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/log/log.h"
#include "sql/parser/parse.h"
#include "sql/expr/tuple_cell.h"
#include "sql/expr/expression.h"
#include "storage/record/record.h"

class Table;

class TupleCellSpec
{
public:
  TupleCellSpec() = default;

  // Take ownership of expr
  TupleCellSpec(Expression *expr) : TupleCellSpec(std::shared_ptr<Expression>(expr))
  {}

  TupleCellSpec(std::shared_ptr<Expression> expr) : expression_(expr)
  {}

  ~TupleCellSpec() = default;

  void set_alias(std::string alias)
  {
    this->alias_ = alias;
  }
  const char *alias() const
  {
    return alias_.c_str();
  }

  Expression *expression() const
  {
    return expression_.get();
  }

private:
  std::string alias_;
  std::shared_ptr<Expression> expression_ = nullptr;
};

class Tuple
{
public:
  Tuple() = default;
  virtual ~Tuple() = default;

  virtual int cell_num() const = 0; 
  virtual RC  cell_at(int index, TupleCell &cell) const = 0;

  virtual RC find_cell(const std::string &cell_name, TupleCell &cell) const
  {
    RC rc = RC::SUCCESS;
    for (int i = 0; i < cell_num(); i++) {
      const TupleCellSpec *spec;
      rc = cell_spec_at(i, spec);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to get spec at %d", i);
        return rc;
      }

      if (spec->expression()->toString(true) == cell_name) {
        return cell_at(i, cell);
      }
    }

    LOG_ERROR("Cannot find cell %s in tuple", cell_name.c_str());
    return RC::NOTFOUND;
  }

  virtual RC find_cell(const Field &field, TupleCell &cell) const
  {
    return find_cell(std::string(field.table_name()) + "." + field.field_name(), cell);
  }

  virtual RC find_cell(const Expression &expr, TupleCell &cell) const
  {
    return find_cell(expr.toString(true), cell);
  }

  virtual RC  cell_spec_at(int index, const TupleCellSpec *&spec) const = 0;

  // for debug
  std::string to_string()
  {
    std::stringstream ss;
    bool first_field = true;
    for (int i = 0; i < cell_num(); i++) {
      if (!first_field) {
        ss << " | ";
      } else {
        first_field = false;
      }

      TupleCell cell;
      cell_at(i, cell);
      cell.to_string(ss);
    }
    return ss.str();
  }

  // for debug
  void print()
  {
    LOG_DEBUG(to_string().c_str());
  }

  bool operator<(const Tuple &other) const
  {
    if (cell_num() != other.cell_num()) {
      return cell_num() < other.cell_num();
    }

    for (int i = 0; i < cell_num(); i++) {
      TupleCell this_cell, other_cell;
      RC rc = cell_at(i, this_cell);
      if (rc != RC::SUCCESS) {
        throw std::invalid_argument("failed to compare tuple");
      }

      rc = other.cell_at(i, other_cell);
      if (rc != RC::SUCCESS) {
        throw std::invalid_argument("failed to compare tuple");
      }

      // HACK
      // Generally two NULL cannot be compared. But two NULL are equal in key
      // tuple. Now we compare only the key tuple.
      if (this_cell.is_null() && other_cell.is_null()) {
        continue;
      } else if (this_cell.is_null() && !other_cell.is_null()) {
        return true;
      } else if (!this_cell.is_null() && other_cell.is_null()) {
        return false;
      }

      auto res = this_cell.compare(other_cell);
      if (res == 0)
        continue;
      else if (res < 0)
        return true;
      else
        return false;
    }

    return false;
  }

  bool operator==(const Tuple &other) const
  {
    if (cell_num() != other.cell_num()) {
      return false;
    }

    for (int i = 0; i < cell_num(); i++) {
      TupleCell this_cell, other_cell;
      RC rc = cell_at(i, this_cell);
      if (rc != RC::SUCCESS) {
        throw std::invalid_argument("failed to compare tuple");
      }

      rc = other.cell_at(i, other_cell);
      if (rc != RC::SUCCESS) {
        throw std::invalid_argument("failed to compare tuple");
      }

      // HACK
      // Generally two NULL cannot be compared. But two NULL are equal in key
      // tuple. Now we compare only the key tuple.
      if (this_cell.is_null() && other_cell.is_null()) {
        continue;
      } else if (this_cell.is_null() || other_cell.is_null()) {
        return false;
      }

      auto res = this_cell.compare(other_cell);
      if (res != 0)
        return false;
    }

    return true;
  }
};

class RowTuple : public Tuple {
public:
  RowTuple() = default;
  RowTuple(RowTuple *tuple)
  {
    record_ = &tuple->record();
    for (int i = 0; i < tuple->cell_num(); i++) {
      const TupleCellSpec *spec;
      tuple->cell_spec_at(i, spec);
      speces_.push_back(const_cast<TupleCellSpec *>(spec));
    }
  }
  virtual ~RowTuple()
  {}  //  不在释放放 vector<TUpleCellSpec*> speces_;因为这个部分会被转移到其它Tuple中。

  void set_record(Record *record)
  {
    this->record_ = record;
  }

  void set_schema(const Table *table, const std::vector<FieldMeta> *fields)
  {
    table_ = table;
    this->speces_.reserve(fields->size());
    for (const FieldMeta &field : *fields) {
      speces_.push_back(new TupleCellSpec(new FieldExpr(table, &field)));
    }
  }

  int cell_num() const override
  {
    return speces_.size();
  }

  RC cell_at(int index, TupleCell &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    const TupleCellSpec *spec = speces_[index];
    FieldExpr *field_expr = (FieldExpr *)spec->expression();
    const FieldMeta *field_meta = field_expr->field().meta();
    cell.set_type(field_meta->type());
    cell.set_data(this->record_->data() + field_meta->offset());
    cell.set_length(field_meta->len());
    cell.set_null(field_meta->is_null(record_->data()));
    return RC::SUCCESS;
  }

  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    const char *table_name = field.table_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    const char *field_name = field.field_name();
    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr * field_expr = (const FieldExpr *)speces_[i]->expression();
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }

  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }

  Record &record()
  {
    return *record_;
  }

  const Record &record() const
  {
    return *record_;
  }
private:
  Record *record_ = nullptr;
  const Table *table_ = nullptr;
  std::vector<TupleCellSpec *> speces_;
};

/*
class CompositeTuple : public Tuple
{
public:
  int cell_num() const override;
  RC  cell_at(int index, TupleCell &cell) const = 0;
private:
  int cell_num_ = 0;
  std::vector<Tuple *> tuples_;
};
*/
class ProjectTuple : public Tuple
{
public:
  ProjectTuple() = default;
  virtual ~ProjectTuple()
  {
    for (TupleCellSpec *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  void set_tuple(Tuple *tuple)
  {
    this->tuple_ = tuple;
  }
  void add_cell_spec(TupleCellSpec *spec)
  {
    speces_.push_back(spec);
  }
  int cell_num() const override
  {
    return speces_.size();
  }

  RC cell_at(int index, TupleCell &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::GENERIC_ERROR;
    }
    if (tuple_ == nullptr) {
      return RC::GENERIC_ERROR;
    }

    const TupleCellSpec *spec = speces_[index];
    return spec->expression()->get_value(*tuple_, cell);
  }

  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    return tuple_->find_cell(field, cell);
  }
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::NOTFOUND;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
private:
  std::vector<TupleCellSpec *> speces_;
  Tuple *tuple_ = nullptr;
};

class MemoryTuple : public Tuple {
public:
  MemoryTuple() = default;
  virtual ~MemoryTuple() = default;
  MemoryTuple(const Tuple &tuple)
  {
    append_tuple(tuple);
  };
  MemoryTuple(const Tuple *tuple)
  {
    append_tuple(*tuple);
  };

  int cell_num() const
  {
    return cells.size();
  }

  RC cell_at(int index, TupleCell &cell) const
  {
    if (index < 0 || index >= static_cast<int>(cells.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    cell = cells[index];
    return RC::SUCCESS;
  }

  RC cell_spec_at(int index, const TupleCellSpec *&spec) const
  {
    if (index < 0 || index >= static_cast<int>(specs.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = &specs[index];
    return RC::SUCCESS;
  }

  RC append_cell(const TupleCell &cell, const TupleCellSpec &spec)
  {
    cells.emplace_back(cell);
    specs.emplace_back(spec);
    return RC::SUCCESS;
  }

  RC append_tuple(const Tuple &tuple)
  {
    RC rc = RC::SUCCESS;
    for (int i = 0; i < tuple.cell_num(); i++) {
      cells.emplace_back();
      rc = tuple.cell_at(i, cells.back());
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to get cell from tuple at %d", i);
        return rc;
      }

      const TupleCellSpec *spec;
      rc = tuple.cell_spec_at(i, spec);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to get cell spec from tuple at %d", i);
        return rc;
      }
      specs.emplace_back(*spec);
    }
    return rc;
  }

  RC add_tuple(const Tuple *tuple)
  {
    return append_tuple(*tuple);
  }

private:
  std::vector<TupleCell> cells;
  std::vector<TupleCellSpec> specs;
};

using ComplexTuple = MemoryTuple;