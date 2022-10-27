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

#include <memory>
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
  TupleCellSpec(Expression *expr) : expression_(expr)
  {}

  ~TupleCellSpec()
  {
    if (expression_) {
      delete expression_;
      expression_ = nullptr;
      // TODO 释放alias_
    }
  }

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
    return expression_;
  }

private:
  std::string alias_;
  Expression *expression_ = nullptr;
};

class Tuple
{
public:
  Tuple() = default;
  virtual ~Tuple() = default;

  virtual int cell_num() const = 0; 
  virtual RC  cell_at(int index, TupleCell &cell) const = 0;
  virtual RC  find_cell(const Field &field, TupleCell &cell) const = 0;

  virtual RC  cell_spec_at(int index, const TupleCellSpec *&spec) const = 0;
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
class ComplexTuple;
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
class ComplexTuple : public Tuple {
  //  一个tuple 可以关于多个表。 表信息在specs_中，数据信息在tuple_cell 中.
  // complex tuple 的基本由RowTuple 进行构造。
public:
  ComplexTuple() = default;
  ComplexTuple(RowTuple *tuple)
  {
    // 将record 里面的信息拆解到tuple cell 中。
    int nums = tuple->cell_num();
    for (int i = 0; i < nums; i++) {
      TupleCell *cell = new TupleCell();
      const TupleCellSpec *spec;
      tuple->cell_at(i, *cell);
      tuple->cell_spec_at(i, spec);
      tuple_.push_back(cell);
      speces_.push_back(spec);
    }
  }
  ComplexTuple(ComplexTuple *tuple)
  {
    int nums = tuple->cell_num();
    for (int i = 0; i < nums; i++) {
      TupleCell *cell = new TupleCell();
      const TupleCellSpec *spec;
      tuple->cell_at(i, *cell);
      tuple->cell_spec_at(i, spec);
      tuple_.push_back(cell);
      speces_.push_back(spec);
    }
  }
  virtual ~ComplexTuple()
  {
    for (const TupleCell *cell : tuple_) {
      delete cell;
    }
    tuple_.clear();
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
    cell = *tuple_[index];
    return RC::SUCCESS;
  }
  void add_row_tuple(RowTuple *tuple)
  {
    for (int i = 0; i < tuple->cell_num(); i++) {
      TupleCell *cell = new TupleCell();
      const TupleCellSpec *spec;
      tuple->cell_at(i, *cell);
      tuple->cell_spec_at(i, spec);
      tuple_.push_back(cell);
      speces_.push_back(spec);
    }
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
  RC find_cell(const Field &field, TupleCell &cell) const override
  {
    const char *field_name = field.field_name();
    const char *table_name = field.table_name();

    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = (const FieldExpr *)speces_[i]->expression();
      const Field &field = field_expr->field();

      if (0 == strcmp(field_name, field.field_name()) && 0 == strcmp(table_name, field.table_name())) {
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }
  void print()
  {
    // for debug
    std::stringstream ss;
    bool first_field = true;
    for (unsigned i = 0; i < tuple_.size(); i++) {
      if (!first_field) {
        ss << " | ";
      } else {
        first_field = false;
      }
      tuple_[i]->to_string(ss);
    }
    std::cout << "complex tuple print : \n " << ss.str() << std::endl;
  }

private:
  std::vector<const TupleCellSpec *> speces_;
  std::vector<TupleCell *> tuple_;  // TupleCell自定义不需要偏移量，直接取用其data
};
