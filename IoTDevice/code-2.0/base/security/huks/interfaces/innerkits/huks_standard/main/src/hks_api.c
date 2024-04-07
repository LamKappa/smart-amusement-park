/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "hks_api.h"
#include "hks_client_service_ipc.h"

#ifdef HKS_HAL_ENGINE_CONFIG_FILE
#include HKS_HAL_ENGINE_CONFIG_FILE
#else
#include "hks_crypto_hal_config.h"
#endif

#include "hks_local_engine.h"
#include "hks_param.h"

HKS_API_EXPORT int32_t HksGetSdkVersion(struct HksBlob *sdkVersion)
{
    if ((sdkVersion == NULL) || (sdkVersion->data == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    uint32_t versionLen = strlen(HKS_SDK_VERSION);
    if (sdkVersion->size < (versionLen + 1)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (memcpy_s(sdkVersion->data, sdkVersion->size, HKS_SDK_VERSION, versionLen) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    sdkVersion->data[versionLen] = '\0';
    sdkVersion->size = versionLen;
    return HKS_SUCCESS;
}

HKS_API_EXPORT int32_t HksInitialize(void)
{
    return HksClientInitialize();
}

HKS_API_EXPORT int32_t HksGenerateKey(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, struct HksParamSet *paramSetOut)
{
#ifdef HKS_SUPPORT_GENERATE_KEY
    struct HksParam *storageFlag = NULL;
    int32_t ret = HksGetParam(paramSetIn, HKS_TAG_KEY_STORAGE_FLAG, &storageFlag);
    if ((ret == HKS_SUCCESS) && (storageFlag->uint32Param == HKS_STORAGE_TEMP)) {
        if ((paramSetIn == NULL) || (paramSetOut == NULL)) {
            return HKS_ERROR_NULL_POINTER;
        }
        return HksLocalGenerateKey(paramSetIn, paramSetOut);
    }

    if ((paramSetIn == NULL) || (keyAlias == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientGenerateKey(keyAlias, paramSetIn, paramSetOut);
#else
    (void)keyAlias;
    (void)paramSetIn;
    (void)paramSetOut;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksImportKey(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *key)
{
#ifdef HKS_SUPPORT_IMPORT
    if ((keyAlias == NULL) || (paramSet == NULL) || (key == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientImportKey(keyAlias, paramSet, key);
#else
    (void)keyAlias;
    (void)paramSet;
    (void)key;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksExportPublicKey(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *key)
{
#ifdef HKS_SUPPORT_EXPORT
    if ((keyAlias == NULL) || (key == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientExportPublicKey(keyAlias, paramSet, key);
#else
    (void)keyAlias;
    (void)paramSet;
    (void)key;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDeleteKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
    if (keyAlias == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientDeleteKey(keyAlias, paramSet);
}

HKS_API_EXPORT int32_t HksGetKeyParamSet(const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, struct HksParamSet *paramSetOut)
{
    (void)paramSetIn;
    if ((keyAlias == NULL) || (paramSetOut == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientGetKeyParamSet(keyAlias, paramSetOut);
}

HKS_API_EXPORT int32_t HksKeyExist(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet)
{
    if (keyAlias == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientKeyExist(keyAlias, paramSet);
}

HKS_API_EXPORT int32_t HksGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
#ifdef HKS_SUPPORT_GENERATE_RANDOM
    if (random == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientGenerateRandom(random, paramSet);
#else
    (void)paramSet;
    (void)random;
#endif
}

HKS_API_EXPORT int32_t HksSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
#ifdef HKS_SUPPORT_SIGN_VERIFY
    if ((key == NULL) || (paramSet == NULL) || (srcData == NULL) || (signature == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientSign(key, paramSet, srcData, signature);
#else
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)signature;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
#ifdef HKS_SUPPORT_SIGN_VERIFY
    if ((key == NULL) || (paramSet == NULL) || (srcData == NULL) || (signature == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalVerify(key, paramSet, srcData, signature);
    }
    return HksClientVerify(key, paramSet, srcData, signature);
#else
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)signature;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
#ifdef HKS_SUPPORT_CIPHER
    if ((key == NULL) || (paramSet == NULL) || (plainText == NULL) || (cipherText == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalEncrypt(key, paramSet, plainText, cipherText);
    }
    return HksClientEncrypt(key, paramSet, plainText, cipherText);
#else
    (void)key;
    (void)paramSet;
    (void)plainText;
    (void)cipherText;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
#ifdef HKS_SUPPORT_CIPHER
    if ((key == NULL) || (paramSet == NULL) || (cipherText == NULL) || (plainText == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalDecrypt(key, paramSet, cipherText, plainText);
    }
    return HksClientDecrypt(key, paramSet, cipherText, plainText);
#else
    (void)key;
    (void)paramSet;
    (void)plainText;
    (void)cipherText;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
#ifdef HKS_SUPPORT_AGREE_KEY
    if ((paramSet == NULL) || (privateKey == NULL) || (peerPublicKey == NULL) || (agreedKey == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
    }
    return HksClientAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
#else
    (void)paramSet;
    (void)privateKey;
    (void)peerPublicKey;
    (void)agreedKey;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    struct HksBlob *derivedKey)
{
#ifdef HKS_SUPPORT_DERIVE_KEY
    if ((paramSet == NULL) || (mainKey == NULL) || (derivedKey == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalDeriveKey(paramSet, mainKey, derivedKey);
    }
    return HksClientDeriveKey(paramSet, mainKey, derivedKey);
#else
    (void)paramSet;
    (void)mainKey;
    (void)derivedKey;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
#ifdef HKS_SUPPORT_MAC
    if ((key == NULL) || (paramSet == NULL) || (srcData == NULL) || (mac == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *isKeyAlias = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IS_KEY_ALIAS, &isKeyAlias);
    if ((ret == HKS_SUCCESS) && (!isKeyAlias->boolParam)) {
        return HksLocalMac(key, paramSet, srcData, mac);
    }
    return HksClientMac(key, paramSet, srcData, mac);
#else
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)mac;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksHash(const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *hash)
{
#ifdef HKS_SUPPORT_HASH
    if ((paramSet == NULL) || (srcData == NULL) || (hash == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksLocalHash(paramSet, srcData, hash);
#else
    (void)paramSet;
    (void)srcData;
    (void)hash;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGetKeyInfoList(const struct HksParamSet *paramSet,
    struct HksKeyInfo *keyInfoList, uint32_t *listCount)
{
#ifdef HKS_SUPPORT_GET_KEY_INFO_LIST
    (void)paramSet;
    if ((keyInfoList == NULL) || (listCount == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientGetKeyInfoList(keyInfoList, listCount);
#else
    (void)paramSet;
    (void)keyInfoList;
    (void)listCount;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksAttestKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain)
{
#ifdef HKS_SUPPORT_ATTEST_KEY
    if ((keyAlias == NULL) || (paramSet == NULL) || (certChain == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientAttestKey(keyAlias, paramSet, certChain);
#else
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksGetCertificateChain(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain)
{
#ifdef HKS_SUPPORT_ATTEST_KEY
    if ((keyAlias == NULL) || (paramSet == NULL) || (certChain == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientGetCertificateChain(keyAlias, paramSet, certChain);
#else
    (void)keyAlias;
    (void)paramSet;
    (void)certChain;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksWrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *wrappedData)
{
#ifdef HKS_SUPPORT_WRAP_KEY
    if ((keyAlias == NULL) || (targetKeyAlias == NULL) || (paramSet == NULL) || (wrappedData == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientWrapKey(keyAlias, targetKeyAlias, paramSet, wrappedData);
#else
    (void)keyAlias;
    (void)targetKeyAlias;
    (void)paramSet;
    (void)wrappedData;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksUnwrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksBlob *wrappedData, const struct HksParamSet *paramSet)
{
#ifdef HKS_SUPPORT_UNWRAP_KEY
    if ((keyAlias == NULL) || (targetKeyAlias == NULL) || (wrappedData == NULL) || (paramSet == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksClientUnwrapKey(keyAlias, targetKeyAlias, wrappedData, paramSet);
#else
    (void)keyAlias;
    (void)targetKeyAlias;
    (void)paramSet;
    (void)wrappedData;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

HKS_API_EXPORT int32_t HksBnExpMod(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n)
{
#ifdef HKS_SUPPORT_BN_EXP_MOD
    if ((x == NULL) || (a == NULL) || (e == NULL) || (n == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }
    return HksLocalBnExpMod(x, a, e, n);
#else
    (void)x;
    (void)a;
    (void)e;
    (void)n;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}
