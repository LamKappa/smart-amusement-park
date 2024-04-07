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

#include "str_util.h"
#include "securec.h"
#include "types.h"

namespace OHOS {
namespace I18N {
std::string Replace(std::string &content, const int index, const char *str)
{
    if ((index < 0) || (static_cast<unsigned int>(index) > strlen(content.data()))) {
        return content;
    }
    return content.replace(index, 1, str);
}

int ReplaceAndCountOff(std::string &content, const int index, const char *sign, const int off)
{
    int signLen = strlen(sign);
    if ((index < 0) || (static_cast<unsigned int>(index) > strlen(content.data()))) {
        return off;
    }
    content = content.replace(index, 1, sign);
    int newOff = off + signLen - 1;
    return newOff;
}

void ArrayCopy(std::string *target, const int targetSize, const std::string *source, const int sourceSize)
{
    if (target == nullptr || source == nullptr || (sourceSize > targetSize)) {
        return;
    }
    for (int i = 0; i < sourceSize; i++) {
        target[i] = source[i];
    }
}

char *NewArrayAndCopy(const char *source, const int len)
{
    if ((source == nullptr) || (len < 0)) {
        return nullptr;
    }
    char *out = new(std::nothrow) char[len + 1];
    if (out == nullptr) {
        return nullptr;
    }
    errno_t rc = strcpy_s(out, len + 1, source);
    if (rc != EOK) {
        delete[] out;
        out = nullptr;
        return nullptr;
    }
    out[len] = '\0';
    return out;
}

bool CleanCharArray(char *target, const int len)
{
    errno_t ret = memset_s(target, len, 0, len);
    if (ret != EOK) {
        return false;
    }
    return true;
}

int LenCharArray(const char *target)
{
    if (target == nullptr) {
        return 0;
    }
    return strlen(target);
}

void Split(const std::string &src, std::string *dst, const int32_t size, const char &sep)
{
    if (dst == nullptr || size <= 0) {
        return;
    }
    int32_t current = 0;

    std::string::size_type begin = 0;
    std::string::size_type end = 0;
    while (end < src.size()) {
        if (src[end] != sep) {
            ++end;
        } else {
            dst[current] = std::string(src, begin, end - begin);
            ++end;
            begin = end;
            ++current;
            if (current >= size) {
                return;
            }
        }
    }
    if ((begin != end) && (current < size)) {
        dst[current] = std::string(src, begin, end - begin);
    }
}

bool CompareLocaleItem(const char *item, const char* other)
{
    if (item == nullptr) {
        if (other != nullptr) {
            return false;
        }
    } else {
        if (other == nullptr || strcmp(item, other) != 0) {
            return false;
        }
    }
    return true;
}
} // I18N
} // OHOS
