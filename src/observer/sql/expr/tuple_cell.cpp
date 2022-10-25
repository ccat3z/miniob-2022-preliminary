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
// Created by WangYunlai on 2022/07/05.
//

#include "sql/expr/tuple_cell.h"
#include "storage/common/field.h"
#include "storage/default/large_block_pool.h"
#include "common/log/log.h"
#include "common/time/datetime.h"
#include "util/comparator.h"
#include "util/util.h"
#include <cstdlib>
#include <string>

using namespace std::string_literals;

void TupleCell::to_string(std::ostream &os) const
{
  if (is_null()) {
    os << "NULL";
    return;
  }

  switch (attr_type_) {
  case INTS: {
    os << *(int *)data_;
  } break;
  case FLOATS: {
    float v = *(float *)data_;
    os << double2string(v);
  } break;
  case CHARS: {
    for (int i = 0; i < length_; i++) {
      if (data_[i] == '\0') {
        break;
      }
      os << data_[i];
    }
  } break;
  case DATE: {
    common::Date date(*(int *)data_);
    os << date.format();
  } break;
  case TEXT: {
    auto &lbp = LargeBlockPool::instance();
    auto blk = lbp->get(*(uint32_t *)data_);
    os << blk->data;
  } break;
  default: {
    LOG_WARN("unsupported attr type: %d", attr_type_);
  } break;
  }
}

int TupleCell::compare(const TupleCell &other) const
{
  if (!other.try_cast(attr_type_)) {
    try_cast(other.attr_type_);
  }

  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
    case INTS: return compare_int(this->data_, other.data_);
    case FLOATS: return compare_float(this->data_, other.data_);
    case CHARS: return compare_string(this->data_, this->length_, other.data_, other.length_);
    case DATE:
      return compare_int(this->data_, other.data_);
    default:
      throw std::invalid_argument("unsupported type: "s + std::to_string(this->attr_type_));
    }
  } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = *(int *)data_;
    return compare_float(&this_data, other.data_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = *(int *)other.data_;
    return compare_float(data_, &other_data);
  } else if (this->attr_type_ == DATE || other.attr_type_ == DATE) {
    throw std::invalid_argument("cannot compare DATE with non-DATE");
  } else {
    auto a = as_float();
    auto b = other.as_float();
    return compare_float(&a, &b);
  }
}

bool TupleCell::try_cast(const AttrType &type) const
{
  if (this->attr_type_ == type)
    return true;

  if (this->attr_type_ == CHARS && type == DATE) {
    common::Date date;
    if (!date.parse((char *)this->data_)) {
      return false;
    }

    this->attr_type_ = DATE;
    // FIXME: Memory leak
    this->data_ = (char *)malloc(sizeof(int));
    int julian = date.julian();
    memcpy(this->data_, &julian, sizeof(int));
    return true;
  }

  return false;
}

float TupleCell::as_float() const
{
  /// as float
  switch (this->attr_type_) {
    case INTS:
    case DATE:
      return float(*(int *)data_);
    case FLOATS:
      return *(float *)data_;
    case CHARS:
      return std::atof(data_);
    default:
      return 0;
  }
}