/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UPDATE_SPARSE_WRITER_H
#define UPDATE_SPARSE_WRITER_H

#include <cstdio>
#include <unistd.h>
#include "applypatch/data_writer.h"
#include "sparse_image/sparse_image_api.h"

namespace updater {
class SparseWriter : public DataWriter {
public:
    virtual bool Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName);

    explicit SparseWriter(const std::string &partitionName) : sf_(nullptr), partitionName_(partitionName) {}

    virtual ~SparseWriter();
private:
    struct SparseImage *sf_;

    SparseWriter(const SparseWriter&) = delete;

    const SparseWriter& operator=(const SparseWriter&) = delete;
    std::string partitionName_;
};
} // updater
#endif // UPDATE_SPARSE_WRITER_H
