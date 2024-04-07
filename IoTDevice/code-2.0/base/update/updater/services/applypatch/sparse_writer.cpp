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
#include "sparse_writer.h"
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include "log/log.h"
#include "sparse_image/sparse_image_api.h"

namespace updater {
constexpr uint32_t SPARSE_HEADER_MAGIC = 0XED26FF3A;
SparseWriter::~SparseWriter()
{
    if (sf_ != nullptr) {
        DeallocateSparseImage(sf_);
    }
}

bool SparseWriter::Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName)
{
    if (addr == nullptr) {
        LOG(ERROR) << "SparseWrite: invalid address.";
        return false;
    }

    if (len == 0) {
        LOG(WARNING) << "SparseWrite: write length is 0, skip.";
        return false;
    }
    if ((len <= sizeof(SPARSE_HEADER_MAGIC)) || (*reinterpret_cast<const uint32_t *>(addr) != SPARSE_HEADER_MAGIC)) {
        LOG(ERROR) << "SparseWrite: Invalid sparse image.";
        return false;
    }

    auto p = const_cast<uint8_t *>(addr);
    sf_ = CreateSparseImageFromBuffer(reinterpret_cast<char *>(p));
    UPDATER_ERROR_CHECK(sf_ != nullptr, "Sparsewrite: cannot get sparse file descriptor.", return false);
    int fd = OpenPartition(partitionName_);
    UPDATER_CHECK_ONLY_RETURN(fd >= 0, return false);
    int ret = SparseImageRestore(fd, sf_);
    UPDATER_ERROR_CHECK(ret >= 0, "Sparsewrite: write sparse image failed.", close(fd); return false);
    LOG(INFO) << "Sparsewrite: write sparse image successfully.";
    close(fd);
    return true;
}
} // updater
