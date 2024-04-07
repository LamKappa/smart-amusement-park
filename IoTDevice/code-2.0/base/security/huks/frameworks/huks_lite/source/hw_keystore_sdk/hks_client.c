/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "hks_client.h"
#include "securec.h"

#include "common/hks_common.h"
#include "common/hks_log_utils.h"
#include "hks_errno.h"
#include "hks_file_api.h"
#include "hks_hardware_api.h"
#include "hks_types.h"
#include "soft_service/hks_service.h"

#include "hks_access.h"

/* AES-GCM encrypt IV minimum size,96bit=12bytes */
#define HKS_AES_GCM_MIN_IV_LENGTH 12

void hks_get_sdk_version(struct hks_blob *sdk_version)
{
    if (sdk_version == NULL)
        return;

    const size_t version_len = strlen(HKS_SDK_VERSION);
    if ((sdk_version->data != NULL) && (sdk_version->size > version_len)) {
        if (memcpy_s(sdk_version->data, sdk_version->size, HKS_SDK_VERSION, version_len) != EOK) {
            log_error("memcpy_s fail");
            return;
        }
        sdk_version->data[version_len] = '\0';
        sdk_version->size = (uint32_t)version_len + 1;
    }
}

HKS_DLL_API_PUBLIC int32_t hks_generate_key(const struct hks_blob *key_alias,
    const struct hks_key_param *key_param)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    HKS_TRACE_IN;
    hks_if_status_error_return(hks_is_valid_auth_id(key_param));
    hks_if_status_error_return(hks_is_valid_alias(key_alias));

    if (key_param->key_type != HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519)
        return HKS_ERROR_NOT_SUPPORTED;
    return hks_access_generate_key(key_alias, key_param);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_generate_asymmetric_key(
    const struct hks_key_param *key_param, struct hks_blob *pri_key,
    struct hks_blob *pub_key)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    HKS_TRACE_IN;
    hks_if_true_return_error((key_param == NULL), HKS_ERROR_NULL_POINTER);
    if (key_param->key_type != HKS_KEY_TYPE_ECC_KEYPAIR_CURVE25519)
        return HKS_ERROR_NOT_SUPPORTED;
    if (key_param->key_mode != hks_alg_ecdh(HKS_ALG_SELECT_RAW))
        return HKS_ERROR_NOT_SUPPORTED;
    if ((pri_key == NULL) || (pri_key->data == NULL) || (pub_key == NULL) ||
        (pub_key->data == NULL))
        return HKS_ERROR_NULL_POINTER;
    return hks_access_generate_key_ex(key_param, pri_key, pub_key);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_import_public_key(
    const struct hks_blob *key_alias,
    const struct hks_key_param *key_param, const struct hks_blob *key)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    HKS_TRACE_IN;
    hks_if_status_error_return(hks_is_valid_auth_id(key_param));
    int32_t status = hks_is_valid_alias(key_alias);

    if (status != HKS_STATUS_OK)
        return status;
    if (key_param->key_type != HKS_KEY_TYPE_EDDSA_PUBLIC_KEY_ED25519)
        return HKS_ERROR_NOT_SUPPORTED;
    hks_if_true_return_error((key == NULL), HKS_ERROR_NULL_POINTER);
    if ((key->data == NULL) || (key->size != CRYPTO_PUBLIC_KEY_BYTES))
        return HKS_ERROR_INVALID_PUBLIC_KEY;

    return hks_access_import_key(key_alias, key_param, key);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_export_public_key(
    const struct hks_blob *key_alias, struct hks_blob *key)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    int32_t status = hks_is_valid_alias(key_alias);

    if (status != HKS_STATUS_OK)
        return status;
    if (key == NULL)
        return HKS_ERROR_NULL_POINTER;
    if ((key->data == NULL) || (key->size < CRYPTO_PUBLIC_KEY_BYTES))
        return HKS_ERROR_BUF_TOO_SMALL;
    return hks_access_export_key(key_alias, key);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_delete_key(const struct hks_blob *key_alias)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    /* to add log here track return value */
    int32_t status = hks_is_valid_alias(key_alias);

    if (status != HKS_STATUS_OK)
        return status;
    return hks_access_delete_key(key_alias);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_get_key_param(const struct hks_blob *key_alias,
    struct hks_key_param *key_param)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    int32_t status = hks_is_valid_alias(key_alias);

    if (status != HKS_STATUS_OK)
        return status;
    if (key_param == NULL)
        return HKS_ERROR_NULL_POINTER;
    return hks_access_get_key_param(key_alias, key_param);
#endif
}

