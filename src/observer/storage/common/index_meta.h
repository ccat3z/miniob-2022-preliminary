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
// Created by Meiyi & Wangyunlai on 2021/5/12.
//

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_META_H__
#define __OBSERVER_STORAGE_COMMON_INDEX_META_H__

#include <memory>
#include <string>
#include <vector>
#include "rc.h"

class TableMeta;
class FieldMeta;
struct RID;

namespace Json {
class Value;
}  // namespace Json

class IndexMeta {
public:
  IndexMeta() = default;

  RC init(const char *name, const FieldMeta &field, bool unique);
  RC init(const char *name, const std::vector<FieldMeta> &fields, bool unique);

public:
  const char *name() const;
  const char *field() const;

  const std::vector<FieldMeta> &fields() const;
  FieldMeta mixed_field_meta() const;

  void desc(std::ostream &os) const;
  bool unique() const
  {
    return unique_;
  }

  std::string extract_key(const char *record, const RID *rid) const;

public:
  void to_json(Json::Value &json_value) const;
  static RC from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index);

protected:
  std::string name_;   // index's name
  std::string field_;  // fields' names list string
  std::vector<FieldMeta> fields_;  // fields' names
  bool unique_ = false;
};
#endif  // __OBSERVER_STORAGE_COMMON_INDEX_META_H__