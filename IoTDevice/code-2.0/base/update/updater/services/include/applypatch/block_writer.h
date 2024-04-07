/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UPDATER_BLOCK_WRITER_H
#define UPDATER_BLOCK_WRITER_H
#include <cstdio>
#include <string>
#include <unistd.h>
#include "applypatch/block_set.h"
#include "applypatch/data_writer.h"

namespace updater {
class BlockWriter : public DataWriter {
public:
    virtual bool Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName);
    virtual ~BlockWriter() {}
    BlockWriter(int fd, BlockSet& bs) : fd_(fd), bs_(bs), totalWritten_(0), blockIndex_(0),
        currentBlockLeft_(0) {}
    size_t GetTotalWritten() const;
    size_t GetBlocksSize() const;
    bool IsWriteDone() const;
private:
    BlockWriter(const BlockWriter&) = delete;
    const BlockWriter& operator=(const BlockWriter&) = delete;
    int fd_;
    BlockSet bs_;
    size_t totalWritten_;
    // index of BlockPair in BlockSet
    size_t blockIndex_;
    size_t currentBlockLeft_;
};
} // namespace updater
#endif // UPDATER_BLOCK_WRITER_H