/*
 * For current interface, if status is HKS_STATUS_OK
 * means key exist, others means key does not exist or error occurs
 */
HKS_DLL_API_PUBLIC int32_t hks_is_key_exist(const struct hks_blob *key_alias)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    int32_t status = hks_is_valid_alias(key_alias);

    if (status != HKS_STATUS_OK)
        return status;
    status = hks_access_is_key_exist(key_alias);
    return status;
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_asymmetric_sign(
    const struct hks_blob *key_alias,
    const struct hks_key_param *key_param, const struct hks_blob *hash,
    struct hks_blob *signature)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    int32_t status = hks_is_valid_alias(key_alias);

    hks_if_status_error_return(status);
    hks_if_true_return_error(((key_param == NULL) || (hash == NULL) ||
        (signature == NULL)), HKS_ERROR_NULL_POINTER);
    if ((key_param->key_type != HKS_KEY_TYPE_EDDSA_KEYPAIR_ED25519) ||
        ((key_param->key_usage & HKS_KEY_USAGE_SIGN) == 0))
        return HKS_ERROR_NOT_SUPPORTED;
    if ((hash->data == NULL) || (hash->size <= 0))
        return HKS_ERROR_INVALID_ARGUMENT;
    if ((signature->data == NULL) ||
        (signature->size < HKS_SIGNATURE_MIN_SIZE))
        return HKS_ERROR_BUFFER_TOO_SMALL;

    return hks_access_sign(key_alias, key_param, hash, signature);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_asymmetric_verify(const struct hks_blob *key,
    const struct hks_key_param *key_param, const struct hks_blob *hash,
    const struct hks_blob *signature)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    hks_if_true_return_error(((key == NULL) || (key_param == NULL) ||
        (hash == NULL) || (signature == NULL)), HKS_ERROR_NULL_POINTER);

    int32_t status;

    if (key->type == HKS_BLOB_TYPE_ALIAS) {
        status = hks_is_valid_alias(key);
        hks_if_status_error_return(status);
    } else if (key->type == HKS_BLOB_TYPE_KEY) {
        if (key_param->key_type != HKS_KEY_TYPE_EDDSA_PUBLIC_KEY_ED25519)
            return HKS_ERROR_NOT_SUPPORTED;
        if ((key->data == NULL) || (key->size != HKS_PUBLIC_BYTES_ED25519))
            return HKS_ERROR_INVALID_KEY_INFO;
    } else {
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    if ((key_param->key_usage & HKS_KEY_USAGE_VERIFY) == 0)
        return HKS_ERROR_NOT_SUPPORTED;

    if ((hash->data == NULL) || (hash->size <= 0))
        return HKS_ERROR_INVALID_ARGUMENT;

    if ((signature->data == NULL) || (signature->size < HKS_SIGNATURE_MIN_SIZE))
        return HKS_ERROR_INVALID_ARGUMENT;
    return hks_access_verify(key, hash, signature);
#endif
}

static int32_t hks_aead_encrypt_ree(const struct hks_blob *key,
    const struct hks_key_param *key_param,
    const struct hks_crypt_param *crypt_param,
    const struct hks_blob *plain_text,
    struct hks_blob *cipher_text_with_tag)
{
    hks_if_true_return_error(((key_param->key_type != HKS_KEY_TYPE_AES) ||
        (key_param->key_mode != HKS_ALG_GCM) ||
        (key_param->key_pad != HKS_PADDING_NONE) ||
        ((key_param->key_usage & HKS_KEY_USAGE_ENCRYPT) == 0)), HKS_ERROR_NOT_SUPPORTED);

    hks_if_true_return_error(((key_param->key_len != HKS_MAX_KEY_LEN_128) &&
        (key_param->key_len != HKS_MAX_KEY_LEN_192) &&
        (key_param->key_len != HKS_MAX_KEY_LEN_256)), HKS_ERROR_NOT_SUPPORTED);

    hks_if_true_return_error(((key->type != HKS_BLOB_TYPE_KEY) || (key->data == NULL) ||
        (key->size != (key_param->key_len / HKS_BITS_PER_BYTES))), HKS_ERROR_INVALID_KEY_INFO);

    hks_if_true_return_error(((crypt_param->nonce.data == NULL) ||
        (crypt_param->nonce.size < HKS_AES_GCM_MIN_IV_LENGTH) ||
        (crypt_param->aad.data == NULL) ||
        (crypt_param->aad.size == 0) ||
        (plain_text->data == NULL) || (plain_text->size == 0)), HKS_ERROR_INVALID_ARGUMENT);

