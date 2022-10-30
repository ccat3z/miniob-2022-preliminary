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

#include "sql/expr/expression.h"
#include "common/log/log.h"
#include "common/time/datetime.h"
#include "sql/executor/execute_stage.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_cell.h"
#include "sql/operator/operator.h"
#include "sql/parser/parse_defs.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <stdexcept>
#include <strings.h>
#include <vector>

RC FieldExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  return tuple.find_cell(field_, cell);
}

RC ValueExpr::get_value(const Tuple &tuple, TupleCell & cell) const
{
  cell = tuple_cell_;
  return RC::SUCCESS;
}

class TupleExpr : public Expression {
public:
  TupleExpr(const FuncExpr &expr)
  {
    for (size_t i = 0; i < expr.arg_num; i++) {
      args.emplace_back(create(expr.args[i]));
    }
  }

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  RC get_value(const Tuple &tuple, TupleCell &cell) const override
  {
    RC rc = RC::SUCCESS;
    bool got = false;

    rc = get_values(tuple, [&](TupleCell &c) {
      if (got) {
        LOG_ERROR("Got multi values");
        return RC::INVALID_ARGUMENT;
      }

      cell = c;
      got = true;
      return RC::SUCCESS;
    });

    if (got) {
      return rc;
    } else {
      LOG_ERROR("Failed to get any cell from query");
      return RC::GENERIC_ERROR;
    }
  }

  RC get_values(const Tuple &tuple, std::function<RC(TupleCell &cell)> on_cell) const override
  {
    RC rc = RC::SUCCESS;
    TupleCell cell;
    for (auto &arg : args) {
      rc = arg->get_value(tuple, cell);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("tuple eval failed");
        return rc;
      }

      rc = on_cell(cell);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("tuple iter on_cell failed");
        return rc;
      }
    }
    return rc;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << "(";
    bool first = true;
    for (auto &arg : args) {
      if (!first) {
        ss << ",";
      } else {
        first = false;
      }
      ss << arg->toString(show_table, show_table_alias);
    }
    ss << ")";
    return ss.str();
  }

private:
  std::vector<std::unique_ptr<Expression>> args;
};

class NegExpression : public Expression {
public:
  NegExpression(const FuncExpr &expr)
  {
    if (expr.arg_num != 1) {
      throw std::invalid_argument("- only accept 1 arg");
    }

    arg = create(expr.args[0]);
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override
  {
    RC rc = arg->get_value(tuple, cell);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("neg: failed to get cell");
      return rc;
    }

    if (!cell.try_best_cast(FLOATS)) {
      LOG_ERROR("neg: only accept number");
      return rc;
    }

    const float *num;
    rc = cell.safe_get(num);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("neg: failed to get float");
      return rc;
    }

    cell.save(-*num);
    return RC::SUCCESS;
  }

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << "-" << arg->toString(show_table, show_table_alias);
    return ss.str();
  }

private:
  std::unique_ptr<Expression> arg;
};

class CalcExpression : public Expression {
public:
  CalcExpression(const FuncExpr &expr, std::string op, std::function<float(float, float)> calc) : calc(calc), op(op)
  {
    if (expr.arg_num != 2) {
      throw std::invalid_argument("calc expr only accept 2 arg");
    }

    exprs.emplace_back(create(expr.args[0]));
    exprs.emplace_back(create(expr.args[1]));
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override
  {
    float f[2];

    for (int i = 0; i < 2; i++) {
      RC rc = exprs[i]->get_value(tuple, cell);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("%s: failed to get cell", op.c_str());
        return rc;
      }

      if (cell.is_null()) {
        return RC::SUCCESS;
      }

      if (!cell.try_best_cast(FLOATS)) {
        LOG_ERROR("%s: only accept number", op.c_str());
        return rc;
      }

      const float *num;
      rc = cell.safe_get(num);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("%s: failed to get float", op.c_str());
        return rc;
      }
      f[i] = *num;
    }

    float res = calc(f[0], f[1]);
    switch (std::fpclassify(res)) {
      case FP_INFINITE:
      case FP_NAN:
        cell.set_null(true);
        break;
      default:
        cell.save(res);
        break;
    }
    return RC::SUCCESS;
  }

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << exprs[0]->toString(show_table, show_table_alias) << op << exprs[1]->toString(show_table, show_table_alias);
    return ss.str();
  }

private:
  std::vector<std::unique_ptr<Expression>> exprs;
  std::function<float(float, float)> calc;
  std::string op;
};

