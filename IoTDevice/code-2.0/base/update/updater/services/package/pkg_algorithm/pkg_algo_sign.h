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
#ifndef PKG_ALGORITHM_SIGN_H
#define PKG_ALGORITHM_SIGN_H

#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include "pkg_utils.h"

namespace hpackage {
enum KEYTYPE {
    KEY_TYPE_RSA,
    KEY_TYPE_EC,
};

struct CertKeySt {
    X509 *cert;
    int hashLen;
    KEYTYPE keyType;
    RSA *rsa;
    EC_KEY *ecKey;
};

class SignAlgorithm {
public:
    using SignAlgorithmPtr = std::shared_ptr<SignAlgorithm>;

    SignAlgorithm(std::string keyPath, uint8_t digestMethod) : keyName_(keyPath), digestMethod_(digestMethod) {}

    virtual ~SignAlgorithm() {}

    virtual int32_t SignBuffer(const PkgBuffer &buffer, std::vector<uint8_t> &sign, size_t &signLen) const = 0;
    virtual int32_t VerifyBuffer(const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature) = 0;

protected:
    std::string keyName_ {};
    uint8_t digestMethod_ = PKG_DIGEST_TYPE_SHA256;
};

class VerifyAlgorithm : public SignAlgorithm {
public:
    VerifyAlgorithm(std::string keyPath, uint8_t digestMethod) : SignAlgorithm(keyPath, digestMethod) {}

    ~VerifyAlgorithm() override {}

    int32_t SignBuffer(const PkgBuffer &buffer, std::vector<uint8_t> &sign, size_t &signLen) const override
    {
        UNUSED(buffer);
        UNUSED(sign);
        UNUSED(signLen);
        return PKG_INVALID_SIGNATURE;
    }

    int32_t VerifyBuffer(const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature) override;

private:
    bool CheckEccKey(const EC_KEY *eccKey) const;

    bool CheckRsaKey(const RSA *rsakey) const;

    bool LoadPubKey(const std::string &filename, struct CertKeySt &certs) const;
};

class SignAlgorithmRsa : public SignAlgorithm {
public:
    SignAlgorithmRsa(const std::string &keyPath, uint8_t digestMethod) : SignAlgorithm(keyPath, digestMethod) {}

    ~SignAlgorithmRsa() override {}

    int32_t SignBuffer(const PkgBuffer &buffer, std::vector<uint8_t> &sign, size_t &signLen) const override;

    int32_t VerifyBuffer(const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature) override
    {
        UNUSED(digest);
        UNUSED(signature);
        return PKG_INVALID_SIGNATURE;
    }
};

class SignAlgorithmEcc : public SignAlgorithm {
public:
    SignAlgorithmEcc(const std::string &keyPath, uint8_t digestMethod) : SignAlgorithm(keyPath, digestMethod) {}

    ~SignAlgorithmEcc() override {}

    int32_t SignBuffer(const PkgBuffer &buffer, std::vector<uint8_t> &sign, size_t &signLen) const override;

    int32_t VerifyBuffer(const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature) override
    {
        UNUSED(digest);
        UNUSED(signature);
        return PKG_INVALID_SIGNATURE;
    }
};
} // namespace hpackage
#endif
