/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "ace_mem_base.h"
#include "js_config.h"
#include "securec.h"
#include "string_util.h"

namespace OHOS {
namespace ACELite {
char *StringUtil::Copy(const char *sequence)
{
    if (sequence == nullptr) {
        return nullptr;
    }
    size_t size = strlen(sequence);
    if (size >= UINT16_MAX) {
        return nullptr;
    }
    char *buffer = StringUtil::Malloc(size);
    if (buffer == nullptr) {
        return nullptr;
    }
    if (strncpy_s(buffer, size + 1, sequence, size) == 0) {
        return buffer;
    }
    ace_free(buffer);
    return nullptr;
}
char *StringUtil::Malloc(const uint32_t size)
{
    char *buffer = static_cast<char *>(ace_malloc(size + 1));
    if (buffer == nullptr) {
        return nullptr;
    }
    buffer[size] = '\0';
    return buffer;
}
char *StringUtil::Slice(const char *sequence, const int32_t start)
{
    return StringUtil::Slice(sequence, start, strlen(sequence));
}
char *StringUtil::Slice(const char *sequence, const int32_t start, const int32_t end)
{
    if (sequence == nullptr) {
        return nullptr;
    }
    uint32_t size = strlen(sequence);
    if (size == 0) {
        return nullptr;
    }
    int32_t startIdx = (start < 0) ? (start + size) : start;
    startIdx = (startIdx < 0) ? 0 : startIdx;
    int32_t endIdx = (end < 0) ? (end + size) : end;
    if (startIdx < endIdx || endIdx < 0) {
        return nullptr;
    }
    int32_t diffSize = endIdx - startIdx;
    if (diffSize < 0) {
        return nullptr;
    }
    char *buffer = StringUtil::Malloc(diffSize);
    if (buffer == nullptr) {
        return nullptr;
    }
    if (strncpy_s(buffer, diffSize + 1, sequence + startIdx, diffSize) == 0) {
        return buffer;
    }
    ace_free(buffer);
    return nullptr;
}
bool StringUtil::StartsWith(const char *sequence, const char *subsequence)
{
    if (sequence == nullptr) {
        return subsequence == nullptr;
    }
    if (subsequence == nullptr) {
        return true;
    }
    return strncmp(sequence, subsequence, strlen(subsequence)) == 0;
}
} // namespace ACELite
} // namespace OHOS
