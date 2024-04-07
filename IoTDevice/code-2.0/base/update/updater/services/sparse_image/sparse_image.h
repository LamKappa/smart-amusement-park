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
#ifndef UPDATER_SPARSE_IMAGE_H
#define UPDATER_SPARSE_IMAGE_H
#include <cstdint>
#include "sparse_block.h"

namespace updater {
constexpr uint32_t SPARSE_IMAGE_MAGIC = 0xED26FF3A;
constexpr uint16_t SPARSE_IMAGE_MAJOR_VERSIOIN = 0x1;
constexpr uint16_t SPARSE_IMAGE_MINOR_VERSIOIN = 0x0;

struct SparseImageHeader {
    // Magic number of sparse image is 0xed26ff3a
    uint32_t magicNumber;
    // major versoin of sparse image is 0x1
    uint16_t majorVersion;
    // minor versoin of sparse image is 0x0
    uint16_t minorVersion;
    // file header size, 28 bytes
    uint16_t fileHeaderSize;
    // chunk header size, 12 bytes
    uint16_t chunkHeaderSize;
    // block size in bytes, default is 4096 bytes
    uint32_t blockSize;
    // total blocks of non-sparse image.
    uint32_t totalBlocks;
    // total chunks of sparse image.
    uint32_t totalChunks;
    // CRC32 checksum of original data.
    uint32_t checkSum;
};

// chunk type definition
constexpr uint16_t CHUNK_TYPE_RAW = 0xCAC1;
constexpr uint16_t CHUNK_TYPE_FILL = 0xCAC2;
constexpr uint16_t CHUNK_TYPE_DONT_CARE = 0xCAC3;
constexpr uint16_t CHUNK_TYPE_CRC32 = 0xCAC4;

struct SparseImageChunkHeader {
    uint16_t type; // chunk type: raw, fill or don't care
    uint16_t reserved1;
    uint32_t chunkSize; // size in blocks of output image.
    uint32_t totalSize; // size in bytes, include chunk header and data
};

struct SparseImage {
    // size in bock
    unsigned int blockSize;
    // total size of sparse image
    int64_t totalSize;
    // block list
    struct BlockList *bl;
};
} // namespace updater
#endif // UPDATER_SPARSE_IMAGE_H
