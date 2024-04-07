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

#include "diffpatch.h"
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "openssl/sha.h"

namespace updatepatch {
int32_t WriteDataToFile(const std::string &fileName, const std::vector<uint8_t> &data, size_t dataSize)
{
    std::ofstream patchFile(fileName, std::ios::out | std::ios::binary);
    PATCH_CHECK(patchFile, return -1, "Failed to open %s", fileName.c_str());
    patchFile.write(reinterpret_cast<const char*>(data.data()), dataSize);
    patchFile.close();
    return PATCH_SUCCESS;
}

int32_t PatchMapFile(const std::string &fileName, MemMapInfo &info)
{
    int32_t file = open(fileName.c_str(), O_RDONLY);
    PATCH_CHECK(file >= 0, return -1, "Failed to open file %s", fileName.c_str());
    struct stat st {};
    int32_t ret = fstat(file, &st);
    PATCH_CHECK(ret >= 0, close(file); return ret, "Failed to fstat");

    info.memory = static_cast<uint8_t*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, file, 0));
    PATCH_CHECK(info.memory != nullptr, close(file); return -1, "Failed to memory map");
    info.length = st.st_size;
    close(file);
    return PATCH_SUCCESS;
}

std::string GeneraterBufferHash(const BlockBuffer &buffer)
{
    SHA256_CTX sha256Ctx;
    SHA256_Init(&sha256Ctx);
    SHA256_Update(&sha256Ctx, buffer.buffer, buffer.length);
    std::vector<uint8_t> digest(SHA256_DIGEST_LENGTH);
    SHA256_Final(digest.data(), &sha256Ctx);
    return ConvertSha256Hex({
            digest.data(), SHA256_DIGEST_LENGTH
        });
}

std::string ConvertSha256Hex(const BlockBuffer &buffer)
{
    const std::string hexChars = "0123456789abcdef";
    std::string haxSha256 = "";
    unsigned int c;
    for (size_t i = 0; i < buffer.length; ++i) {
        auto d = buffer.buffer[i];
        c = (d >> SHIFT_RIGHT_FOUR_BITS) & 0xf;     // last 4 bits
        haxSha256.push_back(hexChars[c]);
        haxSha256.push_back(hexChars[d & 0xf]);
    }
    return haxSha256;
}
} // namespace updatepatch