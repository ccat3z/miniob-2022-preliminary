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

class DecartsJoinOperator : public Operator {
public:
  DecartsJoinOperator() = default;
  virtual ~DecartsJoinOperator() = default;

  RC open() override;
  RC next() override;
  RC close() override;
  Tuple *current_tuple();

private:
  void decartes_one(RowTuple *tuples);
  RC get_child_tuples(Operator *child);  // 获取一个child的所有tuple,并计算它与当前的tuples的笛卡尔积
  bool hasjoined = false;                             // 标记是否进行过笛卡尔积计算
  std::vector<ComplexTuple *> current_tuples;         // 存放计算好的笛卡尔积的结果
  std::vector<ComplexTuple *> tuples;                 // 存放正在计算的笛卡尔积的计算结果
  int current_index = -1;                             // 当前访问的tuple 在tuples 中的index
};
