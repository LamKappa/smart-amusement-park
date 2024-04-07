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

#include "hal_pms.h"
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <mbedtls/md.h>
#include "emmc_if.h"
#include "parameter.h"
#include "securec.h"

#define CID_LENGTH 16
#define UDID_ORI_BYTES 32
#define HEX_LEN 2
#define P_DIR "/storage/app/etc/permissions/"
#define PERM_MAX 1024

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static const PermissionDef g_permissions[] = {
    // appfwk
    {"ohos.permission.LISTEN_BUNDLE_CHANGE",        SYSTEM_GRANT,   NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.GET_BUNDLE_INFO",             SYSTEM_GRANT,   NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.INSTALL_BUNDLE",              SYSTEM_GRANT,   NOT_RESTRICTED,    CAP_NOT_BINDED},
    // media
    {"ohos.permission.CAMERA",                      USER_GRANT,     NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.MODIFY_AUDIO_SETTINGS",       SYSTEM_GRANT,   NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.READ_MEDIA",                  USER_GRANT,     NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.MICROPHONE",                  USER_GRANT,     NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.WRITE_MEDIA",                 USER_GRANT,     NOT_RESTRICTED,    CAP_NOT_BINDED},
    // soft_bus
    {"ohos.permission.DISTRIBUTED_DATASYNC",        USER_GRANT,     NOT_RESTRICTED,    CAP_NOT_BINDED},
    // dvkit
    {"ohos.permission.DISTRIBUTED_VIRTUALDEVICE",   USER_GRANT,     NOT_RESTRICTED,    CAP_NOT_BINDED},
    // might expire in the future
    {"ohos.permission.RECORD_AUDIO",          USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.READ_MEDIA_AUDIO",      USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.READ_MEDIA_IMAGES",     USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.READ_MEDIA_VIDEO",      USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.WRITE_MEDIA_AUDIO",     USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.WRITE_MEDIA_IMAGES",    USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
    {"ohos.permission.WRITE_MEDIA_VIDEO",     USER_GRANT,    NOT_RESTRICTED,    CAP_NOT_BINDED},
};

static unsigned char *GetCId()
{
    int mallocSize = sizeof(unsigned char) * CID_LENGTH;
    unsigned char *cid = (unsigned char *)malloc(mallocSize);

    if (cid == NULL) {
        return NULL;
    }
    if (memset_s(cid, mallocSize, 0x0, mallocSize) != EOK) {
        free(cid);
        return NULL;
    }
    EmmcGetHuid(cid, CID_LENGTH);
    return cid;
}

static int GenerateOriginalDevUdid(unsigned char *ori, int size)
{
    if (ori == NULL || size != UDID_ORI_BYTES) {
        return PERM_ERRORCODE_INVALID_PARAMS;
    }
    int result = PERM_ERRORCODE_GENERATE_UDID_FAILED;
    const mbedtls_md_info_t *mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (mdInfo == NULL) {
        return result;
    }
    mbedtls_md_context_t mdCtx;
    mbedtls_md_init(&mdCtx);
    if (mbedtls_md_setup(&mdCtx, mdInfo, 0) != 0) {
        goto EXIT;
    }
    if (mbedtls_md_starts(&mdCtx) != 0) {
        goto EXIT;
    }
    // Get sn number
    const char *sn = GetSerial();
    if (sn == NULL) {
        goto EXIT;
    }
    // Get emmc id
    unsigned char *cid = GetCId();
    if (cid == NULL) {
        goto EXIT;
    }
    if (mbedtls_md_update(&mdCtx, sn, strlen(sn)) != 0) {
        goto FREE_EXIT;
    }
    if (mbedtls_md_update(&mdCtx, cid, CID_LENGTH) != 0) {
        goto FREE_EXIT;
    }
    if (mbedtls_md_finish(&mdCtx, ori) != 0) {
        goto FREE_EXIT;
    }
    result = PERM_ERRORCODE_SUCCESS;

FREE_EXIT:
    free(cid);

EXIT:
    mbedtls_md_free(&mdCtx);
    return result;
}

PermissionDef* HalGetPermissionList(unsigned int *length)
{
    if (length == NULL) {
        return NULL;
    }
    *length = sizeof(g_permissions) / sizeof(PermissionDef);
    return (PermissionDef*)g_permissions;
}

const char *HalGetPermissionPath()
{
    return P_DIR;
}

int HalGetMaxPermissionSize()
{
    return PERM_MAX;
}

void* HalMalloc(unsigned int size)
{
    if (size == 0) {
        return NULL;
    }
    return malloc(size);
}

void HalFree(void *ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}

int HalAccess(const char *pathname)
{
    return access(pathname, F_OK);
}

void HalMutexLock()
{
    pthread_mutex_lock(&g_mutex);
}

void HalMutexUnlock()
{
    pthread_mutex_unlock(&g_mutex);
}

int HalGetDevUdid(unsigned char *udid, int size)
{
    unsigned char tempUdid[UDID_ORI_BYTES] = {0};
    int result = GenerateOriginalDevUdid(tempUdid, UDID_ORI_BYTES);
    if (result != PERM_ERRORCODE_SUCCESS) {
        return result;
    }

    for (int i = 0; i < UDID_ORI_BYTES; i++) {
        if (sprintf_s(udid + HEX_LEN * i, UDID_FINAL_BYTES + 1 - HEX_LEN * i, "%02X", tempUdid[i]) < 0) {
            return PERM_ERRORCODE_GENERATE_UDID_FAILED;
        }
    }
    udid[UDID_FINAL_BYTES] = '\0';
    return PERM_ERRORCODE_SUCCESS;
}

bool HalIsValidPath(const char *path)
{
    if (path == NULL) {
        return false;
    }
    char resolvedPath[PATH_MAX + 1] = {0x0};
    if (strlen(path) > PATH_MAX || NULL == realpath(path, resolvedPath)) {
        return false;
    }
    struct stat buf;
    stat(path, &buf);
    if (S_IFDIR & buf.st_mode) {
        resolvedPath[strlen(resolvedPath)] = '/';
    }

    return (strncmp(resolvedPath, P_DIR, strlen(P_DIR)) == 0);
}
