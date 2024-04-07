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

#include "hks_access.h"
#include "securec.h"

#include "common/hks_log_utils.h"
#include "hks_errno.h"
#include "soft_service/hks_service.h"

static void __hks_handle_secure_call(struct sec_mod_msg *msg_box);
void hks_enter_secure_mode(struct sec_mod_msg *msg)
{
    if (msg == NULL)
        return;
    __hks_handle_secure_call(msg);
}

#ifndef _CUT_AUTHENTICATE_
int32_t hks_access_init(void)
{
    int32_t status = hks_service_key_info_init();

    hks_if_status_error_return(status);

    return status;
}

void hks_access_destroy(void)
{
    hks_service_destroy();
}

int32_t hks_access_refresh_key_info(void)
{
    int32_t status = hks_service_refresh_key_info();

    hks_if_status_error_return(status);

    return status;
}

int32_t hks_access_generate_key(const struct hks_blob *key_alias,
    const struct hks_key_param *key_param)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_generate_key_msg *msg = &msg_box.msg_data.generate_key_msg;

    msg_box.cmd_id = HKS_GENERATE_KEY;
    msg->key_alias = key_alias;
    msg->key_param = key_param;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_generate_key_ex(const struct hks_key_param *key_param, struct hks_blob *priv_key,
    struct hks_blob *pub_key)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_generate_ex_msg *msg = &msg_box.msg_data.generate_ex_msg;

    msg_box.cmd_id = HKS_GENERATE_KEY_EX;
    msg->key_param = key_param;
    msg->priv_key = priv_key;
    msg->pub_key = pub_key;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_sign(const struct hks_blob *key_alias, const struct hks_key_param *key_param,
    const struct hks_blob *hash, struct hks_blob *signature)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_sign_verify_msg *msg = &msg_box.msg_data.sign_verify_msg;

    msg_box.cmd_id = HKS_SIGN;
    msg->key = key_alias;
    msg->key_param = key_param;
    msg->message = hash;
    msg->signature = signature;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_verify(const struct hks_blob *key_alias, const struct hks_blob *hash,
    const struct hks_blob *signature)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_sign_verify_msg *msg = &msg_box.msg_data.sign_verify_msg;

    msg_box.cmd_id = HKS_VERIFY;
    msg->key = key_alias;
    msg->message = hash;
    msg->signature = (struct hks_blob *)signature;
    hks_enter_secure_mode(&msg_box);
    return msg_box.status;
}
#endif

int32_t hks_access_aead_encrypt(const struct hks_blob *key, const struct hks_key_param *key_param,
    const struct hks_crypt_param *crypt_param, const struct hks_blob *plain_text, struct hks_blob *cipher_text_with_tag)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_encrypt_decrypt_msg *msg = &msg_box.msg_data.encrypt_decrypt_msg;

    msg_box.cmd_id = HKS_ENCRYPT;
    msg->key = key;
    msg->key_param = key_param;
    msg->crypt_param = crypt_param;
    msg->plain_text = (struct hks_blob *)plain_text;
    msg->cipher_text_with_tag = cipher_text_with_tag;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_aead_decrypt(const struct hks_blob *key, const struct hks_key_param *key_param,
    const struct hks_crypt_param *crypt_param, const struct hks_blob *cipher_text_with_tag, struct hks_blob *plain_text)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_encrypt_decrypt_msg *msg = &msg_box.msg_data.encrypt_decrypt_msg;

    msg_box.cmd_id = HKS_DECRYPT;
    msg->key = key;
    msg->key_param = key_param;
    msg->crypt_param = crypt_param;
    msg->plain_text = plain_text;
    msg->cipher_text_with_tag = (struct hks_blob *)cipher_text_with_tag;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

#ifndef _CUT_AUTHENTICATE_
int32_t hks_access_import_key(const struct hks_blob *key_alias, const struct hks_key_param *key_param,
    const struct hks_blob *key)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_import_key_msg *msg = &msg_box.msg_data.import_key_msg;

    msg_box.cmd_id = HKS_IMPORT_KEY;
    msg->key_alias = key_alias;
    msg->key_param = key_param;
    msg->key_data = key;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_export_key(const struct hks_blob *key_alias, struct hks_blob *key)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_export_key_msg *msg = &msg_box.msg_data.export_key_msg;

    msg_box.cmd_id = HKS_EXPORT_KEY;
    msg->key_alias = key_alias;
    msg->key_data = key;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_delete_key(const struct hks_blob *key_alias)
{
    int32_t status = hks_service_delete_key(key_alias);

    hks_if_status_error_return(status);

    return status;
}

