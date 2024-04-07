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
#include "sparse_image.h"
#include <cerrno>
#include "log/log.h"
#include "securec.h"
#include "sparse_chunk.h"
#include "sparse_image_handler.h"

namespace updater {
// Check if sparse image header is valid.
// returns true is sparse image header is valid,
// otherwise, returns false.
static bool IsValidSparseImageHeader(const SparseImageHeader &header)
{
    if (header.magicNumber != SPARSE_IMAGE_MAGIC) {
        LOG(ERROR) << "SparseImage: invalid sparse image magic number";
        return false;
    }

    // Only check if major version is expected.
    if (header.majorVersion != SPARSE_IMAGE_MAJOR_VERSIOIN) {
        LOG(ERROR) << "SparseImage: sparse image with incorrect version";
        return false;
    }

    // Check if sparse image size is sufficient
    if (header.fileHeaderSize < sizeof(SparseImageHeader)) {
        LOG(ERROR) << "SparseImage: sparse image header size is invalid";
        return false;
    }

    if (header.chunkHeaderSize < sizeof(SparseImageChunkHeader)) {
        LOG(ERROR) << "SparseImage: sparse image chunk header size is invalid";
        return false;
    }

    // Everything seems OK.
    return true;
}

static struct SparseImage *AllocateSparseImage(int64_t totalSize, uint32_t blockSize)
{
    struct SparseImage *si = static_cast<struct SparseImage *>(calloc(1, sizeof(struct SparseImage)));
    if (si == nullptr) {
        LOG(ERROR) << "SparseImage: allocate memory for sparse image failed: " << errno;
        return nullptr;
    }

    si->bl = static_cast<struct BlockList *>(calloc(1, sizeof(struct BlockList)));
    if (si->bl == nullptr) {
        LOG(ERROR) << "SparseImage: allocate memory for block list failed: " << errno;
        free(si);
    }

    si->blockSize = blockSize;
    si->totalSize = totalSize;
    si->bl->blockSize = blockSize;
    return si;
}

void DeallocateSparseImage(struct SparseImage *si)
{
    if (si == nullptr) {
        return;
    }
    DeallocateBlockList(si->bl);
    free(si);
    si = nullptr;
}

static int ReadSparseImageFromBuffer(struct SparseImage *si, SparseImageHandlerFromBuffer &imageBuffer)
{
    SparseImageHeader imageHeader {};
    SparseImageChunkHeader chunkHeader {};
    uint32_t currentBlock = 0;

    int ret = imageBuffer.ReadContent(&imageHeader, sizeof(SparseImageHeader));
    if (ret < 0) {
        LOG(ERROR) << "SparseImage: Read sparse image header failed";
        return ret;
    }

    if (!IsValidSparseImageHeader(imageHeader)) {
        return -EINVAL;
    }
    // check if there is a gab between SparseImageHeader::fileHeaderSize and length of SparseImageHeader
    if (imageHeader.fileHeaderSize > sizeof(SparseImageHeader)) {
        imageBuffer.Seek(imageHeader.fileHeaderSize - sizeof(SparseImageHeader));
    }

    for (uint32_t i = 0; i < imageHeader.totalChunks; i++) {
        ret = imageBuffer.ReadContent(&chunkHeader, sizeof(SparseImageChunkHeader));
        if (ret < 0) {
            LOG(ERROR) << "SparseImage: Read sparse image chunk header failed";
            return ret;
        }

        if (imageHeader.chunkHeaderSize > sizeof(SparseImageChunkHeader)) {
            imageBuffer.Seek(imageHeader.chunkHeaderSize - sizeof(SparseImageChunkHeader));
        }
        ret = HandleChunks(si, imageBuffer, chunkHeader, imageHeader, currentBlock);
        if (ret < 0) {
            return -EINVAL;
        }
    }

    // Check if all chunks handled
    if (imageHeader.totalBlocks != currentBlock) {
        LOG(ERROR) << "SparseImage: read sparse image failed: " << " expected " << imageHeader.totalBlocks <<
            " actual handled blocks: " << currentBlock;
        return -EINVAL;
    }
    return 0;
}

struct SparseImage *CreateSparseImageFromBuffer(char *buff)
{
    SparseImageHandlerFromBuffer imageBuffer(buff);
    SparseImageHeader imageHeader {};
    struct SparseImage *si = nullptr;

    // Read sparse image header from buffer.
    int ret = imageBuffer.ReadContent(&imageHeader, sizeof(SparseImageHeader));
    if (ret < 0) {
        LOG(ERROR) << "SparseImage: Read sparse image header failed";
        return nullptr;
    }

    if (!IsValidSparseImageHeader(imageHeader)) {
        return nullptr;
    }

    // Well, the sparse image header looks fine.
    // Get total size of sparse image.
    auto sparseImageSize = static_cast<int64_t>(imageHeader.totalBlocks * imageHeader.blockSize);
    si = AllocateSparseImage(sparseImageSize, imageHeader.blockSize);
    if (si == nullptr) {
        return nullptr;
    }

    imageBuffer.SetOffset(0);
    ret = ReadSparseImageFromBuffer(si, imageBuffer);
    if (ret < 0) {
        DeallocateSparseImage(si);
        return nullptr;
    }
    return si;
}
} // namespace updater
