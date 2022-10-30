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
#include "sql/executor/execute_stage.h"
#include "sql/expr/tuple.h"
#include "sql/expr/tuple_cell.h"
#include "sql/operator/operator.h"
#include "sql/parser/parse_defs.h"
#include <cstring>
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
      ss << arg->toString(show_table, show_table_alias);
      if (first) {
        ss << ",";
        first = false;
      }
    }
    ss << ")";
    return ss.str();
  }

private:
  std::vector<std::unique_ptr<Expression>> args;
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

  TupleCell str_cell;
  rc = arg->get_value(tuple, str_cell);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to get value from arg");
    return rc;
  }

  if (str_cell.is_null()) {
    cell.save(0);
    return RC::SUCCESS;
  }

  const char *str;
  rc = str_cell.safe_get(str);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("length() only accept CHARS");
    return rc;
  }

  cell.save((int32_t)strlen(str));

  return RC::SUCCESS;
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