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

#include "sparse_block.h"

#include <cstring>
#include "log/log.h"
#include "securec.h"

namespace updater {
void DeallocateBlockList(struct BlockList *bl)
{
    SparseImageBlock *imageBlocks = bl->imageBlocks;
    while (imageBlocks != nullptr) {
        SparseImageBlock *next = imageBlocks->next;
        if (imageBlocks->type == BLOCK_TYPE_FILE) {
            free(imageBlocks->file.fileName);
        }
        free(imageBlocks);
        imageBlocks = next;
    }
    free(bl);
    bl = nullptr;
}

static void MergeBlocks(unsigned int blockSize, SparseImageBlock *prev, SparseImageBlock *next)
{
    // sanity checks
    if (prev == nullptr || next == nullptr) { // Do not need to merge
        return;
    }

    if (prev->type != next->type) { // block with different type
        LOG(WARNING) << "SparseBlock: try to merge blocks with different type: " <<
            " previous block with type: " << prev->type <<
            " while next block with type: " << next->type;
        return;
    }

    // should never happen
    if (blockSize == 0) {
        LOG(ERROR) << "SparseBlock: block size is zero";
        return;
    }
    // check if these two blocks adjacent
    unsigned int blockCounts = prev->size / blockSize;
    if (prev->blockIndex + blockCounts != next->blockIndex) {
        LOG(WARNING) << "SparseBlock: try to merge two block nonadjacent: " <<
            " previous block index: " << prev->blockIndex << " size: " << prev->size <<
            " while next block index: " << next->blockIndex;
        return;
    }

    if (prev->type == BLOCK_TYPE_FILE) {
        if (strcmp(prev->file.fileName, next->file.fileName) != 0 ||
            prev->file.offset + prev->size != next->file.offset) {
            return;
        }
    } else if (prev->type == BLOCK_TYPE_FD) {
        if (prev->fd.fd != next->fd.fd || prev->fd.offset + prev->size != next->fd.offset) {
            return;
        }
    } else if (prev->type == BLOCK_TYPE_FILL) {
        if (prev->fill.value != next->fill.value) {
            return;
        }
    } else { // BLOCK_TYPE_DATA, do not need to merge
        return;
    }

    // OK, we've done all checks, now merge next to prev.
    prev->size += next->size;
    prev->next = next->next;
    // free next;
    if (next->type == BLOCK_TYPE_FILE) {
        free(next->file.fileName);
    }
    free(next);
    return;
}

static int InsertToBlockList(struct BlockList *bl, SparseImageBlock *sib)
{
    if (bl->imageBlocks == nullptr) {
        bl->imageBlocks = sib;
        bl->imageBlocks->next = nullptr;
        return 0;
    }

    // block list is not empty, insert @sib to block list in order.
    if (bl->imageBlocks->blockIndex > sib->blockIndex) {
        // Insert @sib at front of bl
        sib->next = bl->imageBlocks;
        bl->imageBlocks = sib;
        return 0;
    }

    // @sib->blockIndex is larger. find a appropriate place
    SparseImageBlock *tmp = nullptr;
    for (tmp = bl->imageBlocks; tmp->next != nullptr; tmp = tmp->next) {
        if (tmp->next->blockIndex > sib->blockIndex) {
            break;
        }
    }

    // Insert @sib to the appropriate place.
    if (tmp->next == nullptr) {
        tmp->next = sib;
    } else {
        sib->next = tmp->next;
        tmp->next = sib;
    }

    // adjacent block may have the same property, merge them.
    // 1. merge sib with next one
    MergeBlocks(bl->blockSize, sib, sib->next);
    MergeBlocks(bl->blockSize, tmp, sib);
    return 0;
}

int AddRawDataToSparseImage(struct BlockList *bl, void *buffer, int64_t size, uint32_t blockIndex)
{
    SparseImageBlock *sib = static_cast<SparseImageBlock *>(calloc(1, sizeof(SparseImageBlock)));
    if (sib == nullptr) {
        LOG(ERROR) << "SparseBlock: Failed to allocate memory for sparse block";
        return -ENOMEM;
    }
    sib->blockIndex = blockIndex;
    sib->size = static_cast<unsigned int>(size); // maybe the size should be unsigned int
    sib->type = BLOCK_TYPE_DATA;
    sib->data.data = buffer;
    sib->next = nullptr;

    // insert into block list.
    return InsertToBlockList(bl, sib);
}

int AddFillValueToSparseImage(struct BlockList *bl, uint32_t value, int64_t size, uint32_t blockIndex)
{
    SparseImageBlock *sib = static_cast<SparseImageBlock *>(calloc(1, sizeof(SparseImageBlock)));
    if (sib == nullptr) {
        LOG(ERROR) << "SparseBlock: Failed to allocate memory for sparse block";
        return -ENOMEM;
    }

    sib->blockIndex = blockIndex;
    sib->size = static_cast<unsigned int>(size);
    sib->type = BLOCK_TYPE_FILL;
    sib->fill.value = value;
    sib->next = nullptr;

    return InsertToBlockList(bl, sib);
}
} // namespace updater
