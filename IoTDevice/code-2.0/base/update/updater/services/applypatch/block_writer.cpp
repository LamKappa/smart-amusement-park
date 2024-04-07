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

#include "applypatch/block_writer.h"
#include "applypatch/block_set.h"
#include "log/log.h"
#include "utils.h"

namespace updater {
bool BlockWriter::IsWriteDone() const
{
    return blockIndex_ == bs_.CountOfRanges() && currentBlockLeft_ == 0;
}

size_t BlockWriter::GetBlocksSize() const
{
    return bs_.TotalBlockSize() * H_BLOCK_SIZE;
}

size_t BlockWriter::GetTotalWritten() const
{
    return totalWritten_;
}

bool BlockWriter::Write(const uint8_t *addr, size_t len, WriteMode mode, const std::string &partitionName)
{
    (void)(partitionName);
    (void)(mode);

    if (IsWriteDone()) {
        LOG(WARNING) << "BlockWriter: call writer while no more blocks need to write. skip " << len << " byte(s)";
        return false;
    }
    LOG(DEBUG) << "BlockWriter: try to write " << len << " byte(s)";
    while (len > 0) {
        // Check if blocks can be written.
        // All blocks are written.
        LOG(DEBUG) << "blockIndex = " << blockIndex_;

        // Current block written is done, get next block.
        if (currentBlockLeft_ == 0) {
            if (blockIndex_ >= bs_.CountOfRanges()) {
                LOG(DEBUG) << "block write overflow";
                return false;
            }
            // Get next block pair to write.
            const BlockPair &bp = bs_[blockIndex_];
            // where do we start to write.
            off_t offset = static_cast<off_t>(bp.first) * H_BLOCK_SIZE;
            currentBlockLeft_ = (bp.second - bp.first) * H_BLOCK_SIZE;
            LOG(DEBUG) << "Init currentBlockLeft_ = " << currentBlockLeft_;
            blockIndex_++;
            // Move to right place to write.
            if (lseek(fd_, offset, SEEK_SET) < 0) {
                LOG(ERROR) << "BlockWriter: Cannot seek to offset: " << offset << " Block index: " << blockIndex_;
                return false;
            }
        } else {
            LOG(DEBUG) << "Current block " << blockIndex_ << " left " << currentBlockLeft_ << " to written.";
        }
        size_t written = len;
        if (currentBlockLeft_ < len) {
            written = currentBlockLeft_;
        }
        if (updater::utils::WriteFully(fd_, addr, written) == false) {
            LOG(ERROR) << "BlockWriter: failed to write " << written << " byte(s).";
            return false;
        }
        len -= written;
        addr += written;
        currentBlockLeft_ -= written;
        totalWritten_ += written;
    }

    if (fsync(fd_) == -1) {
        LOG(ERROR) << "Failed to fsync ";
        return -1;
    }

    return true;
}
} // namespace updater
