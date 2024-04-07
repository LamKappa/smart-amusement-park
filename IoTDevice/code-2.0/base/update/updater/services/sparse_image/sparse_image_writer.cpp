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
#include <cerrno>
#include <fcntl.h>
#include <malloc.h>
#include <sys/mman.h>
#include <unistd.h>
#include "log/log.h"
#include "sparse_block.h"
#include "sparse_image.h"

namespace updater {
#define ROUNDUPWITH(a, b) (((a) + (b) - 1) / (b))
#define ALIGNWITH(a, b) ((b) * ROUNDUPWITH((a), (b)))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// When write file block, will map file to memory,
// the mapped memory size should be aligned.
constexpr unsigned int MMAP_ALIGNED_MASK = (4096 - 1);

static int WriteToFd(int fd, void *data, size_t size)
{
    ssize_t written = 0;
    size_t rest = size;

    while (rest > 0) {
        written = write(fd, data, rest);
        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            LOG(ERROR) << "SparseImage: write to fd failed: " << errno;
            return -1;
        }
        data = static_cast<char *>(data) + written;
        rest -= written;
    }
    return 0;
}

static int SkipChunks(int fd, int64_t offset)
{
    off64_t rc = 0;
    rc = lseek64(fd, offset, SEEK_CUR);
    if (rc < 0) {
        LOG(ERROR) << "SparseImage: seek file to " << offset << " failed: " << errno;
        return -1;
    }
    return 0;
}

static int WriteFdBlockToFd(int out, int in, int64_t offset, unsigned int size, unsigned int blockSize)
{
    int64_t alignedOffset = offset & MMAP_ALIGNED_MASK;
    int alignedGap = static_cast<int>(offset - alignedOffset);
    int64_t mapSize = static_cast<int64_t>(size) + static_cast<int64_t>(alignedGap);

    if (mapSize < 0) { // overflow
        LOG(ERROR) << "SparseImage: file size too big. overflow.";
        return -1;
    }

    char *ptr = static_cast<char *>(mmap64(nullptr, mapSize, PROT_READ, MAP_SHARED, in, alignedOffset));
    if (ptr == MAP_FAILED) {
        LOG(ERROR) << "SparseImage: map file failed: " << errno;
        return -1;
    }

    char *p = ptr + alignedGap;
    int ret = WriteToFd(out, p, size);
    if (ret < 0) {
        munmap(ptr, mapSize);
        return ret;
    }

    // Written data should be aligned with @blockSize.
    // if not so, skip reset size.
    unsigned int alignedSize = ALIGNWITH(size, blockSize);
    if (alignedSize > size) {
        ret = SkipChunks(out, alignedSize - size);
        if (ret < 0) {
            munmap(ptr, mapSize);
            return ret;
        }
    }
    munmap(ptr, mapSize);
    return 0;
}

static int WriteFileBlockToFd(int out, const char *fileName, int64_t offset, unsigned int size, unsigned int blockSize)
{
    int in = open(fileName, O_RDONLY);
    if (in < 0) {
        LOG(ERROR) << "SparseImage: open " << fileName << " failed: " << errno;
        return -1;
    }
    int ret = WriteFdBlockToFd(out, in, offset, size, blockSize);
    close(in);
    in = -1;
    return ret;
}

static int WriteDataBlockToFd(int fd, void *data, unsigned int size, unsigned int blockSize)
{
    unsigned int alignedSize = ALIGNWITH(size, blockSize);

    int ret = WriteToFd(fd, data, size);
    if (ret < 0) {
        return ret;
    }
    // Written data should be aligned with @blockSize.
    // if not so, skip reset size.
    if (alignedSize > size) {
        ret = SkipChunks(fd, alignedSize - size);
        if (ret < 0) {
            return -1;
        }
    }
    return 0;
}

