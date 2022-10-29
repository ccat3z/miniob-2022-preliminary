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
// Created by WangYunlai on 2022/6/7.
//

#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"
#include "storage/common/table.h"
#include "storage/common/field_meta.h"

class TupleCell
{
public: 
  TupleCell() = default;

  TupleCell(FieldMeta *meta, char *data, bool is_null = false) : TupleCell(meta->type(), data, is_null)
  {}
  TupleCell(AttrType attr_type, char *data, bool is_null = false)
      : attr_type_(attr_type), data_(data), is_null_(is_null)
  {}

  void set_type(AttrType type) { this->attr_type_ = type; }
  void set_length(int length) { this->length_ = length; }
  void set_data(char *data) { this->data_ = data; }
  void set_data(const char *data) { this->set_data(const_cast<char *>(data)); }

  void to_string(std::ostream &os) const;

  int compare(const TupleCell &other) const;

  const char *data() const
  {
    return data_;
  }

  int length() const { return length_; }

  AttrType attr_type() const
  {
    return attr_type_;
  }

  bool try_cast(const AttrType &type) const;
  bool try_best_cast(const AttrType &type);

  // Force convert to float
  float as_float() const;

  bool is_null() const
  {
    return is_null_;
  }

  void set_null(bool is_null)
  {
    is_null_ = is_null;
  }

private:
  mutable AttrType attr_type_ = UNDEFINED;
  mutable int length_ = -1;
  mutable char *data_ = nullptr;  // real data. no need to move to field_meta.offset
  bool is_null_;

public:
  template <typename T>
  void save(const T &d, AttrType type)
  {
    placeholder.reset(malloc(sizeof(T)), free);
    *(T *)placeholder.get() = d;

    attr_type_ = type;
    data_ = (char *)placeholder.get();
    length_ = sizeof(T);
    is_null_ = false;
  }

  void save(int32_t i)
  {
    save(i, INTS);
  }

  void save(float f)
  {
    save(f, FLOATS);
  }

  void save(const char *val)
  {
    placeholder.reset(strdup(val), free);

    attr_type_ = CHARS;
    data_ = (char *)placeholder.get();
    length_ = strlen(val) + 1;
    is_null_ = false;
  }

  void save(const std::string &str)
  {
    save(str.c_str());
  }

  template <typename T>
  RC unsafe_get(const T *&b, AttrType type) const
  {
    if (data_ == nullptr) {
      LOG_ERROR("Failed to get value from cell. Cell is null");
      return RC::GENERIC_ERROR;
    }

    if (attr_type_ != type) {
      if (!try_cast(type)) {
        LOG_ERROR("Failed to get value from cell. Expect type: %d, actual: %d", type, attr_type_);
        return RC::GENERIC_ERROR;
      }
    }

    b = (const T *)data_;
    return RC::SUCCESS;
  }

  RC safe_get(const int32_t *&v) const
  {
    return unsafe_get(v, INTS);
  }

  RC safe_get(const float *&v) const
  {
    return unsafe_get(v, FLOATS);
  }

  RC safe_get(const char *&v) const
  {
    return unsafe_get(v, CHARS);
  }

private:
  std::shared_ptr<void> placeholder;
};