class RoundExpression : public Expression {
public:
  RoundExpression(const FuncExpr &expr)
  {
    if (expr.arg_num != 2) {
      throw std::invalid_argument("round expr only accept 2 arg");
    }

    exprs.emplace_back(create(expr.args[0]));
    exprs.emplace_back(create(expr.args[1]));
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override
  {
    float num;
    int precision;

    {
      RC rc = RC::SUCCESS;
      const float *val;
      if (RC::SUCCESS != (rc = exprs[0]->get_value(tuple, cell))) {
        LOG_ERROR("round(): failed to get cell");
        return rc;
      } else if (cell.is_null()) {
        return RC::SUCCESS;
      } else if (!cell.try_best_cast(FLOATS)) {
        LOG_ERROR("round(): only accept number");
        return RC::INVALID_ARGUMENT;
      } else if (RC::SUCCESS != (rc = cell.safe_get(val))) {
        LOG_ERROR("round(): failed to get value from cell");
        return rc;
      } else {
        num = *val;
      }
    }

    {
      RC rc = RC::SUCCESS;
      const int *val;

      if (RC::SUCCESS != (rc = exprs[1]->get_value(tuple, cell))) {
        LOG_ERROR("round(): failed to get cell");
        return rc;
      } else if (cell.is_null()) {
        return RC::SUCCESS;
      } else if (!cell.try_best_cast(INTS)) {
        LOG_ERROR("round(): only accept number");
        return RC::INVALID_ARGUMENT;
      } else if (RC::SUCCESS != (rc = cell.safe_get(val))) {
        LOG_ERROR("round(): failed to get value from cell");
        return rc;
      } else {
        precision = *val;
      }
    }

    float scale = std::pow(10, precision);
    if (num < 0)
      scale = -scale;
    num *= scale;
    num = int(num + .5);
    num /= scale;
    cell.save(num);
    return RC::SUCCESS;
  }

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << "round(" << exprs[0]->toString(show_table, show_table_alias) << ","
       << exprs[1]->toString(show_table, show_table_alias) << ")";
    return ss.str();
  }

private:
  std::vector<std::unique_ptr<Expression>> exprs;
};

class DateFormatExpression : public Expression {
public:
  DateFormatExpression(const FuncExpr &expr)
  {
    if (expr.arg_num != 2) {
      throw std::invalid_argument("date_format expr only accept 2 arg");
    }

    exprs.emplace_back(create(expr.args[0]));
    exprs.emplace_back(create(expr.args[1]));
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override
  {
    int32_t julian;
    const char *format;

    {
      RC rc = RC::SUCCESS;
      const int32_t *val;
      if (RC::SUCCESS != (rc = exprs[0]->get_value(tuple, cell))) {
        LOG_ERROR("date_format(): failed to get cell");
        return rc;
      } else if (cell.is_null()) {
        return RC::SUCCESS;
      } else if (!cell.try_best_cast(DATE)) {
        LOG_ERROR("date_format(): only accept date");
        return RC::INVALID_ARGUMENT;
      } else if (RC::SUCCESS != (rc = cell.unsafe_get(val, DATE))) {
        LOG_ERROR("date_format(): failed to get value from cell");
        return rc;
      } else {
        julian = *val;
      }
    }

    {
      RC rc = RC::SUCCESS;
      const char *val;

      if (RC::SUCCESS != (rc = exprs[1]->get_value(tuple, cell))) {
        LOG_ERROR("date_format(): failed to get cell");
        return rc;
      } else if (cell.is_null()) {
        return RC::SUCCESS;
      } else if (!cell.try_best_cast(CHARS)) {
        LOG_ERROR("date_format(): only accept CHARS");
        return RC::INVALID_ARGUMENT;
      } else if (RC::SUCCESS != (rc = cell.safe_get(val))) {
        LOG_ERROR("date_format(): failed to get value from cell");
        return rc;
      } else {
        format = val;
      }
    }

    common::Date date(julian);
    cell.save(date.format(format));
    return RC::SUCCESS;
  }

  ExprType type() const override
  {
    return ExprType::EVAL;
  };

  std::string toString(bool show_table, bool show_table_alias = false) const override
  {
    std::stringstream ss;
    ss << "round(" << exprs[0]->toString(show_table, show_table_alias) << ","
       << exprs[1]->toString(show_table, show_table_alias) << ")";
    return ss.str();
  }

private:
  std::vector<std::unique_ptr<Expression>> exprs;
};

std::unique_ptr<Expression> Expression::create(const UnionExpr &union_expr)
{
  Expression *expr = nullptr;
  switch (union_expr.type) {
    case EXPR_ATTR: {
      auto &field = *union_expr.hack.field;
      expr = new FieldExpr(field);
    } break;
    case EXPR_VALUE: {
      expr = new ValueExpr(union_expr.value.value);
    } break;
    case EXPR_FUNC: {
      auto &name = union_expr.value.func.name;
      // TODO: ignore case
      if (strcasecmp(name, "length") == 0) {
        expr = new LengthFuncExpr(union_expr.value.func);
      } else if (strcasecmp(name, "tuple") == 0) {
        expr = new TupleExpr(union_expr.value.func);
      } else if (strcasecmp(name, "round") == 0) {
        expr = new RoundExpression(union_expr.value.func);
      } else if (strcasecmp(name, "date_format") == 0) {
        expr = new DateFormatExpression(union_expr.value.func);
      } else if (strcasecmp(name, "neg") == 0) {
        expr = new NegExpression(union_expr.value.func);
      } else if (strcasecmp(name, "+") == 0) {
        expr = new CalcExpression(union_expr.value.func, name, [](float a, float b) { return a + b; });
      } else if (strcasecmp(name, "-") == 0) {
        expr = new CalcExpression(union_expr.value.func, name, [](float a, float b) { return a - b; });
      } else if (strcasecmp(name, "*") == 0) {
        expr = new CalcExpression(union_expr.value.func, name, [](float a, float b) { return a * b; });
      } else if (strcasecmp(name, "/") == 0) {
        expr = new CalcExpression(union_expr.value.func, name, [](float a, float b) { return a / b; });
      } else {
        throw std::invalid_argument(std::string("Unsupport func: ") + name);
      }
    } break;
    case EXPR_AGG: {
      expr = new AggFuncExpr(union_expr.value.func);
    } break;
    case EXPR_RUNTIME_ATTR: {
      expr = new RuntimeAttrExpr(union_expr.value.attr);
    } break;
    case EXPR_SELECT: {
      expr = new SelectExpr(union_expr.hack.select);
    } break;
    default:
      LOG_ERROR("Unsupport expr type: %d", union_expr.type);
      throw std::invalid_argument("Unsupport expr type");
  }

  return std::unique_ptr<Expression>(expr);
}

RC create_expression(std::unique_ptr<Expression> &expr, const UnionExpr &union_expr) noexcept
{
  try {
    expr = Expression::create(union_expr);
  } catch (const std::exception &e) {
    LOG_ERROR("Failed to create expression: %s", e.what());
    return RC::INVALID_ARGUMENT;
  }

  return RC::SUCCESS;
}

RC LengthFuncExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  RC rc = RC::SUCCESS;