static int WriteFillBlockToFd(int fd, uint32_t value, unsigned int size, unsigned int blockSize)
{
    // should never happen
    if (blockSize == 0) {
        LOG(ERROR) << "SparseImage: block size is zero";
        return -1;
    }
    auto buffer = static_cast<uint32_t *>(calloc(blockSize, 1));
    uint32_t toWrite = 0;
    if (buffer == nullptr) {
        LOG(ERROR) << "SparseImage: allocate memory for fill block failed: " << errno;
        return -1;
    }

    for (int i = 0; i < static_cast<int>(blockSize / sizeof(uint32_t)); i++) {
        buffer[i] = value;
    }

    while (size) {
        toWrite = MIN(size, blockSize);
        int ret = WriteToFd(fd, buffer, toWrite);
        if (ret < 0) {
            free(buffer);
            buffer = nullptr;
            return ret;
        }
        size -= toWrite;
    }
    free(buffer);
    buffer = nullptr;
    return 0;
}

static int WriteBlockToFd(int fd, SparseImageBlock *sib, unsigned int blockSize)
{
    BLOCKTYPE type = sib->type;
    int ret = -EINVAL;
    switch (type) {
        case BLOCK_TYPE_DATA:
            ret = WriteDataBlockToFd(fd, sib->data.data, sib->size, blockSize);
            if (ret) {
                LOG(ERROR) << "SparseImage: write data block to file failed";
            }
            break;
        case BLOCK_TYPE_FILE:
            ret = WriteFileBlockToFd(fd, sib->file.fileName, sib->file.offset, sib->size, blockSize);
            if (ret) {
                LOG(ERROR) << "SparseImage: write file block to file failed";
            }
            break;
        case BLOCK_TYPE_FD:
            ret = WriteFdBlockToFd(fd, sib->fd.fd, sib->fd.offset, sib->size, blockSize);
            if (ret) {
                LOG(ERROR) << "SparseImage: write fd block to file failed";
            }
            break;
        case BLOCK_TYPE_FILL:
            ret = WriteFillBlockToFd(fd, sib->fill.value, sib->size, blockSize);
            if (ret) {
                LOG(ERROR) << "SparseImage: write fill block to file failed";
            }
            break;
        default:
            LOG(ERROR) << "SparseImage: Unsupported block type " <<  std::hex << type << std::dec;
            break;
    }
    return ret;
}

// Write Block to fd.
// Write all blocks in sparse image to the
// file bind to fd.
static int WriteBlocksToFd(int fd, struct BlockList *bl, int64_t totalSize)
{
    SparseImageBlock *sib = nullptr;
    uint32_t proceedBlocks = 0;
    int ret = 0;

    for (sib = bl->imageBlocks; sib != nullptr; sib = sib->next) {
        // There is a gap between blocks.
        // maybe there are don't care chunks
        // Just skip it.
        if (sib->blockIndex > proceedBlocks)  {
            uint32_t skippedBlocks = sib->blockIndex - proceedBlocks;
            ret = SkipChunks(fd, skippedBlocks * bl->blockSize);
            if (ret < 0) {
                LOG(ERROR) << "SparseImage: Skip chunks failed";
                return ret;
            }
        }
        ret = WriteBlockToFd(fd, sib, bl->blockSize);
        if (ret < 0) {
            return ret;
        }
        proceedBlocks = sib->blockIndex + ROUNDUPWITH(sib->size, bl->blockSize);
    }

    // We've written all blocks
    // check if there is a gap between totalSize and proceed blocks
    int64_t gap = totalSize - proceedBlocks * bl->blockSize;
    ret = SkipChunks(fd, gap);
    if (ret < 0) {
        LOG(ERROR) << "SparseImage: Skip chunks failed";
    }
    return ret;
}

int SparseImageRestore(int fd, struct SparseImage *si)
{
    if (si == nullptr || fd < 0) {
        LOG(ERROR) << "SparseImage: invalid arguments";
        return -1;
    }

    // We don't need to check if SparseImage is valid here.
    struct BlockList *bl = si->bl;
    int ret = WriteBlocksToFd(fd, bl, si->totalSize);
    if (ret < 0) {
        LOG(ERROR) << "SparseImage: failed to write blocks to fd";
        return -1;
    }
    // truncate the file to sparse image size.
    ret = ftruncate64(fd, si->totalSize);
    if (ret < 0) {
        LOG(WARNING) << "SparseImage: failed to truncate file: " << errno;
    }
    return 0;
}
} // namespace updater
