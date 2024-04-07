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

#include "applypatch/block_set.h"
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <unistd.h>
#include "applypatch/command.h"
#include "applypatch/store.h"
#include "applypatch/transfer_manager.h"
#include "log/log.h"
#include "patch/update_patch.h"
#include "securec.h"
#include "utils.h"

using namespace updater;
using namespace updater::utils;

namespace updater {
BlockSet::BlockSet(std::vector<BlockPair> &&pairs)
{
    blockSize_ = 0;
    UPDATER_ERROR_CHECK(!pairs.empty(), "Invalid block.", return);

    for (const auto &pair : pairs) {
        UPDATER_CHECK_ONLY_RETURN(CheckReliablePair(pair), return);
        PushBack(pair);
    }
}

bool BlockSet::CheckReliablePair(BlockPair pair)
{
    UPDATER_ERROR_CHECK(pair.first < pair.second, "Invalid number of block size", return false);
    size_t size = pair.second - pair.first;
    UPDATER_ERROR_CHECK(blockSize_ < (SIZE_MAX - size), "Block size overflow", return false);
    return true;
}

void BlockSet::PushBack(BlockPair blockPair)
{
    blocks_.push_back(std::move(blockPair));
    blockSize_ += (blockPair.second - blockPair.first);
}

void BlockSet::ClearBlocks()
{
    blockSize_ = 0;
    blocks_.clear();
}

bool BlockSet::ParserAndInsert(const std::string &blockStr)
{
    UPDATER_ERROR_CHECK(blockStr != "", "Invalid argument, this argument is empty", return false);
    std::vector<std::string> pairs = SplitString(blockStr, ",");
    return ParserAndInsert(pairs);
}

bool BlockSet::ParserAndInsert(const std::vector<std::string> &blockToken)
{
    ClearBlocks();
    UPDATER_ERROR_CHECK(!blockToken.empty(), "Invalid block token argument", return false);
    UPDATER_ERROR_CHECK(blockToken.size() >= 3, "Too small blocks_ in argument", return false);
    // Get number of blockToken
    unsigned long long int blockPairSize;
    std::vector<std::string> bt = blockToken;
    std::vector<std::string>::iterator bp = bt.begin();
    blockPairSize = String2Int<unsigned long long int>(*bp, N_DEC);
    UPDATER_ERROR_CHECK(blockPairSize != 0 && blockPairSize % 2 == 0 &&
        blockPairSize == bt.size() - 1, "Invalid number in block token", return false);

    while (++bp != bt.end()) {
        size_t first = String2Int<size_t>(*bp++, N_DEC);
        size_t second = String2Int<size_t>(*bp, N_DEC);
        blocks_.push_back(BlockPair {
            first, second
        });
        blockSize_ += (second - first);
    }
    return true;
}

std::vector<BlockPair>::iterator BlockSet::Begin()
{
    return blocks_.begin();
}

std::vector<BlockPair>::iterator BlockSet::End()
{
    return blocks_.end();
}

std::vector<BlockPair>::const_iterator BlockSet::CBegin() const
{
    return blocks_.cbegin();
}

std::vector<BlockPair>::const_iterator BlockSet::CEnd() const
{
    return blocks_.cend();
}

std::vector<BlockPair>::const_reverse_iterator BlockSet::CrBegin() const
{
    return blocks_.crbegin();
}

std::vector<BlockPair>::const_reverse_iterator BlockSet::CrEnd() const
{
    return blocks_.crend();
}

size_t BlockSet::ReadDataFromBlock(int fd, std::vector<uint8_t> &buffer)
{
    size_t pos = 0;
    std::vector<BlockPair>::iterator it = blocks_.begin();
    int ret;
    for (; it != blocks_.end(); ++it) {
        ret = lseek64(fd, static_cast<off64_t>(it->first * H_BLOCK_SIZE), SEEK_SET);
        UPDATER_ERROR_CHECK(ret != -1, "Fail to seek", return -1);
        size_t size = (it->second - it->first) * H_BLOCK_SIZE;
        UPDATER_ERROR_CHECK(utils::ReadFully(fd, buffer.data() + pos, size), "Fail to read", return -1);
        pos += size;
    }
    return pos;
}

size_t BlockSet::WriteDataToBlock(int fd, std::vector<uint8_t> &buffer)
{
    size_t pos = 0;
    std::vector<BlockPair>::iterator it = blocks_.begin();
    int ret = 0;
    for (; it != blocks_.end(); ++it) {
        off64_t offset = static_cast<off64_t>(it->first * H_BLOCK_SIZE);
        size_t writeSize = (it->second - it->first) * H_BLOCK_SIZE;
#ifndef UPDATER_UT
        uint64_t arguments[] = {static_cast<uint64_t>(offset), writeSize};
        ret = ioctl(fd, BLKDISCARD, &arguments);
        UPDATER_ERROR_CHECK(ret != -1 || errno == EOPNOTSUPP, "Error to write block set to memory", return -1);
#endif
        ret = lseek64(fd, offset, SEEK_SET);
        UPDATER_ERROR_CHECK(ret != -1, "BlockSet::WriteDataToBlock Fail to seek", return -1);
        if (utils::WriteFully(fd, buffer.data() + pos, writeSize) == false) {
            LOG(ERROR) << "Write data to block error, errno : " << errno;
            return -1;
        }
        pos += writeSize;
    }
    UPDATER_ERROR_CHECK(fsync(fd) != -1, "Failed to fsync", return -1);
    return pos;
}

size_t BlockSet::CountOfRanges() const
{
    return blocks_.size();
}

size_t BlockSet::TotalBlockSize() const
{
    return blockSize_;
}

int32_t BlockSet::VerifySha256(const std::vector<uint8_t> &buffer, const size_t size, const std::string &expected)
{
    uint8_t digest[SHA256_DIGEST_LENGTH];
    SHA256(buffer.data(), size * H_BLOCK_SIZE, digest);
    std::string hexdigest = utils::ConvertSha256Hex(digest, SHA256_DIGEST_LENGTH);
    UPDATER_CHECK_ONLY_RETURN(hexdigest != expected, return 0);
    return -1;
}

bool BlockSet::IsTwoBlocksOverlap(const BlockSet &source, BlockSet &target)
{
    auto firstIter = source.CBegin();
    for (; firstIter != source.CEnd(); ++firstIter) {
        std::vector<BlockPair>::iterator secondIter = target.Begin();
        for (; secondIter != target.End(); ++secondIter) {
            if (!(secondIter->first >= firstIter->second ||
                firstIter->first >= secondIter->second)) {
                return true;
            }
        }
    }
    return false;
}

void BlockSet::MoveBlock(std::vector<uint8_t> &target, const BlockSet& locations,
    const std::vector<uint8_t>& source)
{
    const uint8_t *sd = source.data();
    uint8_t *td = target.data();
    size_t start = locations.TotalBlockSize();
    for (auto it = locations.CrBegin(); it != locations.CrEnd(); it++) {
        size_t blocks = it->second - it->first;
        start -= blocks;
        UPDATER_ERROR_CHECK(!memmove_s(td + (it->first * H_BLOCK_SIZE), blocks * H_BLOCK_SIZE, sd + (start *
            H_BLOCK_SIZE), blocks * H_BLOCK_SIZE), "MoveBlock memmove_s failed!", return);
    }
}

int32_t BlockSet::LoadSourceBuffer(const Command &cmd, size_t &pos, std::vector<uint8_t> &sourceBuffer,
    bool &isOverlap, size_t &srcBlockSize)
{
    std::string blockLen = cmd.GetArgumentByPos(pos++);
    srcBlockSize = String2Int<size_t>(blockLen, N_DEC);
    sourceBuffer.resize(srcBlockSize * H_BLOCK_SIZE);
    std::string targetCmd = cmd.GetArgumentByPos(pos++);
    std::string storeBase = TransferManager::GetTransferManagerInstance()->GetGlobalParams()->storeBase;
    if (targetCmd != "-") {
        BlockSet srcBlk;
        srcBlk.ParserAndInsert(targetCmd);
        isOverlap = IsTwoBlocksOverlap(srcBlk, *this);
        // read source data
        LOG(INFO) << "new start to read source block ...";
        UPDATER_CHECK_ONLY_RETURN(srcBlk.ReadDataFromBlock(cmd.GetFileDescriptor(), sourceBuffer) > 0, return -1);
        std::string nextArgv = cmd.GetArgumentByPos(pos++);
        UPDATER_CHECK_ONLY_RETURN(nextArgv != "", return 0);
        BlockSet locations;
        locations.ParserAndInsert(nextArgv);
        MoveBlock(sourceBuffer, locations, sourceBuffer);
    }

    std::string lastArg = cmd.GetArgumentByPos(pos++);
    while (lastArg != "") {
        std::vector<std::string> tokens = SplitString(lastArg, ":");
        UPDATER_ERROR_CHECK(tokens.size() == H_CMD_ARGS_LIMIT, "invalid parameter", return -1);
        std::vector<uint8_t> stash;
        auto ret = Store::LoadDataFromStore(storeBase, tokens[H_ZERO_NUMBER], stash);
        UPDATER_ERROR_CHECK(ret != -1, "Failed to load tokens", return -1);
        BlockSet locations;
        locations.ParserAndInsert(tokens[1]);
        MoveBlock(sourceBuffer, locations, stash);

        lastArg = cmd.GetArgumentByPos(pos++);
    }
    return 1;
}

int32_t BlockSet::LoadTargetBuffer(const Command &cmd, std::vector<uint8_t> &buffer, size_t &blockSize,
    size_t pos, std::string &srcHash)
{
    bool isOverlap = false;
    auto ret = LoadSourceBuffer(cmd, pos, buffer, isOverlap, blockSize);
    UPDATER_CHECK_ONLY_RETURN(ret == 1,
    return ret);
    std::string storeBase = TransferManager::GetTransferManagerInstance()->GetGlobalParams()->storeBase;
    std::string storePath = storeBase + "/" + srcHash;
    struct stat storeStat {};
    int res = stat(storePath.c_str(), &storeStat);
    if (VerifySha256(buffer, blockSize, srcHash) == 0) {
        if (isOverlap && res != 0) {
            ret = Store::WriteDataToStore(storeBase, srcHash, buffer, blockSize * H_BLOCK_SIZE);
            UPDATER_ERROR_CHECK(ret == 0, "failed to stash overlapping source blocks", return -1);
        }
        return 0;
    }
    UPDATER_CHECK_ONLY_RETURN(Store::LoadDataFromStore(storeBase, srcHash, buffer) != 0, return 0);
    return -1;
}

int32_t BlockSet::WriteZeroToBlock(int fd, bool isErase)
{
    std::vector<uint8_t> buffer;
    buffer.resize(H_BLOCK_SIZE);
    UPDATER_ERROR_CHECK(!memset_s(buffer.data(), H_BLOCK_SIZE, 0, H_BLOCK_SIZE), "memset_s failed", return -1);

    auto iter = blocks_.begin();
    while (iter != blocks_.end()) {
        off64_t offset = static_cast<off64_t>(iter->first * H_BLOCK_SIZE);
        int ret = 0;
#ifndef UPDATER_UT
        size_t writeSize = (iter->second - iter->first) * H_BLOCK_SIZE;
        uint64_t arguments[2] = {static_cast<uint64_t>(offset), writeSize};
        ret = ioctl(fd, BLKDISCARD, &arguments);
        UPDATER_ERROR_CHECK(ret != -1 || errno == EOPNOTSUPP, "Error to write block set to memory", return -1);
#endif
        if (!isErase) {
            ret = lseek64(fd, offset, SEEK_SET);
            UPDATER_ERROR_CHECK(ret != -1, "BlockSet::WriteZeroToBlock Fail to seek", return -1);
            for (size_t pos = iter->first; pos < iter->second; pos++) {
                if (utils::WriteFully(fd, buffer.data(), H_BLOCK_SIZE) == false) {
                    UPDATER_CHECK_ONLY_RETURN(errno != EIO, return 1);
                    LOG(ERROR) << "BlockSet::WriteZeroToBlock Write 0 to block error, errno : " << errno;
                    return -1;
                }
            }
        }
        iter++;
    }
    return 0;
}

int32_t BlockSet::WriteDiffToBlock(const Command &cmd, std::vector<uint8_t> &srcBuffer,
    const size_t srcBlockSize, bool isImgDiff)
{
    size_t pos = H_MOVE_CMD_ARGS_START;
    size_t offset = utils::String2Int<size_t>(cmd.GetArgumentByPos(pos++), utils::N_DEC);
    size_t length = utils::String2Int<size_t>(cmd.GetArgumentByPos(pos++), utils::N_DEC);
    LOG(INFO) << "Get patch data offset: " << offset << ", length: " << length;
    // Get patch buffer
    auto globalParams = TransferManager::GetTransferManagerInstance()->GetGlobalParams();
    auto patchBuff = globalParams->patchDataBuffer + offset;
    size_t srcBuffSize =  srcBlockSize * H_BLOCK_SIZE;
    if (isImgDiff) {
        std::vector<uint8_t> empty;
        updatepatch::PatchParam patchParam = {
            reinterpret_cast<u_char*>(srcBuffer.data()), srcBuffSize, reinterpret_cast<u_char*>(patchBuff), length
        };
        std::unique_ptr<BlockWriter> writer = std::make_unique<BlockWriter>(cmd.GetFileDescriptor(), *this);
        UPDATER_ERROR_CHECK(writer.get() != nullptr, "Cannot create block writer, imgdiff patch abort!", return -1);
        int32_t ret = updatepatch::UpdatePatch::ApplyImagePatch(patchParam, empty,
            [&](size_t start, const updatepatch::BlockBuffer &data, size_t size) -> int {
                bool ret = writer->Write(data.buffer, size, WRITE_BLOCK, "");
                return ret ? 0 : -1;
            }, cmd.GetArgumentByPos(pos + 1));
        writer.reset();
        UPDATER_ERROR_CHECK(ret == 0, "Fail to ApplyImagePatch", return -1);
    } else {
        std::vector<uint8_t> targetBuffer;
        targetBuffer.resize(TotalBlockSize() * H_BLOCK_SIZE);
        LOG(DEBUG) << "Run bsdiff patch.";
        updatepatch::PatchBuffer patchInfo = {patchBuff, 0, length};
        auto ret = updatepatch::UpdatePatch::ApplyBlockPatch(patchInfo, {srcBuffer.data(), srcBuffSize}, targetBuffer);
        if (ret != 0) {
            LOG(ERROR) << "Failed to apply bspatch";
            return -1;
        }
        if (VerifySha256(targetBuffer, TotalBlockSize(), cmd.GetArgumentByPos(pos + 1)) != 0) {
            LOG(ERROR) << "Not match sha256 after bspatch";
            return -1;
        }
        if (WriteDataToBlock(cmd.GetFileDescriptor(), targetBuffer) <= 0) {
            LOG(ERROR) << "Error to write source to file";
            return -1;
        }
    }
    return 0;
}
} // namespace updater
