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
#ifndef PKG_UTILS_H
#define PKG_UTILS_H

#include <cstring>
#include <iostream>
#include "log.h"
#include "pkg_manager.h"
#include "securec.h"

namespace hpackage {
#ifndef EOK
#define EOK 0
#endif

#define UNUSED(x) (void)(x)

std::string GetCurrPath();
size_t GetFileSize(const std::string &fileName);
std::string GetFilePath(const std::string &fileName);
std::string GetName(const std::string &filePath);
int32_t CheckFile(const std::string &fileName);
uint8_t *MapMemory(const std::string &fileName, size_t size);
void ReleaseMemory(uint8_t *memMap, size_t size);
void ExtraTimeAndDate(time_t when, uint16_t &date, uint16_t &time);
std::string ConvertShaHex(const std::vector<uint8_t> &shaDigest);

// 读取或写入buffer
uint32_t ReadLE32(const uint8_t *buff);
void WriteLE32(uint8_t *buff, uint32_t value);
uint16_t ReadLE16(const uint8_t *buff);
void WriteLE16(uint8_t *buff, uint16_t value);
uint64_t ReadLE64(const uint8_t *buff);

#define PKG_LOGE(format, ...) Logger(updater::ERROR, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define PKG_LOGI(format, ...) Logger(updater::INFO, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)
#define PKG_LOGW(format, ...) Logger(updater::WARNING, (__FILE_NAME__), (__LINE__), format, ##__VA_ARGS__)

#define PKG_CHECK(retCode, exper, ...) \
    if (!(retCode)) {                  \
        PKG_LOGE(__VA_ARGS__);         \
        exper;                         \
    }

#define PKG_ONLY_CHECK(retCode, exper) \
    if (!(retCode)) {                  \
        exper;                         \
    }

#define PKG_IS_TRUE_DONE(retCode, exper) \
    if ((retCode)) {                  \
        exper;                         \
    }

enum {
    PKG_INFO_BASE = PKG_ERROR_BASE + 100,
    PKG_NOT_EXIST_ALGORITHM,
    PKG_EXIT_FILE,
    PKG_BUFFER_END,
    PKG_LZ4_FINISH,
};
} // namespace hpackage
#endif // PKG_UTILS_H