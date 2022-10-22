// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#ifndef __OBSERVER_STORAGE_DEFAULT_LARGE_BLOCK_POOL_H_
#define __OBSERVER_STORAGE_DEFAULT_LARGE_BLOCK_POOL_H_
#include "rc.h"
#include "common/conf/ini.h"
#include <memory>
#include <string>

#define LARGE_BLOCK_POOL_BLOCK_SIZE ((size_t)4096)

class LargeBlock {
public:
  char data[LARGE_BLOCK_POOL_BLOCK_SIZE];
  char null = 0;
};

class LargeBlockPool {
public:
  LargeBlockPool();
  virtual ~LargeBlockPool();

  RC open_file(std::string file_name);
  void close();
  RC remove();

  std::unique_ptr<LargeBlock> get(uint32_t idx) const;
  RC set(uint32_t idx, const char *data, size_t size);
  void mark(uint32_t idx, bool used);

  uint32_t find_next_free();

  static LargeBlockPool *&instance()
  {
    if (instance_ == nullptr) {
      // HACK: Save lbp data in storage stage
      auto base_dir = common::get_properties()->get("BaseDir", "./", "DefaultStorageStage");

      instance_ = new LargeBlockPool();
      instance_->open_file(base_dir + "/lbp.data");
    }
    return instance_;
  }

  static void reset()
  {
    if (instance_) {
      free(instance_);
      instance_ = nullptr;
    }
  }

private:
  RC init();
  std::string file_name;
  int fd = -1;

  static LargeBlockPool *instance_;
};

#endif // __OBSERVER_STORAGE_DEFAULT_LARGE_BLOCK_POOL_H_