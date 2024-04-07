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

#include "sparse_chunk.h"

#include <cstdint>

#include "log/log.h"

namespace updater {
static bool IsValidChunk(uint32_t chunkContentSize, uint32_t chunkSize, uint32_t blockSize)
{
    // should never happen
    if (blockSize == 0) {
        LOG(ERROR) << "SparseImage: block size is zero";
        return false;
    }
    // check if chunk content size align with block size.
    if (chunkContentSize % blockSize != 0) {
        LOG(ERROR) << "SparseImage: invalid chunk " <<
            " chunk content is not aligned with " << blockSize;
        return false;
    }

    if (chunkContentSize / blockSize != chunkSize) {
        LOG(ERROR) << "SparseImage: invalid chunk " <<
            " chunk content size is " << chunkContentSize <<
            " while chunk size record in chunk header is " << chunkSize * blockSize;
        return false;
    }
    return true;
}

static int HandleRawChunk(struct SparseImage *si, SparseImageHandlerFromBuffer &imageBuffer,
    const SparseImageChunkHeader &chunkHeader, const SparseImageHeader &imageHeader, uint32_t currentBlock)
{
    int64_t outputContentSize = chunkHeader.chunkSize * imageHeader.blockSize;
    uint32_t chunkContentSize = chunkHeader.totalSize - static_cast<uint32_t>(imageHeader.chunkHeaderSize);

    if (!IsValidChunk(chunkContentSize, chunkHeader.chunkSize, imageHeader.blockSize)) {
        return -EINVAL;
    }

    int ret = imageBuffer.CopyToSparseImage(si, outputContentSize, currentBlock);
    if (ret == 0) {
        imageBuffer.Seek(outputContentSize);
    }
    return (ret < 0) ? -EINVAL : 0;
}

static int HandleFillChunk(struct SparseImage *si, SparseImageHandlerFromBuffer &imageBuffer,
    const SparseImageChunkHeader &chunkHeader, const SparseImageHeader &imageHeader, uint32_t currentBlock)
{
    int64_t outputContentSize = chunkHeader.chunkSize * imageHeader.blockSize;
    uint32_t chunkContentSize = chunkHeader.totalSize - static_cast<uint32_t>(imageHeader.chunkHeaderSize);
    uint32_t value;

    if (chunkContentSize != sizeof(value)) {
        LOG(ERROR) << "SparseImage: invalid fill chunk";
        return -EINVAL;
    }

    int ret = imageBuffer.ReadContent(&value, sizeof(value));
    if (ret < 0) {
        LOG(ERROR) << "SparseImage: failed to read value from fill chunk";
        return -EINVAL;
    }

    ret = AddFillValueToSparseImage(si->bl, value, outputContentSize, currentBlock);
    return (ret < 0) ? -EINVAL : 0;
}

int HandleChunks(struct SparseImage *si, SparseImageHandlerFromBuffer &imageBuffer,
    const SparseImageChunkHeader &chunkHeader, const SparseImageHeader &imageHeader, uint32_t &currentBlock)
{
    int ret = 0;
    switch (chunkHeader.type) {
        case CHUNK_TYPE_RAW:
            ret = HandleRawChunk(si, imageBuffer, chunkHeader, imageHeader, currentBlock);
            if (ret < 0) {
                LOG(ERROR) << "SparseImage: handle raw chunk failed";
                return -EINVAL;
            }
            currentBlock += chunkHeader.chunkSize;
            break;
        case CHUNK_TYPE_FILL:
            ret = HandleFillChunk(si, imageBuffer, chunkHeader, imageHeader, currentBlock);
            if (ret < 0) {
                LOG(ERROR) << "SparseImage: handle fill chunk failed";
                return -EINVAL;
            }
            currentBlock += chunkHeader.chunkSize;
            break;
        case CHUNK_TYPE_DONT_CARE:
        {
            int64_t chunkContentSize = chunkHeader.totalSize - sizeof(SparseImageChunkHeader);
            if (chunkContentSize != 0) { // For don't care chunk, chunk content size should be zero.
                LOG(ERROR) << "SparseImage: dont care chunk with non-zero chunk size";
                return -EINVAL;
            }
            currentBlock += chunkHeader.chunkSize;
            break;
        }
        case CHUNK_TYPE_CRC32:
            LOG(WARNING) << "SparseImage: crc32 chunk is unsupported it for now";
            return -EINVAL;
        default:
            LOG(ERROR) << "SparseImage: unexpected chunk type: " << std::hex << chunkHeader.type << std::dec;
            return -EINVAL;
    }
    return ret;
}
} // namespace updater
