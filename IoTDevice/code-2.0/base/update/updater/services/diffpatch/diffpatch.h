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

#ifndef DIFF_PATCH_H
#define DIFF_PATCH_H
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "log/log.h"
#include "patch/update_patch.h"

namespace updatepatch {
#define PATCH_LOGE(format, ...) Logger(updater::ERROR, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define PATCH_DEBUG(format, ...) Logger(updater::DEBUG, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define PATCH_LOGI(format, ...) Logger(updater::INFO, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define PATCH_LOGW(format, ...) Logger(updater::WARNING, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)

#define PATCH_CHECK(retCode, exper, ...) \
    if (!(retCode)) { \
        PATCH_LOGE(__VA_ARGS__); \
        exper; \
    }

#define PATCH_ONLY_CHECK(retCode, exper) \
    if (!(retCode)) {                  \
        exper;                         \
    }

#define PATCH_IS_TRUE_DONE(retCode, exper) \
    if ((retCode)) {                  \
        exper;                         \
    }

enum {
    PATCH_SUCCESS = 0,
	PATCH_INVALID_PARAM,
    PATCH_NEW_FILE,
    PATCH_EXCEED_LIMIT,
    PATCH_INVALID_PATCH,
};

/*
 * The imgdiff patch header looks like this:
 *
 *    "IMGDIFF2"                  (8)   [magic number and version]
 *    chunk count                 (4)
 *    for each chunk:
 *        chunk type              (4)   [CHUNK_{NORMAL, GZIP, DEFLATE, RAW}]
 *        if chunk type == CHUNK_NORMAL:
 *           source start         (8)
 *           source len           (8)
 *           bsdiff patch offset  (8)   [from start of patch file]
 *        if chunk type == CHUNK_GZIP:      (version 1 only)
 *           source start         (8)
 *           source len           (8)
 *           bsdiff patch offset  (8)   [from start of patch file]
 *           source expanded len  (8)   [size of uncompressed source]
 *           target expected len  (8)   [size of uncompressed target]
 *           gzip level           (4)
 *                method          (4)
 *                windowBits      (4)
 *                memLevel        (4)
 *                strategy        (4)
 *           gzip header len      (4)
 *           gzip header          (gzip header len)
 *           gzip footer          (8)
 *        if chunk type == CHUNK_DEFLATE:   (version 2 only)
 *           source start         (8)
 *           source len           (8)
 *           bsdiff patch offset  (8)   [from start of patch file]
 *           source expanded len  (8)   [size of uncompressed source]
 *           target expected len  (8)   [size of uncompressed target]
 *           gzip level           (4)
 *                method          (4)
 *                windowBits      (4)
 *                memLevel        (4)
 *                strategy        (4)
 *        if chunk type == CHUNK_LZ4:   (version 3 only)
 *           source start         (8)
 *           source len           (8)
 *           bsdiff patch offset  (8)   [from start of patch file]
 *           source expanded len  (8)   [size of uncompressed source]
 *           target expected len  (8)   [size of uncompressed target]
 *           gzip level           (4)
 *                method          (4)
 *                blockIndependence     (4)
 *                contentChecksumFlag   (4)
 *                blockSizeID     (4)
 *        if chunk type == RAW:             (version 2 only)
 *           target len           (4)
 *           data                 (target len)
 *
 */

/* Header is
    0	8	 "BSDIFF40"
    8	8	length of bzip2ed ctrl block
    16	8	length of bzip2ed diff block
    24	8	length of new file
*/
/* File is
    0	32	Header
    32	40	Bzip2ed ctrl block
    40	48	Bzip2ed diff block
    48	56	Bzip2ed extra block
*/

// Image patch block types
#define BLOCK_NORMAL 0
#define BLOCK_GZIP 1     // version 1 only
#define BLOCK_DEFLATE 2  // version 2 only
#define BLOCK_RAW 3      // version 2 only
#define BLOCK_LZ4 4      // version 3 only

// The gzip header size is actually variable, but we currently don't
// support gzipped data with any of the optional fields, so for now it
// will always be ten bytes.  See RFC 1952 for the definition of the
// gzip format.
static constexpr size_t GZIP_HEADER_LEN = 10;
static constexpr size_t VERSION = 2;
static constexpr unsigned short HEADER_CRC = 0x02; /* bit 1 set: CRC16 for the gzip header */
static constexpr unsigned short EXTRA_FIELD = 0x04; /* bit 2 set: extra field present */
static constexpr unsigned short ORIG_NAME = 0x08; /* bit 3 set: original file name present */
static constexpr unsigned short COMMENT = 0x10; /* bit 4 set: file comment present */
static constexpr unsigned short ENCRYPTED = 0x20; /* bit 5 set: file is encrypted */
static constexpr uint8_t SHIFT_RIGHT_FOUR_BITS = 4;

// The gzip footer size really is fixed.
static constexpr size_t GZIP_FOOTER_LEN = 8;
static constexpr size_t LZ4_HEADER_LEN = 4;
static constexpr size_t IGMDIFF_LIMIT_UNIT = 10240;

static constexpr int LZ4S_MAGIC = 0x184D2204;
static constexpr int LZ4B_MAGIC = 0x184C2102;
static constexpr int GZIP_MAGIC = 0x00088b1f;

static constexpr int PATCH_NORMAL_MIN_HEADER_LEN = 24;
static constexpr int PATCH_DEFLATE_MIN_HEADER_LEN = 60;
static constexpr int PATCH_LZ4_MIN_HEADER_LEN = 64;

static const std::string BSDIFF_MAGIC = "BSDIFF40";
static const std::string IMGDIFF_MAGIC = "IMGDIFF2";

struct PatchHeader {
    size_t srcStart = 0;
    size_t srcLength = 0;
    size_t patchOffset = 0;
    size_t expandedLen = 0;
    size_t targetSize = 0;
};

struct ControlData {
    int64_t diffLength;
    int64_t extraLength;
    int64_t offsetIncrement;
    uint8_t *diffNewStart;
    uint8_t *diffOldStart;
    uint8_t *extraNewStart;
};

struct MemMapInfo {
    uint8_t *memory;
    size_t length;
    ~MemMapInfo()
    {
        if (memory != nullptr) {
            munmap(memory, length);
        }
        memory = nullptr;
    }
};

int32_t WriteDataToFile(const std::string &fileName, const std::vector<uint8_t> &data, size_t dataSize);
int32_t PatchMapFile(const std::string &fileName, MemMapInfo &info);
std::string GeneraterBufferHash(const BlockBuffer &buffer);
std::string ConvertSha256Hex(const BlockBuffer &buffer);
} // namespace updatepatch
#endif // DIFF_PATCH_H