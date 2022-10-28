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
#include "sql/expr/tuple.h"
#include "sql/parser/parse_defs.h"
#include <cstring>
#include <memory>
#include <stdexcept>

RC FieldExpr::get_value(const Tuple &tuple, TupleCell &cell) const
{
  return tuple.find_cell(field_, cell);
}

RC ValueExpr::get_value(const Tuple &tuple, TupleCell & cell) const
{
  cell = tuple_cell_;
  return RC::SUCCESS;
}

std::unique_ptr<Expression> Expression::create(const UnionExpr &union_expr)
{
  Expression *expr = nullptr;
  switch (union_expr.type) {
    case EXPR_ATTR: {
      auto &field = *union_expr.hack.field;
      auto table = field.table();
      auto field_meta = field.meta();
      expr = new FieldExpr(table, field_meta);
    } break;
    case EXPR_VALUE: {
      expr = new ValueExpr(union_expr.value.value);
    } break;
    case EXPR_FUNC: {
      auto &name = union_expr.value.func.name;
      // TODO: ignore case
      if (strcmp(name, "length") == 0) {
        expr = new LengthFuncExpr(union_expr.value.func);
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