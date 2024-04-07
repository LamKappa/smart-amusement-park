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

#ifndef HKS_CRYPTO_HAL_CONFIG_H
#define HKS_CRYPTO_HAL_CONFIG_H

/* AES */
#define HKS_SUPPORT_AES_C
#define HKS_SUPPORT_AES_GENERATE_KEY
#define HKS_SUPPORT_AES_CBC_NOPADDING
#define HKS_SUPPORT_AES_CBC_PKCS7
#define HKS_SUPPORT_AES_GCM
#define HKS_SUPPORT_AES_CCM

/* BN */
#define HKS_SUPPORT_BN_C

/* ECC */
#define HKS_SUPPORT_ECC_C
#define HKS_SUPPORT_ECC_GENERATE_KEY
#define HKS_SUPPORT_ECC_GET_PUBLIC_KEY
#define HKS_SUPPORT_ECDH_C
#define HKS_SUPPORT_ECDSA_C

/* ED25519 */
#define HKS_SUPPORT_ED25519_C
#define HKS_SUPPORT_ED25519_GENERATE_KEY
#define HKS_SUPPORT_ED25519_SIGN_VERIFY
#define HKS_SUPPORT_ED2519_GET_PUBLIC_KEY

/* HASH */
#define HKS_SUPPORT_HASH_C

/* HMAC */
#define HKS_SUPPORT_HMAC_C

/* KDF */
#define HKS_SUPPORT_KDF_C
#define HKS_SUPPORT_KDF_PBKDF2
#define HKS_SUPPORT_KDF_HKDF

/* RSA */
#define HKS_SUPPORT_RSA_C
#define HKS_SUPPORT_RSA_GENERATE_KEY
#define HKS_SUPPORT_RSA_CRYPT
#define HKS_SUPPORT_RSA_SIGN_VERIFY
#define HKS_SUPPORT_RSA_GET_PUBLIC_KEY

/* X25519 */
#define HKS_SUPPORT_X25519_C
#define HKS_SUPPORT_X25519_GENERATE_KEY
#define HKS_SUPPORT_X25519_AGREE_KEY
#define HKS_SUPPORT_X25519_GET_PUBLIC_KEY
#define HKS_SUPPORT_ED25519_TO_X25519
#define HKS_SUPPORT_HASH_TO_POINT

/* HksGetKeyInfoList */
#define HKS_SUPPORT_GET_KEY_INFO_LIST

#if defined(HKS_SUPPORT_ECC_C) || defined(HKS_SUPPORT_ED25519_C) || defined(HKS_SUPPORT_RSA_C)
#define HKS_SUPPORT_SIGN_VERIFY
#endif

#if defined(HKS_SUPPORT_AES_C) || defined(HKS_SUPPORT_RSA_C)
#define HKS_SUPPORT_CIPHER
#endif

#if defined(HKS_SUPPORT_ECC_C) || defined(HKS_SUPPORT_RSA_C) || \
    defined(HKS_SUPPORT_ED25519_C) || defined(HKS_SUPPORT_X25519_C) || \
    defined(HKS_SUPPORT_AES_C)
#define HKS_SUPPORT_IMPORT
#endif

#if defined(HKS_SUPPORT_ECC_C) || defined(HKS_SUPPORT_RSA_C) || \
    defined(HKS_SUPPORT_ED25519_C) || defined(HKS_SUPPORT_X25519_C)
#define HKS_SUPPORT_EXPORT
#endif

#define HKS_SUPPORT_GENERATE_KEY
#define HKS_SUPPORT_GENERATE_RANDOM
#define HKS_SUPPORT_AGREE_KEY
#define HKS_SUPPORT_DERIVE_KEY
#define HKS_SUPPORT_MAC
#define HKS_SUPPORT_HASH
#define HKS_SUPPORT_BN_EXP_MOD

#endif