  const char *val;

  if (RC::SUCCESS != (rc = arg->get_value(tuple, cell))) {
    LOG_ERROR("length(): failed to get cell");
    return rc;
  } else if (cell.is_null()) {
    cell.set_null(true);
    return RC::SUCCESS;
  } else if (!cell.try_best_cast(CHARS)) {
    LOG_ERROR("length(): only accept CHARS");
    return RC::INVALID_ARGUMENT;
  } else if (RC::SUCCESS != (rc = cell.safe_get(val))) {
    LOG_ERROR("length(): failed to get value from cell");
    return rc;
  } else {
    cell.save((int32_t)strlen(val));
  }

  return rc;
};

RC AggFuncExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  return tuple.find_cell(*this, cell);
}

RC SelectExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  RC rc = RC::SUCCESS;
  bool got = false;

  rc = get_values(tuple, [&](TupleCell &c) {
    if (got) {
      LOG_ERROR("Got multi values");
      return RC::INVALID_ARGUMENT;
    }

    cell = c;
    got = true;
    return RC::SUCCESS;
  });

  if (got) {
    return rc;
  } else {
    LOG_ERROR("Failed to get any cell from query");
    return RC::GENERIC_ERROR;
  }
}

class ContextOperator : public Operator {
public:
  virtual RC open() override
  {
    return RC::SUCCESS;
  }

  virtual RC next() override
  {
    if (readed)
      return RC::RECORD_EOF;

    readed = true;
    return RC::SUCCESS;
  }

  virtual RC close() override
  {
    readed = false;
    return RC::SUCCESS;
  }

  Tuple *current_tuple() override
  {
    return &tuple;
  }

  MemoryTuple tuple;
  bool readed = false;
};

RC SelectExpr::get_values(const Tuple &tuple, std::function<RC(TupleCell &cell)> on_cell) const
{
  if (oper == nullptr) {
    ctx_oper = std::make_shared<ContextOperator>();
    oper = build_operator(*select, ctx_oper);
    if (!oper) {
      LOG_ERROR("Failed to build operator");
      return RC::INVALID_ARGUMENT;
    }
  }

  ctx_oper->tuple = tuple;
  if (oper->tuple_cell_num() != 1) {
    LOG_ERROR("Sub query can only have 1 column");
    return RC::INVALID_ARGUMENT;
  }

  RC rc = exec_operator(*oper, [&](Tuple &tuple) {
    TupleCell cell;
    RC rc = tuple.cell_at(0, cell);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to get cell");
      return rc;
    }
    rc = on_cell(cell);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("on_cell() failed");
      return rc;
    }
    return rc;
  });
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to exec query");
    return rc;
  }

  return rc;
}