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
// Created by WangYunlai on 2022/07/01.
//

#include "common/log/log.h"
#include "sql/expr/expression.h"
#include "sql/operator/project_operator.h"
#include "sql/parser/parse_defs.h"
#include "storage/record/record.h"
#include "storage/common/table.h"
#include <memory>

RC ProjectOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("project operator must has 1 child");
    return RC::INTERNAL;
  }

  Operator *child = children_[0];
  RC rc = child->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  return RC::SUCCESS;
}

RC ProjectOperator::next()
{
  return children_[0]->next();
}

RC ProjectOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}
Tuple *ProjectOperator::current_tuple()
{
  Tuple *new_tuple = children_[0]->current_tuple();
  tuple_.set_tuple(new_tuple);
  LOG_DEBUG("ProjectOperator::current_tuple():%d", new_tuple->cell_num());
  LOG_DEBUG("ProjectOperator::current_tuple() after projection:%d", tuple_.cell_num());
  return &tuple_;
}

RC ProjectOperator::add_projection(const std::vector<AttrExpr> &attrs, bool multi_table)
{
  RC rc = RC::SUCCESS;

  for (auto &attr : attrs) {
    std::unique_ptr<Expression> expr = nullptr;

    rc = create_expression(expr, attr.expr);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to create expression");
      return rc;
    }

    if (!expr) {
      LOG_ERROR("Failed to create expression");
      return RC::GENERIC_ERROR;
    }

    TupleCellSpec *spec = new TupleCellSpec(expr.release());
    if (attr.name != nullptr) {
      spec->set_alias(attr.name);
    } else {
      // 对单表来说，展示的(alias) 字段总是字段名称，
      // 对多表查询来说，展示的alias 需要带表名字
      spec->set_alias(spec->expression()->toString(multi_table, true));
    }

    tuple_.add_cell_spec(spec);
  }

  return RC::SUCCESS;
}

RC ProjectOperator::tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const
{
  return tuple_.cell_spec_at(index, spec);
}
