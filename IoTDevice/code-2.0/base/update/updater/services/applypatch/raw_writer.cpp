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
#include "raw_writer.h"
#include <cerrno>
#include <cstdio>
#include <string>
#include <unistd.h>
#include "log/log.h"

namespace updater {
bool RawWriter::Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName)
{
    if (addr == nullptr) {
        LOG(ERROR) << "RawWriter: invalid address.";
        return false;
    }

    if (len == 0) {
        LOG(WARNING) << "RawWriter: write length is 0, skip.";
        return false;
    }

    if (fd_ < 0) {
        fd_ = OpenPartition(partitionName_);
        if (fd_ < 0) {
            return false;
        }
    }

    UPDATER_CHECK_ONLY_RETURN(WriteInternal(fd_, addr, len) >= 0, return false);
    return true;
}

int RawWriter::WriteInternal(int fd, const uint8_t *data, size_t len)
{
    ssize_t written = 0;
    size_t rest = len;

    int ret = lseek64(fd, offset_, SEEK_SET);
    UPDATER_FILE_CHECK(ret != -1, "RawWriter: failed to seek file to " << offset_, return -1);

    while (rest > 0) {
        written = write(fd, data, rest);
        UPDATER_FILE_CHECK(written >= 0, "RawWriter: failed to write data of len " << len, return -1);
        data += written;
        rest -= written;
    }
    offset_ += len;
    return 0;
}
} // namespace updater
