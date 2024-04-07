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
#ifndef UPDATER_BLOCKSET_H
#define UPDATER_BLOCKSET_H

#include <string>
#include <vector>

#ifndef SIZE_MAX
#define SIZE_MAX (~(size_t)0)
#endif

using BlockPair = std::pair<size_t, size_t>;
static constexpr int H_BLOCK_SIZE = 4096;
static constexpr int H_CMD_ARGS_LIMIT = 2;
static constexpr int H_DIFF_CMD_ARGS_START = 3;
static constexpr int H_MOVE_CMD_ARGS_START = 1;
static constexpr int H_ZERO_NUMBER = 0;


namespace updater {
class Command;

class BlockSet {
public:
    BlockSet()
    {
        blockSize_ = 0;
    }

    explicit BlockSet(std::vector<BlockPair> &&pairs);

    // Insert block to set after parsing from a string type or vector type
    bool ParserAndInsert(const std::string &blockStr);

    bool ParserAndInsert(const std::vector<std::string> &blockToken);

    // Get a number of ranges
    size_t CountOfRanges() const;

    // Get total size of blocks
    size_t TotalBlockSize() const;

    // Get begin iterator of blocks
    std::vector<BlockPair>::iterator Begin();

    // Get end iterator of blocks
    std::vector<BlockPair>::iterator End();

    // Get const begin iterator of blocks
    std::vector<BlockPair>::const_iterator CBegin() const;

    // Get const end iterator of blocks
    std::vector<BlockPair>::const_iterator CEnd() const;

    std::vector<BlockPair>::const_reverse_iterator CrBegin() const;

    std::vector<BlockPair>::const_reverse_iterator CrEnd() const;

    // Get a block by index
    const BlockPair& operator[] (size_t index) const
    {
        return blocks_[index];
    }

    static int32_t VerifySha256(const std::vector<uint8_t> &buffer, const size_t size,
        const std::string &expected);

    static bool IsTwoBlocksOverlap(const BlockSet &source, BlockSet &target);

    static void MoveBlock(std::vector<uint8_t> &target, const BlockSet &locations,
        const std::vector<uint8_t> &source);

    int32_t LoadTargetBuffer(const Command &cmd, std::vector<uint8_t> &buffer, size_t &blockSize, size_t pos,
        std::string &srcHash);
    int32_t WriteZeroToBlock(int fd, bool isErase = true);

    int32_t WriteDiffToBlock(const Command &cmd, std::vector<uint8_t> &sourceBuffer, const size_t srcBlockSize,
        bool isImgDiff = true);

    // Read data from block
    size_t ReadDataFromBlock(int fd, std::vector<uint8_t> &buffer);

    // write data to block
    size_t WriteDataToBlock(int fd, std::vector<uint8_t> &buffer);
protected:
    size_t blockSize_;
    std::vector<BlockPair> blocks_;
private:
    void PushBack(BlockPair block_pair);
    void ClearBlocks();
    bool CheckReliablePair(BlockPair pair);
    int32_t LoadSourceBuffer(const Command &cmd, size_t &pos, std::vector<uint8_t> &sourceBuffer,
        bool &isOverlap, size_t &srcBlockSize);
};
} // namespace updater
#endif // UPDATER_BLOCKSET_H
