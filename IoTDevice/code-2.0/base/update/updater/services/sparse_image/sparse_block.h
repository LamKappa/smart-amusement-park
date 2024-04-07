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

#ifndef UPDATER_SPARSE_BLOCK_H
#define UPDATER_SPARSE_BLOCK_H
#include <cstdint>

namespace updater {
enum BLOCKTYPE {
    BLOCK_TYPE_DATA,
    BLOCK_TYPE_FILE,
    BLOCK_TYPE_FD,
    BLOCK_TYPE_FILL,
};

// Sparse image combined by many
// blocks. each block has different type
struct SparseImageBlock {
    unsigned int blockIndex;
    unsigned int size;
    BLOCKTYPE type;
    union {
        struct {
            void *data;
        } data;
        struct {
            char *fileName;
            int64_t offset;
        } file;
        struct {
            int fd;
            int64_t offset;
        } fd;
        struct {
            uint32_t value;
        } fill;
    };
    SparseImageBlock *next;
};

struct BlockList {
    SparseImageBlock *imageBlocks;
    unsigned int blockSize;
};

void DeallocateBlockList(struct BlockList *bl);
int AddFillValueToSparseImage(struct BlockList *bl, uint32_t value, int64_t size, uint32_t blockIndex);
int AddRawDataToSparseImage(struct BlockList *bl, void *buffer, int64_t size, uint32_t blockIndex);
} // namespace updater
#endif 

