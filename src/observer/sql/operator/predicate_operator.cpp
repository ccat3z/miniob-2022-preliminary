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
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/expr/tuple_cell.h"
#include "sql/operator/predicate_operator.h"
#include "sql/parser/parse_defs.h"
#include "storage/record/record.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/field.h"
#include <regex>
#include <stdexcept>

RC PredicateOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("predicate operator must has one child");
    return RC::INTERNAL;
  }

  return children_[0]->open();
}

RC PredicateOperator::next()
{
  RC rc = RC::SUCCESS;
  Operator *oper = children_[0];
  // next（）获取当前的tuple，如果符合predicate的条件则返回rc，如果不符合，则继续获取下一个tuple然后继续查找。
  while (RC::SUCCESS == (rc = oper->next())) {
    Tuple *tuple = oper->current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }
    LOG_DEBUG("PredicateOperator::current_tuple():tuple->cell_num():%d", tuple->cell_num());
    tuple->print();
    if (do_predicate(static_cast<Tuple &>(*tuple))) {
      return rc;
    }
  }
  return rc;
}

RC PredicateOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple * PredicateOperator::current_tuple()
{
  return children_[0]->current_tuple();
}

bool PredicateOperator::do_predicate(Tuple &tuple)
{
  if (filter_stmt_ == nullptr || filter_stmt_->filter_units().empty()) {
    return true;
  }

  for (const FilterUnit *filter_unit : filter_stmt_->filter_units()) {
    Expression *left_expr = filter_unit->left();
    Expression *right_expr = filter_unit->right();
    CompOp comp = filter_unit->comp();
    TupleCell left_cell;
    TupleCell right_cell;

    if (RC::SUCCESS != left_expr->get_value(tuple, left_cell)) {
      throw std::invalid_argument("get value failed for filter");
    }

    if (comp == OP_IN || comp == OP_NOT_IN) {
      RC rc = RC::SUCCESS;
      bool found = false;
      rc = right_expr->get_values(tuple, [&](TupleCell &cell) {
        if (cell.is_null() && left_cell.is_null()) {
          found = true;
          return RC::RECORD_EOF;
        }

        if (cell.is_null() || left_cell.is_null()) {
          return RC::SUCCESS;
        }

        if (cell.compare(left_cell) == 0) {
          found = true;
          return RC::RECORD_EOF;
        }
        return RC::SUCCESS;
      });

      if (found) {
        return comp == OP_IN ? true : false;
      }

      if (rc == RC::SUCCESS) {
        return comp == OP_IN ? false : true;
      }

      throw std::invalid_argument("Failed to get values for filter");
    }

    if (RC::SUCCESS != right_expr->get_value(tuple, right_cell)) {
      throw std::invalid_argument("get value failed for filter");
    }

    if (comp == IS_NULL) {
      return left_cell.is_null();
    }

    if (comp == IS_NOT_NULL) {
      return !left_cell.is_null();
    }

    if (left_cell.is_null() || right_cell.is_null()) {
      return false;
    }

    if (comp == OP_LIKE || comp == OP_NOT_LIKE) {
      if (left_cell.attr_type() != CHARS || right_cell.attr_type() != CHARS) {
        LOG_WARN("like support CHARS only");
        return false;
      }

      std::string target;
      std::string like_expr;

      // TODO: Check invalid like op
      if (left_expr->type() == ExprType::FIELD) {
        target = left_cell.data();
        like_expr = right_cell.data();
      } else {
        target = right_cell.data();
        like_expr = left_cell.data();
      }

      std::string regex_expr = "^";
      regex_expr.reserve(7);
      for (const auto c : like_expr) {
        switch (c) {
          case '%':
            regex_expr.append(".*");
            break;
          case '_':
            regex_expr.push_back('.');
            break;
          default:
            regex_expr.push_back(c);
            break;
        }
      }
      regex_expr.push_back('$');

      std::regex regex(regex_expr, std::regex_constants::icase);
      auto res = std::regex_match(target.begin(), target.end(), regex);

      if (comp == OP_LIKE && !res)
        return false;
      else if (comp == OP_NOT_LIKE && res)
        return false;

      continue;
    }

    const int compare = left_cell.compare(right_cell);
    bool filter_result = false;
    switch (comp) {
    case EQUAL_TO: {
      filter_result = (0 == compare); 
    } break;
    case LESS_EQUAL: {
      filter_result = (compare <= 0); 
    } break;
    case NOT_EQUAL: {
      filter_result = (compare != 0);
    } break;
    case LESS_THAN: {
      filter_result = (compare < 0);
    } break;
    case GREAT_EQUAL: {
      filter_result = (compare >= 0);
    } break;
    case GREAT_THAN: {
      filter_result = (compare > 0);
    } break;
    default: {
      LOG_WARN("invalid compare type: %d", comp);
    } break;
    }
    if (!filter_result) {
      return false;
    }
  }
  return true;
}

// int PredicateOperator::tuple_cell_num() const
// {
//   return children_[0]->tuple_cell_num();
// }
// RC PredicateOperator::tuple_cell_spec_at(int index, TupleCellSpec &spec) const
// {
//   return children_[0]->tuple_cell_spec_at(index, spec);
// }