int32_t hks_access_is_key_exist(const struct hks_blob *key_alias)
{
    int32_t status = hks_service_is_key_exist(key_alias);

    hks_if_status_error_return(status);

    return status;
}

int32_t hks_access_get_key_param(const struct hks_blob *key_alias,
    struct hks_key_param *key_param)
{
    int32_t status = hks_service_get_key_param(key_alias, key_param);

    hks_if_status_error_return(status);

    return status;
}

int32_t hks_access_get_pub_key_alias_list(struct hks_blob *key_alias_list, uint32_t *list_count)
{
    int32_t status = hks_service_get_pub_key_alias_list(key_alias_list, list_count);

    hks_if_status_error_return(status);

    return status;
}
#endif

int32_t hks_access_get_random(struct hks_blob *random)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_generate_random_msg *msg = &msg_box.msg_data.generate_random_msg;

    msg_box.cmd_id = HKS_GENERATE_RANDOM;
    msg->random = random;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_hmac(const struct hks_blob *key,
    uint32_t alg, const struct hks_blob *src_data, struct hks_blob *output)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_hmac_msg *msg = &msg_box.msg_data.hmac_msg;

    msg_box.cmd_id = HKS_HMAC;
    msg->alg = alg;
    msg->key = key;
    msg->src_data = src_data;
    msg->output = output;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

#ifndef _CUT_AUTHENTICATE_
int32_t hks_access_hash(uint32_t alg, const struct hks_blob *src_data,
    struct hks_blob *hash)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_hash_msg *msg = &msg_box.msg_data.hash_msg;

    msg_box.cmd_id = HKS_HASH;
    msg->alg = alg;
    msg->src_data = src_data;
    msg->hash = hash;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_key_agreement(struct hks_blob *agreed_key, const struct hks_key_param *private_key_param,
    const struct hks_blob *private_key, const struct hks_blob *peer_public_key, const uint32_t agreement_alg)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_key_agreement_msg *msg = &msg_box.msg_data.key_agreement_msg;

    msg_box.cmd_id = HKS_KEY_AGREEMENT;
    msg->agreed_key = agreed_key;
    msg->key_param = private_key_param;
    msg->agreement_alg = agreement_alg;
    msg->priv_key = private_key;
    msg->pub_key = peer_public_key;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}
#endif

int32_t hks_access_key_derivation(struct hks_blob *derived_key, const struct hks_blob *kdf_key,
    const struct hks_blob *salt, const struct hks_blob *label, const struct hks_key_param *key_params)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_key_derivation_msg *msg = &msg_box.msg_data.key_derivation_msg;

    msg_box.cmd_id = HKS_KEY_DERIVATION;
    msg->derived_key = derived_key;
    msg->key_param = key_params;
    msg->kdf_key = kdf_key;
    msg->salt = salt;
    msg->label = label;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

int32_t hks_access_bn_exp_mod(struct hks_blob *x, const struct hks_blob *a, const struct hks_blob *e,
    const struct hks_blob *n)
{
    struct sec_mod_msg msg_box;

    (void)memset_s(&msg_box, sizeof(msg_box), 0, sizeof(msg_box));

    struct hks_bn_exp_mod_msg *msg = &msg_box.msg_data.bn_exp_mod_msg;

    msg_box.cmd_id = HKS_BN_EXP_MOD;
    msg->x = x;
    msg->a = a;
    msg->e = e;
    msg->n = n;
    hks_enter_secure_mode(&msg_box);

    return msg_box.status;
}

#ifndef _CUT_AUTHENTICATE_
static void hks_handle_generate_key(struct sec_mod_msg *msg_box)
{
    struct hks_generate_key_msg *msg = &msg_box->msg_data.generate_key_msg;

    msg_box->status = hks_service_generate_key(msg->key_alias, msg->key_param);
}

static void hks_handle_sign(struct sec_mod_msg *msg_box)
{
    struct hks_sign_verify_msg *msg = &msg_box->msg_data.sign_verify_msg;

    msg_box->status = hks_service_asymmetric_sign(msg->key, msg->key_param, msg->message, msg->signature);
}

static void hks_handle_import(struct sec_mod_msg *msg_box)
{
    struct hks_import_key_msg *msg = &msg_box->msg_data.import_key_msg;

    msg_box->status = hks_service_import_public_key(msg->key_alias, msg->key_param, msg->key_data);
}

static void hks_handle_export(struct sec_mod_msg *msg_box)
{
    struct hks_export_key_msg *msg = &msg_box->msg_data.export_key_msg;

    msg_box->status = hks_service_export_public_key(msg->key_alias, msg->key_data);
}

