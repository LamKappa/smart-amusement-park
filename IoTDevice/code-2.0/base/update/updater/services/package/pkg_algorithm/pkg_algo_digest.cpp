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
#include "pkg_algo_digest.h"
#include "openssl/sha.h"
#include "pkg_algorithm.h"
#include "pkg_utils.h"
#include "zlib.h"

namespace hpackage {
size_t DigestAlgorithm::GetDigestLen(int8_t digestMethod)
{
    static size_t digestLens[PKG_DIGEST_TYPE_MAX] = {0, DIGEST_CRC_LEN, DIGEST_SHA256_LEN, DIGEST_SHA256_LEN};
    if (digestMethod < PKG_DIGEST_TYPE_MAX) {
        return digestLens[digestMethod];
    }
    return 0;
}

size_t DigestAlgorithm::GetSignatureLen(int8_t digestMethod)
{
    static size_t signatureLens[PKG_DIGEST_TYPE_MAX] = {0, 0, SIGN_SHA256_LEN, SIGN_SHA384_LEN};
    if (digestMethod < PKG_DIGEST_TYPE_MAX) {
        return signatureLens[digestMethod];
    }
    return SIGN_SHA256_LEN;
}

uint8_t DigestAlgorithm::GetDigestMethod(std::string version)
{
    return PKG_DIGEST_TYPE_SHA256;
}

int32_t Crc32Algorithm::Init()
{
    crc32_ = 0;
    return PKG_SUCCESS;
}

int32_t Crc32Algorithm::Update(const PkgBuffer &buffer, size_t size)
{
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    crc32_ = crc32(crc32_, buffer.buffer, size);
    return PKG_SUCCESS;
}

int32_t Crc32Algorithm::Final(PkgBuffer &result)
{
    PKG_CHECK(result.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    PKG_CHECK(result.length == sizeof(uint32_t), return PKG_INVALID_PARAM, "Invalid size %zu", result.length);
    WriteLE32(result.buffer, crc32_);
    return PKG_SUCCESS;
}

int32_t Crc32Algorithm::Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size)
{
    PKG_CHECK(result.buffer != nullptr && buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    PKG_CHECK(result.length == sizeof(uint32_t), return PKG_INVALID_PARAM, "Invalid size %zu", result.length);
    auto crc = reinterpret_cast<uint32_t *>(result.buffer);
    // Generate CRC32 of file.
    *crc = crc32(*crc, buffer.buffer, size);
    return PKG_SUCCESS;
}

int32_t Sha256Algorithm::Init()
{
    SHA256_Init(&sha256Ctx_);
    return PKG_SUCCESS;
}

int32_t Sha256Algorithm::Update(const PkgBuffer &buffer, size_t size)
{
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    SHA256_Update(&sha256Ctx_, buffer.buffer, size);
    return PKG_SUCCESS;
}

int32_t Sha256Algorithm::Final(PkgBuffer &result)
{
    PKG_CHECK(result.buffer != nullptr && result.length == DIGEST_SHA256_LEN,
        return PKG_INVALID_PARAM, "Param context null!");
    SHA256_Final(result.buffer, &sha256Ctx_);
    return PKG_SUCCESS;
}

int32_t Sha256Algorithm::Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size)
{
    PKG_CHECK(result.buffer != nullptr && result.length == DIGEST_SHA256_LEN,
        return PKG_INVALID_PARAM, "Param context null!");
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    SHA256_Init(&sha256Ctx_);
    SHA256_Update(&sha256Ctx_, buffer.buffer, size);
    SHA256_Final(result.buffer, &sha256Ctx_);
    return PKG_SUCCESS;
}

int32_t Sha384Algorithm::Init()
{
    SHA512_Init(&shaCtx_);
    return PKG_SUCCESS;
}

int32_t Sha384Algorithm::Update(const PkgBuffer &buffer, size_t size)
{
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    SHA384_Update(&shaCtx_, buffer.buffer, size);
    return PKG_SUCCESS;
}

int32_t Sha384Algorithm::Final(PkgBuffer &result)
{
    PKG_CHECK(result.buffer != nullptr && result.length == DIGEST_SHA384_LEN,
        return PKG_INVALID_PARAM, "Param context null!");
    SHA384_Final(result.buffer, &shaCtx_);
    return PKG_SUCCESS;
}

int32_t Sha384Algorithm::Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size)
{
    PKG_CHECK(result.buffer != nullptr && result.length == DIGEST_SHA384_LEN,
        return PKG_INVALID_PARAM, "Param context null!");
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    SHA512_Init(&shaCtx_);
    SHA384_Update(&shaCtx_, buffer.buffer, size);
    SHA384_Final(result.buffer, &shaCtx_);
    return PKG_SUCCESS;
}

DigestAlgorithm::DigestAlgorithmPtr PkgAlgorithmFactory::GetDigestAlgorithm(uint8_t type)
{
    switch (type) {
        case PKG_DIGEST_TYPE_CRC:
            return std::make_shared<Crc32Algorithm>();
        case PKG_DIGEST_TYPE_SHA256:
        case PKG_DIGEST_TYPE_SHA384:
            return std::make_shared<Sha256Algorithm>();
        default:
            return std::make_shared<DigestAlgorithm>();
            break;
    }
    return nullptr;
}
} // namespace hpackage
