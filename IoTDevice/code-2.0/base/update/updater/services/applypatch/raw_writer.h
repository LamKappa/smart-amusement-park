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

#ifndef UPDATER_RAW_WRITER_H
#define UPDATER_RAW_WRITER_H

#include <cstdio>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include "applypatch/data_writer.h"

namespace updater {
class RawWriter : public DataWriter {
public:
    virtual bool Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName);

    explicit RawWriter(const std::string partitionName) : offset_(0), fd_(-1), partitionName_(partitionName) {}

    virtual ~RawWriter()
    {
        offset_ = 0;
        if (fd_ > 0) {
            fsync(fd_);
            close(fd_);
        }
        fd_ = -1;
    }
private:
    int WriteInternal(int fd, const uint8_t *data, size_t len);

    RawWriter(const RawWriter&) = delete;

    const RawWriter& operator=(const RawWriter&) = delete;
    off64_t offset_;
    int fd_;
    std::string partitionName_;
};
} // namespace updater
#endif /* UPDATER_RAW_WRITER_H */
