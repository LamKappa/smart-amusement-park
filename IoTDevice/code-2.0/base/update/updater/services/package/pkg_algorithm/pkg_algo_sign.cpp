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
#include "pkg_algo_sign.h"
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include "pkg_algorithm.h"
#include "pkg_utils.h"

using namespace updater;

namespace hpackage {
#ifndef BIO_FP_READ
constexpr uint32_t BIO_FP_READ = 0x02;
#endif
int32_t SignAlgorithmRsa::SignBuffer(const PkgBuffer &buffer, std::vector<uint8_t> &signedData,
    size_t &signLen) const
{
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    BIO *in = BIO_new(BIO_s_file());
    PKG_CHECK(in != nullptr, return PKG_INVALID_PARAM, "Failed to new BIO ");

    int32_t ret = BIO_ctrl(in, BIO_C_SET_FILENAME, BIO_CLOSE | BIO_FP_READ, const_cast<char*>(keyName_.c_str()));
    PKG_CHECK(ret == 1, BIO_free(in);
        return PKG_INVALID_PARAM, "Failed to BIO_read_filename ret %d %s", ret, keyName_.c_str());

    RSA *rsa = PEM_read_bio_RSAPrivateKey(in, nullptr, nullptr, nullptr);
    BIO_free(in);
    PKG_CHECK(rsa != nullptr, return PKG_INVALID_SIGNATURE, "Failed to PEM_read_bio_RSAPrivateKey ");

    // Adjust key size
    uint32_t size = RSA_size(rsa);
    signedData.resize(size);
    ret = 0;
    if (digestMethod_ == PKG_DIGEST_TYPE_SHA256) {
        ret = RSA_sign(NID_sha256, buffer.buffer, buffer.length, signedData.data(), &size, rsa);
    } else if (digestMethod_ == PKG_DIGEST_TYPE_SHA384) {
        ret = RSA_sign(NID_sha384, buffer.buffer, buffer.length, signedData.data(), &size, rsa);
    }
    signLen = size;
    RSA_free(rsa);
    return ((ret == 1) ? PKG_SUCCESS : PKG_INVALID_SIGNATURE);
}

int32_t SignAlgorithmEcc::SignBuffer(const PkgBuffer &buffer, std::vector<uint8_t> &signedData,
    size_t &signLen) const
{
    PKG_CHECK(buffer.buffer != nullptr, return PKG_INVALID_PARAM, "Param null!");
    BIO *in = BIO_new(BIO_s_file());
    PKG_CHECK(in != nullptr, return PKG_INVALID_PARAM, "Failed to new BIO ");

    int ret = BIO_ctrl(in, BIO_C_SET_FILENAME, BIO_CLOSE | BIO_FP_READ, const_cast<char*>(keyName_.c_str()));
    PKG_CHECK(ret == 1, BIO_free(in);
        return PKG_INVALID_PARAM, "Failed to BIO_read_filename ret %d %s", ret, keyName_.c_str());

    EC_KEY *ecKey = PEM_read_bio_ECPrivateKey(in, nullptr, nullptr, nullptr);
    BIO_free(in);
    PKG_CHECK(ecKey != nullptr, return PKG_INVALID_PARAM, "Failed to PEM_read_bio_ECPrivateKey %s", keyName_.c_str());

    // Adjust key size
    uint32_t size = ECDSA_size(ecKey);
    signedData.resize(size + sizeof(uint32_t));
    ret = ECDSA_sign(0, buffer.buffer, buffer.length, signedData.data() + sizeof(uint32_t), &size, ecKey);
    WriteLE32(signedData.data(), size);
    signLen = size + sizeof(uint32_t);
    EC_KEY_free(ecKey);
    return ((ret == 1) ? PKG_SUCCESS : PKG_INVALID_SIGNATURE);
}

bool VerifyAlgorithm::CheckRsaKey(const RSA *rsakey) const
{
    const BIGNUM *rsaN = nullptr;
    const BIGNUM *rsaE = nullptr;
    RSA_get0_key(rsakey, &rsaN, &rsaE, nullptr);
    auto modulusbits = BN_num_bits(rsaN);
    PKG_CHECK(!(modulusbits != 2048), return false,
        "Modulus should be 2048 bits long, actual:%d\n ", modulusbits);
    BN_ULONG exponent = BN_get_word(rsaE);
    PKG_CHECK(!(exponent != 3 && exponent != 65537),
        return false, "Public exponent should be 3 or 65537, actual:%d \n", exponent);
    return true;
}

bool VerifyAlgorithm::CheckEccKey(const EC_KEY *eccKey) const
{
    const EC_GROUP *eccgroup = EC_KEY_get0_group(eccKey);
    PKG_CHECK(eccgroup != nullptr, return false, "Failed to get the ec_group from the ecKey");
    auto eccdegree = EC_GROUP_get_degree(eccgroup);
    PKG_CHECK(eccdegree == 256, return false,
        "Field size of the ec key should be 256 bits long, actual:%d ", eccdegree);
    return true;
}

bool VerifyAlgorithm::LoadPubKey(const std::string &keyfile, struct CertKeySt &certs) const
{
    BIO *certbio = BIO_new_file(keyfile.c_str(), "r");
    PKG_CHECK(certbio != nullptr, return false, "Failed to create BIO");

    X509 *rcert = PEM_read_bio_X509(certbio, nullptr, 0, nullptr);
    BIO_free(certbio);
    PKG_CHECK(rcert != nullptr, return false, "Failed to read x509 certificate ");
    int nid = X509_get_signature_nid(rcert);
    PKG_CHECK((nid == NID_sha256WithRSAEncryption || nid == NID_ecdsa_with_SHA256), X509_free(rcert);
        return false, "Unrecognized nid %d", nid);

    certs.hashLen = SHA256_DIGEST_LENGTH;
    EVP_PKEY *pubKey = X509_get_pubkey(rcert);
    PKG_CHECK(pubKey != nullptr, X509_free(rcert);
        return false, "Failed to extract the public key from x509 certificate %s", keyfile.c_str());

    bool ret = false;
    int keyType = EVP_PKEY_id(pubKey);
    if (keyType == EVP_PKEY_RSA) {
        certs.keyType = KEY_TYPE_RSA;
        certs.rsa = EVP_PKEY_get1_RSA(pubKey);
        PKG_CHECK(certs.rsa != nullptr, X509_free(rcert); return false, "Failed to get rsa");
        ret = CheckRsaKey(certs.rsa);
        PKG_CHECK(ret == true, RSA_free(certs.rsa); X509_free(rcert); return false, "Check rsa key failed");
    }
    if (keyType == EVP_PKEY_EC) {
        certs.keyType = KEY_TYPE_EC;
        certs.ecKey = EVP_PKEY_get1_EC_KEY(pubKey);
        PKG_CHECK(certs.ecKey != nullptr, X509_free(rcert); return false, "Failed to get ec key");
        ret = CheckEccKey(certs.ecKey);
        PKG_CHECK(ret == true, EC_KEY_free(certs.ecKey); X509_free(rcert); return false, "Check ec key failed");
    }
    EVP_PKEY_free(pubKey);
    certs.cert = rcert;
    return ret;
}

int32_t VerifyAlgorithm::VerifyBuffer(const std::vector<uint8_t> &digest, const std::vector<uint8_t> &signature)
{
    struct CertKeySt certs {};
    bool isValid = LoadPubKey(keyName_, certs);
    PKG_CHECK(isValid, return PKG_INVALID_SIGNATURE, "Failed to load public key");

    int hashNid = NID_sha1;
    if (certs.hashLen == SHA256_DIGEST_LENGTH) {
        hashNid = NID_sha256;
    }
    int ret = 0;
    if (certs.keyType == KEY_TYPE_RSA) {
        ret = RSA_verify(hashNid, digest.data(), digest.size(), signature.data(), signature.size(), certs.rsa);
        RSA_free(certs.rsa);
    } else if (certs.keyType == KEY_TYPE_EC && certs.hashLen == SHA256_DIGEST_LENGTH) {
        int dataLen = ReadLE32(signature.data());
        ret = ECDSA_verify(0, digest.data(), digest.size(), signature.data() + sizeof(uint32_t), dataLen, certs.ecKey);
        EC_KEY_free(certs.ecKey);
    }
    X509_free(certs.cert);
    return ((ret == 1) ? PKG_SUCCESS : PKG_INVALID_SIGNATURE);
}

SignAlgorithm::SignAlgorithmPtr PkgAlgorithmFactory::GetSignAlgorithm(const std::string &path,
    uint8_t signMethod, uint8_t type)
{
    switch (signMethod) {
        case PKG_SIGN_METHOD_RSA:
            return std::make_shared<SignAlgorithmRsa>(path, type);
        case PKG_SIGN_METHOD_ECDSA:
            return std::make_shared<SignAlgorithmEcc>(path, type);
        default:
            break;
    }
    return nullptr;
}

SignAlgorithm::SignAlgorithmPtr PkgAlgorithmFactory::GetVerifyAlgorithm(const std::string &path, uint8_t type)
{
    return std::make_shared<VerifyAlgorithm>(path, type);
}
} // namespace hpackage
