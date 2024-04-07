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
#ifndef PKG_ALGORITHM_DIGEST_H
#define PKG_ALGORITHM_DIGEST_H

#include "openssl/sha.h"
#include "pkg_utils.h"

namespace hpackage {
constexpr uint32_t DIGEST_CRC_LEN = 4;
constexpr uint32_t DIGEST_SHA256_LEN = 32;
constexpr uint32_t DIGEST_SHA384_LEN = 64;
constexpr uint32_t SIGN_SHA256_LEN = 256;
constexpr uint32_t SIGN_SHA384_LEN = 384;
constexpr uint32_t SIGN_TOTAL_LEN = 384 + 256;

class DigestAlgorithm {
public:
    using DigestAlgorithmPtr = std::shared_ptr<DigestAlgorithm>;

    DigestAlgorithm() {}

    virtual ~DigestAlgorithm() {}

    virtual int32_t Init()
    {
        return 0;
    };

    virtual int32_t Update(const PkgBuffer &buffer, size_t size)
    {
        UNUSED(buffer);
        return PKG_SUCCESS;
    }

    virtual int32_t Final(PkgBuffer &buffer)
    {
        (void)buffer;
        return PKG_SUCCESS;
    }

    virtual int32_t Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size)
    {
        UNUSED(result);
        UNUSED(buffer);
        return PKG_SUCCESS;
    }

    static size_t GetDigestLen(int8_t digestMethod);
    static size_t GetSignatureLen(int8_t digestMethod);
    static uint8_t GetDigestMethod(std::string version);
};

class Crc32Algorithm : public DigestAlgorithm {
public:
    Crc32Algorithm() {}

    ~Crc32Algorithm() override {}

    int32_t Init() override;

    int32_t Update(const PkgBuffer &buffer, size_t size) override;

    int32_t Final(PkgBuffer &result) override;

    int32_t Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size) override;

private:
    uint32_t crc32_ { 0 };
};

class Sha256Algorithm : public DigestAlgorithm {
public:
    Sha256Algorithm() {}

    ~Sha256Algorithm() override {}

    int32_t Init() override;

    int32_t Update(const PkgBuffer &buffer, size_t size) override;

    int32_t Final(PkgBuffer &result) override;

    int32_t Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size) override;

private:
    SHA256_CTX sha256Ctx_ {};
};

class Sha384Algorithm : public DigestAlgorithm {
public:
    Sha384Algorithm() {}

    ~Sha384Algorithm() override {}

    int32_t Init() override;

    int32_t Update(const PkgBuffer &buffer, size_t size) override;

    int32_t Final(PkgBuffer &result) override;

    int32_t Calculate(PkgBuffer &result, const PkgBuffer &buffer, size_t size) override;

private:
    SHA512_CTX shaCtx_ {};
};
} // namespace hpackage
#endif
