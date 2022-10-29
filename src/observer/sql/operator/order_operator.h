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
// Created by WangYunlai on 2021/6/10.
//

#pragma once

#include "sql/parser/parse.h"
#include "sql/operator/operator.h"
#include "rc.h"

class OrderOperator : public Operator {
public:
  OrderOperator(std::vector<OrderExpr> order_stmt) : order_stmts_(order_stmt)
  {}
  virtual ~OrderOperator()
  {
    for (size_t i = 0; i < current_tuples_.size(); i++) {
      delete current_tuples_[i];
    }
    current_tuples_.clear();
  }

  RC open() override;
  RC next() override;
  RC close() override;
  Tuple *current_tuple();

private:
  RC order_child_tuples(Operator *child);
  void insert_one(Tuple *tuple);
  int compare(ComplexTuple *tuple1, ComplexTuple *tuple2);
  bool hasordered = false;
  std::vector<ComplexTuple *> current_tuples_;
  std::vector<OrderExpr> order_stmts_;
  int current_index = -1;  // 当前访问的tuple 在tuples 中的index
};