    hks_if_true_return_error(((cipher_text_with_tag == NULL) ||
        (cipher_text_with_tag->data == NULL) ||
        (cipher_text_with_tag->size < (plain_text->size + HKS_SALT_MAX_SIZE))),
        HKS_ERROR_INVALID_ARGUMENT);

    return hks_access_aead_encrypt(key, key_param, crypt_param, plain_text, cipher_text_with_tag);
}

HKS_DLL_API_PUBLIC int32_t hks_aead_encrypt(const struct hks_blob *key,
    const struct hks_key_param *key_param,
    const struct hks_crypt_param *crypt_param,
    const struct hks_blob *plain_text,
    struct hks_blob *cipher_text_with_tag)
{
    hks_if_true_return_error(((key == NULL) || (key_param == NULL) ||
        (crypt_param == NULL || (plain_text == NULL) ||
        (cipher_text_with_tag == NULL))), HKS_ERROR_NULL_POINTER);
    return hks_aead_encrypt_ree(key, key_param, crypt_param, plain_text, cipher_text_with_tag);
}

static int32_t hks_aead_decrypt_ree(const struct hks_blob *key,
    const struct hks_key_param *key_param,
    const struct hks_crypt_param *crypt_param,
    struct hks_blob *plain_text,
    const struct hks_blob *cipher_text_with_tag)
{
    hks_if_true_return_error(((key_param->key_type != HKS_KEY_TYPE_AES) ||
        (key_param->key_mode != HKS_ALG_GCM) ||
        (key_param->key_pad != HKS_PADDING_NONE) ||
        ((key_param->key_usage & HKS_KEY_USAGE_DECRYPT) == 0)), HKS_ERROR_NOT_SUPPORTED);

    hks_if_true_return_error(((key_param->key_len != HKS_MAX_KEY_LEN_128) &&
        (key_param->key_len != HKS_MAX_KEY_LEN_192) &&
        (key_param->key_len != HKS_MAX_KEY_LEN_256)), HKS_ERROR_NOT_SUPPORTED);

    hks_if_true_return_error(((key->type != HKS_BLOB_TYPE_KEY) || (key->data == NULL) ||
        (key->size != (key_param->key_len / HKS_BITS_PER_BYTES))), HKS_ERROR_INVALID_KEY_INFO);

    hks_if_true_return_error(((crypt_param->nonce.data == NULL) ||
        (crypt_param->nonce.size == 0) ||
        (crypt_param->aad.data == NULL) ||
        (crypt_param->aad.size == 0) ||
        (plain_text == NULL) || (plain_text->data == NULL)), HKS_ERROR_INVALID_ARGUMENT);

    hks_if_true_return_error(((cipher_text_with_tag->data == NULL) ||
        (cipher_text_with_tag->size <= HKS_SALT_MAX_SIZE) ||
        (plain_text->size < (cipher_text_with_tag->size - HKS_SALT_MAX_SIZE))),
        HKS_ERROR_INVALID_ARGUMENT);

    return hks_access_aead_decrypt(key, key_param, crypt_param, cipher_text_with_tag, plain_text);
}

HKS_DLL_API_PUBLIC int32_t hks_aead_decrypt(const struct hks_blob *key,
    const struct hks_key_param *key_param,
    const struct hks_crypt_param *crypt_param,
    struct hks_blob *plain_text,
    const struct hks_blob *cipher_text_with_tag)
{
    hks_if_true_return_error(((key == NULL) || (key_param == NULL) ||
        (crypt_param == NULL || (plain_text == NULL) ||
        (cipher_text_with_tag == NULL))), HKS_ERROR_NULL_POINTER);
    return hks_aead_decrypt_ree(key, key_param, crypt_param, plain_text, cipher_text_with_tag);
}

HKS_DLL_API_PUBLIC int32_t hks_key_derivation(struct hks_blob *derived_key,
    const struct hks_key_param *key_param, const struct hks_blob *kdf_key,
    const struct hks_blob *salt, const struct hks_blob *label)
{
    hks_if_true_return_error(((derived_key == NULL) ||
        (key_param == NULL) || (kdf_key == NULL) ||
        (salt == NULL) || (label == NULL)),
        HKS_ERROR_NULL_POINTER);
    return hks_access_key_derivation(derived_key, kdf_key, salt, label,
        key_param);
}

