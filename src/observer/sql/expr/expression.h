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
// Created by Wangyunlai on 2022/07/05.
//

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <vector>
#include "common/log/log.h"
#include "rc.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/select_stmt.h"
#include "storage/common/field.h"
#include "sql/expr/tuple_cell.h"

class Tuple;

enum class ExprType { NONE, FIELD, VALUE, EVAL };

class Expression
{
public: 
  Expression() = default;
  virtual ~Expression() = default;
  
  virtual RC get_value(const Tuple &tuple, TupleCell &cell) const = 0;
  virtual RC get_values(const Tuple &tuple, std::function<RC(TupleCell &cell)> cells) const
  {
    LOG_ERROR("%s not support get_values", toString(true).c_str());
    return RC::GENERIC_ERROR;
  };
  virtual ExprType type() const = 0;
  virtual std::string toString(bool show_table, bool show_table_alias = false) const = 0;

  bool operator==(const Expression &other)
  {
    return toString(true) == other.toString(true);
  }

protected:
  // May throw exception
  static std::unique_ptr<Expression> create(const UnionExpr &expr);
  friend RC create_expression(std::unique_ptr<Expression> &expr, const UnionExpr &union_expr) noexcept;
};

class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field)
  {}
  FieldExpr(const Field &field) : field_(field)
  {}

  virtual ~FieldExpr() = default;

  ExprType type() const override
  {
    return ExprType::FIELD;
  }

  Field &field()
  {
    return field_;
  }

  const Field &field() const
  {
    return field_;
  }

  const char *table_name() const
  {
    return field_.table_name();
  }

  const char *field_name() const
  {
    return field_.field_name();
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    if (show_table) {
      return std::string(show_table_alias ? field_.table_alias() : field_.table_name()) + "." + field_.field_name();
    } else {
      return field_.field_name();
    }
  }

private:
  Field field_;
};

class ValueExpr : public Expression
{
public:
  ValueExpr() = default;
  ValueExpr(const Value &value) : tuple_cell_(value.type, (char *)value.data, value.is_null)
  {
    if (value.type == CHARS) {
      tuple_cell_.set_length(strlen((const char *)value.data));
    }
  }

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, TupleCell & cell) const override;
  ExprType type() const override
  {
    return ExprType::VALUE;
  }

  void get_tuple_cell(TupleCell &cell) const {
    cell = tuple_cell_;
  }

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    tuple_cell_.to_string(ss);
    return ss.str();
  }

private:
  TupleCell tuple_cell_;
};

class LengthFuncExpr : public Expression {
public:
  LengthFuncExpr(const FuncExpr &expr)
  {
    if (expr.arg_num != 1) {
      throw std::invalid_argument("length() only accept 1 arg");
    }

    arg = create(expr.args[0]);
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << "length(" << arg->toString(show_table, show_table_alias) << ")";
    return ss.str();
  }

private:
  std::unique_ptr<Expression> arg;
};

class AggFuncExpr : public Expression {
public:
  AggFuncExpr(const FuncExpr &expr)
  {
    for (int i = 0; i < expr.arg_num; i++) {
      args.emplace_back(create(expr.args[i]));
    }
    name = expr.name;
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << name << "(";

    bool first = true;
    for (auto &arg : args) {
      if (first)
        first = false;
      else
        ss << ",";

      ss << arg->toString(show_table, show_table_alias);
    }
    ss << ")";
    return ss.str();
  }

  const std::string &agg_func() const
  {
    return name;
  }

private:
  std::vector<std::unique_ptr<Expression>> args;
  std::string name;
  friend class Aggregator;
  friend class CountAggregator;
};

class RuntimeAttrExpr : public Expression {
public:
  RuntimeAttrExpr(const RelAttr &attr)
  {
    if (attr.relation_name)
      table = attr.relation_name;
    if (attr.attribute_name)
      field = attr.attribute_name;
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override
  {
    LOG_DEBUG("Not implemnt");
    return RC::INTERNAL;
  }

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    if (show_table && !table.empty()) {
      return std::string(table) + "." + field;
    } else {
      return field;
    }
  }

private:
  std::string table;
  std::string field;
};

class ContextOperator;
class ProjectOperator;
class SelectStmt;
class SelectExpr : public Expression {
public:
  SelectExpr(const SelectStmt *select) : select(select)
  {
    std::stringstream ss;
    ss << "Q#" << (int64_t)select;
    query_name = ss.str();
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;
  RC get_values(const Tuple &tuple, std::function<RC(TupleCell &cell)> on_cell) const override;

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    return query_name;
  }

private:
  std::string query_name;
  mutable std::shared_ptr<ProjectOperator> oper;
  mutable std::shared_ptr<ContextOperator> ctx_oper;
  const SelectStmt *select;
};

RC create_expression(std::unique_ptr<Expression> &expr, const UnionExpr &union_expr) noexcept;