static void hks_handle_generate_key_ex(struct sec_mod_msg *msg_box)
{
    struct hks_generate_ex_msg *msg = &msg_box->msg_data.generate_ex_msg;

    msg_box->status = hks_service_generate_asymmetric_key(msg->key_param, msg->priv_key, msg->pub_key);
}

static void hks_handle_verify(struct sec_mod_msg *msg_box)
{
    struct hks_sign_verify_msg *msg = &msg_box->msg_data.sign_verify_msg;

    msg_box->status = hks_service_asymmetric_verify(msg->key, msg->message, msg->signature);
}
#endif

static void hks_handle_encrypt(struct sec_mod_msg *msg_box)
{
    struct hks_encrypt_decrypt_msg *msg = &msg_box->msg_data.encrypt_decrypt_msg;

    msg_box->status = hks_service_aead_encrypt_ex(msg->key, msg->key_param, msg->crypt_param, msg->plain_text,
        msg->cipher_text_with_tag);
}

static void hks_handle_decrypt(struct sec_mod_msg *msg_box)
{
    struct hks_encrypt_decrypt_msg *msg = &msg_box->msg_data.encrypt_decrypt_msg;

    msg_box->status = hks_service_aead_decrypt_ex(msg->key, msg->key_param, msg->crypt_param,
        msg->cipher_text_with_tag, msg->plain_text);
}

static void hks_handle_get_random(struct sec_mod_msg *msg_box)
{
    struct hks_generate_random_msg *msg = &msg_box->msg_data.generate_random_msg;

    msg_box->status = hks_service_get_random(msg->random);
}

#ifndef _CUT_AUTHENTICATE_
static void hks_handle_key_agreement(struct sec_mod_msg *msg_box)
{
    struct hks_key_agreement_msg *msg = &msg_box->msg_data.key_agreement_msg;

    msg_box->status = hks_service_key_agreement(msg->agreed_key, msg->key_param,
        msg->priv_key, msg->pub_key, msg->agreement_alg);
}
#endif

static void hks_handle_key_deviration(struct sec_mod_msg *msg_box)
{
    struct hks_key_derivation_msg *msg = &msg_box->msg_data.key_derivation_msg;

    msg_box->status = hks_service_key_derivation(msg->derived_key, msg->kdf_key, msg->salt, msg->label, msg->key_param);
}

static void hks_handle_hmac(struct sec_mod_msg *msg_box)
{
    struct hks_hmac_msg *msg = &msg_box->msg_data.hmac_msg;

    msg_box->status = hks_service_hmac_ex(msg->key, msg->alg, msg->src_data, msg->output);
}

#ifndef _CUT_AUTHENTICATE_
static void hks_handle_hash(struct sec_mod_msg *msg_box)
{
    struct hks_hash_msg *msg = &msg_box->msg_data.hash_msg;

    msg_box->status = hks_service_hash(msg->alg, msg->src_data, msg->hash);
}
#endif

static void hks_handle_bn_exp_mod(struct sec_mod_msg *msg_box)
{
    struct hks_bn_exp_mod_msg *msg = &msg_box->msg_data.bn_exp_mod_msg;

    msg_box->status = hks_service_bn_exp_mod(msg->x, msg->a, msg->e, msg->n);
}

hks_handle_func_p g_hks_handle_func[HKS_CMD_MAX] = {
#ifndef _CUT_AUTHENTICATE_
    hks_handle_generate_key,
    hks_handle_generate_key_ex,
    hks_handle_sign,
    hks_handle_verify,
    hks_handle_encrypt,
    hks_handle_decrypt,
    hks_handle_import,
    hks_handle_export,
    hks_handle_get_random,
    hks_handle_key_agreement,
    hks_handle_key_deviration,
    hks_handle_hmac,
    hks_handle_hash,
    hks_handle_bn_exp_mod,
#else
    NULL,
    NULL,
    NULL,
    NULL,
    hks_handle_encrypt,
    hks_handle_decrypt,
    NULL,
    NULL,
    hks_handle_get_random,
    NULL,
    hks_handle_key_deviration,
    hks_handle_hmac,
    NULL,
    hks_handle_bn_exp_mod,
#endif
};

static void __hks_handle_secure_call(struct sec_mod_msg *msg_box)
{
    if (msg_box == NULL)
        return;

    if (msg_box->cmd_id < HKS_CMD_MAX) {
        if (g_hks_handle_func[msg_box->cmd_id])
            g_hks_handle_func[msg_box->cmd_id](msg_box);
    } else {
        msg_box->status = HKS_ERROR_NOT_SUPPORTED;
    }
}