HKS_DLL_API_PUBLIC int32_t hks_key_agreement(struct hks_blob *agreed_key,
    const struct hks_key_param *private_key_param,
    const uint32_t agreement_alg, const struct hks_blob *private_key,
    const struct hks_blob *peer_public_key)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    hks_if_true_return_error(((agreed_key == NULL) ||
        (private_key_param == NULL) || (private_key == NULL) ||
        (peer_public_key == NULL)), HKS_ERROR_NULL_POINTER);

    if ((agreed_key == NULL) || (agreed_key->data == NULL))
        return HKS_ERROR_NULL_POINTER;

    if (agreed_key->size < HKS_KEY_BYTES_CURVE25519)
        return HKS_ERROR_INVALID_KEY_INFO;

    if ((private_key_param->key_type !=
        HKS_KEY_TYPE_ECC_KEYPAIR_CURVE25519) ||
        (private_key_param->key_usage != HKS_KEY_USAGE_DERIVE) ||
        (private_key_param->key_mode != hks_alg_ecdh(HKS_ALG_ECDH_RAW)) ||
        (agreement_alg != hks_alg_ecdh(HKS_ALG_ECDH_RAW)))
        return HKS_ERROR_NOT_SUPPORTED;

    if ((private_key->data == NULL) ||
        (private_key->size != HKS_KEY_BYTES_CURVE25519))
        return HKS_ERROR_INVALID_PRIVATE_KEY;

    if ((peer_public_key->data == NULL) ||
        (peer_public_key->size != HKS_KEY_BYTES_CURVE25519))
        return HKS_ERROR_INVALID_PUBLIC_KEY;
    return hks_access_key_agreement(agreed_key,
        private_key_param, private_key, peer_public_key, agreement_alg);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_generate_random(struct hks_blob *random)
{
    hks_if_true_return_error(((random == NULL) || (random->data == NULL)),
        HKS_ERROR_NULL_POINTER);
    hks_if_true_return_error((random->size > HKS_RANDOM_MAX_LEN),
        HKS_ERROR_INVALID_ARGUMENT);
    return hks_access_get_random(random);
}

HKS_DLL_API_PUBLIC int32_t hks_hmac(const struct hks_blob *key,
    const uint32_t alg, const struct hks_blob *src_data,
    struct hks_blob *output)
{
    hks_if_true_return_error(((key == NULL) || (src_data == NULL) ||
        (output == NULL)), HKS_ERROR_NULL_POINTER);
    return hks_access_hmac(key, alg, src_data, output);
}

HKS_DLL_API_PUBLIC int32_t hks_hash(const uint32_t alg,
    const struct hks_blob *src_data, struct hks_blob *hash)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    hks_if_true_return_error(((src_data == NULL) || (hash == NULL)),
        HKS_ERROR_NULL_POINTER);
    return hks_access_hash(alg, src_data, hash);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_bn_exp_mod(struct hks_blob *x,
    const struct hks_blob *a, const struct hks_blob *e,
    const struct hks_blob *n)
{
    hks_if_true_return_error(((x == NULL) || (a == NULL) || (e == NULL) ||
        (n == NULL)), HKS_ERROR_NULL_POINTER);
    return hks_access_bn_exp_mod(x, a, e, n);
}

HKS_DLL_API_PUBLIC int32_t hks_register_file_callbacks(
    struct hks_file_callbacks *callbacks)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_SUCCESS;
#else
    if (callbacks == NULL)
        return HKS_ERROR_NULL_POINTER;

    return hks_service_register_file_callbacks(callbacks);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_register_get_hardware_udid_callback(
    hks_get_hardware_udid_callback callback)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_SUCCESS;
#else
    return hks_service_register_get_hardware_udid_callback(callback);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_register_log_interface(
    const struct hks_log_f_group *log)
{
    if (log == NULL)
        return HKS_ERROR_NULL_POINTER;

    hks_register_log(log);
    return HKS_STATUS_OK;
}

HKS_DLL_API_PUBLIC int32_t hks_get_pub_key_alias_list(
    struct hks_blob *key_alias_list, uint32_t *list_count)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_ERROR_NOT_SUPPORTED;
#else
    if ((key_alias_list == NULL) || (list_count == NULL))
        return HKS_ERROR_NULL_POINTER;

    return hks_access_get_pub_key_alias_list(key_alias_list, list_count);
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_init(void)
{
#ifdef _CUT_AUTHENTICATE_
    log_debug("call hks init success.");
    return HKS_SUCCESS;
#else
    return hks_access_init();
#endif
}

HKS_DLL_API_PUBLIC void hks_destroy(void)
{
#ifdef _CUT_AUTHENTICATE_
    return;
#else
    hks_access_destroy();
#endif
}

HKS_DLL_API_PUBLIC int32_t hks_refresh_key_info(void)
{
#ifdef _CUT_AUTHENTICATE_
    return HKS_SUCCESS;
#else
    return hks_access_refresh_key_info();
#endif
}
