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
// Created by Meiyi & Wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "sql/parser/parse_defs.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "storage/record/record.h"
#include "json/json.h"
#include <cstddef>
#include <cstdint>
#include <vector>

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_UNIQUE("unique");

RC IndexMeta::init(const char *name, const FieldMeta &field, bool unique)
{
  std::vector<FieldMeta> fields = {field};
  return init(name, fields, unique);
}

RC IndexMeta::init(const char *name, const std::vector<FieldMeta> &fields, bool unique)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  unique_ = unique;

  field_ = "";
  for (const auto &field : fields) {
    field_ += field.name();
    field_ += "+";
    fields_.emplace_back(field);
  }
  if (fields.size() > 0)
    field_.pop_back();

  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME] = name_;
  for (const auto &field : fields_) {
    json_value[FIELD_FIELD_NAME].append(field.name());
  }
  json_value[FIELD_UNIQUE] = unique_;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  const Json::Value &unique_value = json_value[FIELD_UNIQUE];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!field_value.isArray()) {
    LOG_ERROR("Field name of index [%s] is not a array. json value=%s",
        name_value.asCString(),
        field_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!unique_value.isBool()) {
    LOG_ERROR("`unique` is not a bool. json value=%s", name_value.asCString(),
              field_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  std::vector<FieldMeta> fields;
  for (unsigned int i = 0; i < field_value.size(); i++) {
    auto field = table.field(field_value.get(i, "").asCString());
    if (nullptr == field) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    fields.emplace_back(*field);
  }

  return index.init(name_value.asCString(), fields, unique_value.asBool());
}

const char *IndexMeta::name() const
{
  return name_.c_str();
}

const char *IndexMeta::field() const
{
  return field_.c_str();
}

const std::vector<FieldMeta> &IndexMeta::fields() const
{
  return fields_;
}

FieldMeta IndexMeta::mixed_field_meta() const
{
  if (fields_.size() == 1 && !fields_[0].nullable())
    return fields_[0];

  FieldMeta field;
  size_t len = 0;
  for (auto &field : fields_) {
    len += field.len();
    if (field.nullable()) {
      len += sizeof(RID);
    }
  }
  field.init(name_.c_str(), MIXED, 0, len, false);
  return field;
}

static inline void append_zero(std::string &str, size_t size)
{
  char zero[size];
  for (size_t i = 0; i < size; i++)
    zero[i] = 0;
  str.append(zero, size);
}

std::string IndexMeta::extract_key(const char *record, const RID *rid) const
{
  std::string key;
  key.reserve(fields_.size() * 4);

  for (auto &field : fields_) {
    if (field.nullable()) {
      if (!field.is_null(record)) {
        append_zero(key, sizeof(RID));
        key.append(record + field.offset(), field.len());
      } else {
        key.append((char *)rid, sizeof(RID));
        append_zero(key, field.len());
      }
    } else {
      key.append(record + field.offset(), field.len());
    }
  }

  return key;
}

void IndexMeta::desc(std::ostream &os) const
{
  os << "index name=" << name_ << ", field=" << field_ << ", unique=" << (unique_ ? "yes" : "no");
}