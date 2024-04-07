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

#include "hks_storage.h"
#include "hks_file_operator.h"
#include "hks_log.h"
#include "hks_mem.h"

#define HKS_ENCODE_OFFSET_LEN         6
#define HKS_ENCODE_KEY_SALT_VALUE     0x3f
#define HKS_STORAGE_BAK_FLAG_TRUE     1
#define HKS_STORAGE_BAK_FLAG_FLASE    0

struct HksStoreInfo {
    char *processPath; /* file path include process */
    char *path; /* file path include process/key(or certchain) */
    char *fileName; /* file name that can be recognized by the file system */
    uint32_t size;
};

struct HksStoreFileInfo {
    struct HksStoreInfo mainPath;
#ifdef SUPPORT_STORAGE_BACKUP
    struct HksStoreInfo bakPath;
#endif
};

struct HksFileEntry {
    char *fileName;
    uint32_t fileNameLen;
};

#ifdef HKS_SUPPORT_GT_WATCH
static void ConstructInvalidCharacter(const char input, char *output)
{
    switch (input) {
        case ':':
            *output = '#';
            return;
        case '<':
            *output = '$';
            return;
        case '>':
            *output = '%';
            return;
        case '?':
            *output = '&';
            return;
        case '\\':
            *output = '(';
            return;
        case '|':
            *output = ')';
            return;
        default:
            *output = input;
            return;
    }
}
#endif

static void ResumeInvalidCharacter(const char input, char *output)
{
    switch (input) {
        case '#':
            *output = ':';
            return;
        case '$':
            *output = '<';
            return;
        case '%':
            *output = '>';
            return;
        case '&':
            *output = '?';
            return;
        case '(':
            *output = '\\';
            return;
        case ')':
            *output = '|';
            return;
        default:
            *output = input;
            return;
    }
}

/* Encode invisible content to visible */
static int32_t ConstructName(const struct HksBlob *blob, char *targetName, uint32_t nameLen)
{
    uint32_t count = 0;

    for (uint32_t i = 0; i < blob->size; ++i) {
        if (count >= nameLen) {
            return HKS_ERROR_INSUFFICIENT_DATA;
        }

        if ((blob->data[i] < '0') || (blob->data[i] > '~')) {
            targetName[count++] = '+' + (blob->data[i] >> HKS_ENCODE_OFFSET_LEN);
            targetName[count++] = '0' + (blob->data[i] & HKS_ENCODE_KEY_SALT_VALUE);
        } else {
            targetName[count++] = blob->data[i];
        }

#ifdef HKS_SUPPORT_GT_WATCH
        ConstructInvalidCharacter(targetName[count - 1], &targetName[count - 1]);
#endif
    }

    return HKS_SUCCESS;
}

/* Decode encoded content to original content */
static int32_t ConstructBlob(const char *src, struct HksBlob *blob)
{
    uint32_t size = strlen(src);
    uint8_t *outputBlob = (uint8_t *)HksMalloc(size);
    if (outputBlob == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    uint32_t count = 0;
    int32_t ret = HKS_SUCCESS;
    for (uint32_t i = 0; i < size; ++i) {
        if ((src[i] >= '+') && (src[i] <= '.')) {
            uint8_t c = (uint8_t)(src[i] - '+') << HKS_ENCODE_OFFSET_LEN;
            i++;
            if (i >= size) {
                ret = HKS_ERROR_INVALID_KEY_FILE; /* keyfile name invalid */
                break;
            }
            char tmp;
            ResumeInvalidCharacter(src[i], &tmp);
            c += tmp - '0';
            outputBlob[count++] = c;
        } else {
            char tmp;
            ResumeInvalidCharacter(src[i], &tmp);
            outputBlob[count++] = tmp;
        }
    }

    if (ret != HKS_SUCCESS) {
        HKS_FREE_PTR(outputBlob);
        return ret;
    }

    if (blob->size < count) {
        HKS_FREE_PTR(outputBlob);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(blob->data, blob->size, outputBlob, count) != EOK) {
        HKS_LOG_E("memcpy failed");
        ret = HKS_ERROR_BAD_STATE;
    }

    blob->size = count;
    HKS_FREE_PTR(outputBlob);
    return ret;
}

static int32_t GetPath(const char *path, const char *name, char *targetPath, uint32_t pathLen, uint32_t bakFlag)
{
    if (strncpy_s(targetPath, pathLen, path, strlen(path)) != EOK) {
        HKS_LOG_E("strncpy path failed");
        return HKS_ERROR_BAD_STATE;
    }

    if (targetPath[strlen(targetPath) - 1] != '/') {
        if (strncat_s(targetPath, pathLen, "/", strlen("/")) != EOK) {
            HKS_LOG_E("strncat slash failed");
            return HKS_ERROR_INTERNAL_ERROR;
        }
    }

    if (strncat_s(targetPath, pathLen, name, strlen(name)) != EOK) {
        HKS_LOG_E("strncat Name failed");
        return HKS_ERROR_BAD_STATE;
    }

    if (bakFlag == HKS_STORAGE_BAK_FLAG_TRUE) {
        if (strncat_s(targetPath, pathLen, ".bak", strlen(".bak")) != EOK) {
            HKS_LOG_E("strncat bak failed");
            return HKS_ERROR_BAD_STATE;
        }
    }

    return HKS_SUCCESS;
}

static int32_t GetFullPath(const char *path, const char *processName, const char *storeName,
    struct HksStoreFileInfo *fileInfo)
{
    int32_t ret = GetPath(path, processName, fileInfo->mainPath.processPath, fileInfo->mainPath.size,
        HKS_STORAGE_BAK_FLAG_FLASE);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get processPath failed, ret = %d.", ret);
        return ret;
    }

    return GetPath(fileInfo->mainPath.processPath, storeName, fileInfo->mainPath.path, fileInfo->mainPath.size,
        HKS_STORAGE_BAK_FLAG_FLASE);
}

static int32_t MakeDirIfNotExist(const char *path)
{
    int32_t ret = HksIsFileExist(NULL, path);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_I("dir not exist, path = %s", path);
        if (HksMakeDir(path) != HKS_SUCCESS) {
            HKS_LOG_E("make dir failed");
            return HKS_ERROR_MAKE_DIR_FAIL;
        }
    }
    return HKS_SUCCESS;
}

#ifdef SUPPORT_STORAGE_BACKUP
static int32_t GetBakFullPath(const char *path, const char *processName, const char *storeName,
    struct HksStoreFileInfo *fileInfo)
{
    int32_t ret = GetPath(path, processName, fileInfo->bakPath.processPath, fileInfo->bakPath.size,
        HKS_STORAGE_BAK_FLAG_TRUE);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get bakProcessPath failed, ret = %d.", ret);
        return ret;
    }

    return GetPath(fileInfo->bakPath.processPath, storeName, fileInfo->bakPath.path, fileInfo->bakPath.size,
        HKS_STORAGE_BAK_FLAG_TRUE);
}

static int32_t GetBakFileName(const char *fileName, char *bakFileName, uint32_t bakFileNameLen)
{
    if (strncpy_s(bakFileName, bakFileNameLen, fileName, strlen(fileName)) != EOK) {
        HKS_LOG_E("strncpy_s fileName failed");
        return HKS_ERROR_BAD_STATE;
    }
    if (strncat_s(bakFileName, bakFileNameLen, ".bak", strlen(".bak")) != EOK) {
        HKS_LOG_E("strncat_s bak failed");
        return HKS_ERROR_BAD_STATE;
    }

    return HKS_SUCCESS;
}

static int32_t CopyKeyBlobFromSrc(const char *srcPath, const char *srcFileName,
    const char *destPath, const char *destFileName)
{
    uint32_t size = HksFileSize(srcPath, srcFileName);
    if (size == 0) {
        HKS_LOG_E("get file size failed, ret = %u.", size);
        return HKS_ERROR_FILE_SIZE_FAIL;
    }

    uint8_t *buffer = (uint8_t *)HksMalloc(size);
    if (buffer == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(buffer, size, 0, size);

    int32_t ret;
    do {
        size = HksFileRead(srcPath, srcFileName, 0, buffer, size);
        if (size == 0) {
            HKS_LOG_E("read file failed, ret = %u.", size);
            ret = HKS_ERROR_READ_FILE_FAIL;
            break;
        }

        ret = MakeDirIfNotExist(destPath);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("makdir destPath failed, ret = %d.", ret);
            break;
        }

        ret = HksFileWrite(destPath, destFileName, 0, buffer, size);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("file write destPath failed, ret = %d.", ret);
            break;
        }
    } while (0);

    HKS_FREE_PTR(buffer);
    return ret;
}

static int32_t CopyKeyBlob(const struct HksStoreFileInfo *fileInfo,
    int32_t isMainFileExist, int32_t isBakFileExist)
{
    if ((isMainFileExist != HKS_SUCCESS) && (isBakFileExist != HKS_SUCCESS)) {
        return HKS_ERROR_NOT_EXIST;
    } else if ((isMainFileExist == HKS_SUCCESS) && (isBakFileExist == HKS_SUCCESS)) {
        return HKS_SUCCESS;
    }

    int32_t ret = HKS_SUCCESS;
    if (isMainFileExist != HKS_SUCCESS) {
        ret = MakeDirIfNotExist(fileInfo->mainPath.processPath);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
        ret = CopyKeyBlobFromSrc(fileInfo->bakPath.path, fileInfo->bakPath.fileName,
            fileInfo->mainPath.path, fileInfo->mainPath.fileName);
    } else if (isBakFileExist != HKS_SUCCESS) {
        ret = MakeDirIfNotExist(fileInfo->bakPath.processPath);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
        ret = CopyKeyBlobFromSrc(fileInfo->mainPath.path, fileInfo->mainPath.fileName,
            fileInfo->bakPath.path, fileInfo->bakPath.fileName);
    }

    return ret;
}
#endif

static int32_t GetKeyBlobFromFile(const char *path, const char *fileName, struct HksBlob *keyBlob)
{
    uint32_t size = HksFileSize(path, fileName);
    if (size == 0) {
        return HKS_ERROR_FILE_SIZE_FAIL;
    }

    if (keyBlob->size < size) {
        return HKS_ERROR_INSUFFICIENT_DATA;
    }

    size = HksFileRead(path, fileName, 0, keyBlob->data, keyBlob->size);
    if (size == 0) {
        HKS_LOG_E("file read failed, ret = %u.", size);
        return HKS_ERROR_READ_FILE_FAIL;
    }

    keyBlob->size = size;
    return HKS_SUCCESS;
}

static int32_t SaveKeyBlob(const char *processPath, const char *path, const char *fileName,
    const struct HksBlob *keyBlob)
{
    int32_t ret = MakeDirIfNotExist(processPath);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("makedir processPath failed, ret = %d.", ret);
        return ret;
    }

    ret = MakeDirIfNotExist(path);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("makedir path failed, ret = %d.", ret);
        return ret;
    }

    return HksFileWrite(path, fileName, 0, keyBlob->data, keyBlob->size);
}

static int32_t DeleteKeyBlob(const struct HksStoreFileInfo *fileInfo)
{
    int32_t isMainFileExist = HksIsFileExist(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
#ifndef SUPPORT_STORAGE_BACKUP
    if (isMainFileExist != HKS_SUCCESS) {
        return HKS_ERROR_NOT_EXIST;
    }

    int32_t ret = HksFileRemove(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("delete key remove file failed, ret = %d.", ret);
        return ret;
    }
#else
    int32_t isBakFileExist = HksIsFileExist(fileInfo->bakPath.path, fileInfo->bakPath.fileName);
    if ((isMainFileExist != HKS_SUCCESS) && (isBakFileExist != HKS_SUCCESS)) {
        return HKS_ERROR_NOT_EXIST;
    }

    int32_t ret = HKS_SUCCESS;
    if (isMainFileExist == HKS_SUCCESS) {
        ret = HksFileRemove(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("delete key remove file failed, ret = %d.", ret);
            return ret;
        }
    }

    if (isBakFileExist == HKS_SUCCESS) {
        ret = HksFileRemove(fileInfo->bakPath.path, fileInfo->bakPath.fileName);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("delete key remove bakfile failed, ret = %d.", ret);
            return ret;
        }
    }
#endif

    return ret;
}

static int32_t GetKeyBlob(const struct HksStoreFileInfo *fileInfo, struct HksBlob *keyBlob)
{
    int32_t isMainFileExist = HksIsFileExist(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
#ifndef SUPPORT_STORAGE_BACKUP
    if (isMainFileExist != HKS_SUCCESS) {
        return HKS_ERROR_NOT_EXIST;
    }
    int32_t ret = GetKeyBlobFromFile(fileInfo->mainPath.path, fileInfo->mainPath.fileName, keyBlob);
#else
    int32_t isBakFileExist = HksIsFileExist(fileInfo->bakPath.path, fileInfo->bakPath.fileName);
    if ((isMainFileExist != HKS_SUCCESS) && (isBakFileExist != HKS_SUCCESS)) {
        return HKS_ERROR_NOT_EXIST;
    }

    int32_t ret = HKS_SUCCESS;
    if (isMainFileExist == HKS_SUCCESS) {
        ret = GetKeyBlobFromFile(fileInfo->mainPath.path, fileInfo->mainPath.fileName, keyBlob);
    } else if (isBakFileExist == HKS_SUCCESS) {
        ret = GetKeyBlobFromFile(fileInfo->bakPath.path, fileInfo->bakPath.fileName, keyBlob);
    }

    if (CopyKeyBlob(fileInfo, isMainFileExist, isBakFileExist) != HKS_SUCCESS) {
        HKS_LOG_W("CopyKeyBlob failed");
    }
#endif

    return ret;
}

static int32_t GetKeyBlobSize(const struct HksStoreFileInfo *fileInfo, uint32_t *keyBlobSize)
{
    int32_t isMainFileExist = HksIsFileExist(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
#ifndef SUPPORT_STORAGE_BACKUP
    if (isMainFileExist != HKS_SUCCESS) {
        return HKS_ERROR_NOT_EXIST;
    }
    uint32_t size = HksFileSize(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
    if (size == 0) {
        return HKS_ERROR_FILE_SIZE_FAIL;
    }
    *keyBlobSize = size;
#else
    int32_t isBakFileExist = HksIsFileExist(fileInfo->bakPath.path, fileInfo->bakPath.fileName);
    if ((isMainFileExist != HKS_SUCCESS) && (isBakFileExist != HKS_SUCCESS)) {
        return HKS_ERROR_NOT_EXIST;
    }

    uint32_t size = 0;
    if (isMainFileExist == HKS_SUCCESS) {
        size = HksFileSize(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
    } else if (isBakFileExist == HKS_SUCCESS) {
        size = HksFileSize(fileInfo->bakPath.path, fileInfo->bakPath.fileName);
    }

    if (size == 0) {
        return HKS_ERROR_FILE_SIZE_FAIL;
    }

    *keyBlobSize = size;
    if (CopyKeyBlob(fileInfo, isMainFileExist, isBakFileExist) != HKS_SUCCESS) {
        HKS_LOG_W("CopyKeyBlob failed");
    }
#endif

    return HKS_SUCCESS;
}

static int32_t IsKeyBlobExist(const struct HksStoreFileInfo *fileInfo)
{
    int32_t isMainFileExist = HksIsFileExist(fileInfo->mainPath.path, fileInfo->mainPath.fileName);
#ifndef SUPPORT_STORAGE_BACKUP
    if (isMainFileExist != HKS_SUCCESS) {
        return HKS_ERROR_NOT_EXIST;
    }
#else
    int32_t isBakFileExist = HksIsFileExist(fileInfo->bakPath.path, fileInfo->bakPath.fileName);
    if ((isMainFileExist != HKS_SUCCESS) && (isBakFileExist != HKS_SUCCESS)) {
        return HKS_ERROR_NOT_EXIST;
    }

    if (CopyKeyBlob(fileInfo, isMainFileExist, isBakFileExist) != HKS_SUCCESS) {
        HKS_LOG_W("CopyKeyBlob failed");
    }
#endif

    return HKS_SUCCESS;
}

static int32_t DataInit(char **data, uint32_t size)
{
    *data = (char *)HksMalloc(size);
    if (*data == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memset_s(*data, size, 0, size);
    return HKS_SUCCESS;
}

static int32_t FileInfoInit(struct HksStoreFileInfo *fileInfo)
{
    fileInfo->mainPath.size = HKS_MAX_FILE_NAME_LEN;
    /* if one param init fail, others free by caller function */
    int32_t ret = DataInit(&fileInfo->mainPath.processPath, fileInfo->mainPath.size);
    ret += DataInit(&fileInfo->mainPath.path, fileInfo->mainPath.size);
    ret += DataInit(&fileInfo->mainPath.fileName, fileInfo->mainPath.size);

#ifdef SUPPORT_STORAGE_BACKUP
    fileInfo->bakPath.size = HKS_MAX_FILE_NAME_LEN;
    ret += DataInit(&fileInfo->bakPath.processPath, fileInfo->bakPath.size);
    ret += DataInit(&fileInfo->bakPath.path, fileInfo->bakPath.size);
    ret += DataInit(&fileInfo->bakPath.fileName, fileInfo->bakPath.size);
#endif

    return ret;
}

static void FileInfoFree(struct HksStoreFileInfo *fileInfo)
{
    HKS_FREE_PTR(fileInfo->mainPath.processPath);
    HKS_FREE_PTR(fileInfo->mainPath.path);
    HKS_FREE_PTR(fileInfo->mainPath.fileName);
    fileInfo->mainPath.size = 0;

#ifdef SUPPORT_STORAGE_BACKUP
    HKS_FREE_PTR(fileInfo->bakPath.processPath);
    HKS_FREE_PTR(fileInfo->bakPath.path);
    HKS_FREE_PTR(fileInfo->bakPath.fileName);
    fileInfo->bakPath.size = 0;
#endif
}

static int32_t GetStorePath(const char *processName, const char *storageName, struct HksStoreFileInfo *fileInfo)
{
    int32_t ret = MakeDirIfNotExist(HKS_KEY_STORE_PATH);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("makedir main path failed");
        return ret;
    }

    ret = GetFullPath(HKS_KEY_STORE_PATH, processName, storageName, fileInfo);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get full path failed, ret = %d.", ret);
        return ret;
    }

#ifdef SUPPORT_STORAGE_BACKUP
    ret = MakeDirIfNotExist(HKS_KEY_STORE_BAK_PATH);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("makedir backup path failed");
        return ret;
    }

    ret = GetBakFullPath(HKS_KEY_STORE_BAK_PATH, processName, storageName, fileInfo);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get backup full path failed, ret = %d.", ret);
    }
#endif

    return ret;
}

static int32_t GetFileInfo(const struct HksBlob *processName, const struct HksBlob *keyAlias, uint32_t storageType,
    struct HksStoreFileInfo *fileInfo)
{
    int32_t ret = FileInfoInit(fileInfo);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hks file info init failed, ret = %d.", ret);
        return ret;
    }

    char *name = (char *)HksMalloc(HKS_MAX_FILE_NAME_LEN);
    if (name == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(name, HKS_MAX_FILE_NAME_LEN, 0, HKS_MAX_FILE_NAME_LEN);

    ret = ConstructName(processName, name, HKS_MAX_FILE_NAME_LEN);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("construct name failed, ret = %d.", ret);
        HKS_FREE_PTR(name);
        return ret;
    }

    ret = HKS_ERROR_NOT_SUPPORTED;
    if (storageType == HKS_STORAGE_TYPE_KEY) {
        ret = GetStorePath(name, HKS_KEY_STORE_KEY_PATH, fileInfo);
    } else if (storageType == HKS_STORAGE_TYPE_CERTCHAIN) {
        ret = GetStorePath(name, HKS_KEY_STORE_CERTCHAIN_PATH, fileInfo);
    } else if (storageType == HKS_STORAGE_TYPE_ROOT_KEY) {
        ret = GetStorePath(name, HKS_KEY_STORE_ROOT_KEY_PATH, fileInfo);
    }
    HKS_FREE_PTR(name);

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get store path failed, ret = %d.", ret);
        return ret;
    }

    ret = ConstructName(keyAlias, fileInfo->mainPath.fileName, fileInfo->mainPath.size);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("construct name failed, ret = %d.", ret);
        return ret;
    }

#ifdef SUPPORT_STORAGE_BACKUP
    ret = GetBakFileName(fileInfo->mainPath.fileName, fileInfo->bakPath.fileName, fileInfo->bakPath.size);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get backup file name failed, ret = %d.", ret);
    }
#endif

    return ret;
}

int32_t HksStoreKeyBlob(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    enum HksStorageType storageType, const struct HksBlob *keyBlob)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = GetFileInfo(processName, keyAlias, storageType, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }
        HKS_LOG_E("generate key or certchain, storage path: %s, key alias：%s.",
            fileInfo.mainPath.path, fileInfo.mainPath.fileName);

        ret = SaveKeyBlob(fileInfo.mainPath.processPath, fileInfo.mainPath.path, fileInfo.mainPath.fileName, keyBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks save key blob failed, ret = %d.", ret);
            break;
        }

#ifdef SUPPORT_STORAGE_BACKUP
        ret = SaveKeyBlob(fileInfo.bakPath.processPath, fileInfo.bakPath.path, fileInfo.bakPath.fileName, keyBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks save bak key blob failed, ret = %d.", ret);
        }
#endif
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

int32_t HksStoreDeleteKeyBlob(const struct HksBlob *processName, const struct HksBlob *keyAlias, uint32_t storageType)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = GetFileInfo(processName, keyAlias, storageType, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }
        HKS_LOG_E("delete key or certchain, storage path: %s, key alias：%s.",
            fileInfo.mainPath.path, fileInfo.mainPath.fileName);

        ret = DeleteKeyBlob(&fileInfo);
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

int32_t HksStoreIsKeyBlobExist(const struct HksBlob *processName, const struct HksBlob *keyAlias, uint32_t storageType)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = GetFileInfo(processName, keyAlias, storageType, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }

        ret = IsKeyBlobExist(&fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("check is key exist, ret = %d.", ret);
        }
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

int32_t HksStoreGetKeyBlob(const struct HksBlob *processName, const struct HksBlob *keyAlias, uint32_t storageType,
    struct HksBlob *keyBlob)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = GetFileInfo(processName, keyAlias, storageType, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }
        HKS_LOG_E("use key, storage path: %s, key alias：%s.",
            fileInfo.mainPath.path, fileInfo.mainPath.fileName);

        ret = GetKeyBlob(&fileInfo, keyBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get keyblob failed, ret = %d.", ret);
        }
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

int32_t HksStoreGetKeyBlobSize(const struct HksBlob *processName, const struct HksBlob *keyAlias,
    uint32_t storageType, uint32_t *keyBlobSize)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = GetFileInfo(processName, keyAlias, storageType, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }

        ret = GetKeyBlobSize(&fileInfo, keyBlobSize);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get keyblob size failed, ret = %d.", ret);
        }
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

static int32_t GetFileCount(const char *path, uint32_t *fileCount)
{
    if ((path == NULL) || (fileCount == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    void *dir = HksOpenDir(path);
    if (dir == NULL) {
        HKS_LOG_W("can't open directory");
        *fileCount = 0;
        return HKS_SUCCESS;
    }

    uint32_t count = 0;
    struct HksFileDirentInfo dire = { NULL };
    int32_t ret = HksGetDirFile(dir, &dire);
    while (ret == HKS_SUCCESS) {
        count++;
        ret = HksGetDirFile(dir, &dire);
    }
    (void)HksCloseDir(dir);
    *fileCount = count;

    return HKS_SUCCESS;
}

static int32_t GetFileNameList(const char *path, struct HksFileEntry *fileNameList, uint32_t *fileCount)
{
    if ((path == NULL) || (fileCount == NULL) || (fileNameList == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    void *dir = HksOpenDir(path);
    if (dir == NULL) {
        HKS_LOG_W("can't open directory");
        *fileCount = 0;
        return HKS_SUCCESS;
    }

    uint32_t count = 0;
    struct HksFileDirentInfo dire = { NULL };
    int32_t ret = HksGetDirFile(dir, &dire);
    while (ret == HKS_SUCCESS) {
        count++;
        uint32_t nameLen = strlen(dire.fileName);
        if ((*fileCount < count) || (fileNameList[count - 1].fileNameLen < (nameLen + 1))) {
            ret = HKS_ERROR_BUFFER_TOO_SMALL;
            break;
        }

        if (strncpy_s(fileNameList[count - 1].fileName, fileNameList[count - 1].fileNameLen,
            dire.fileName, nameLen) != EOK) {
            ret = HKS_ERROR_BAD_STATE;
            break;
        }
        fileNameList[count - 1].fileName[nameLen] = '\0';
        ret = HksGetDirFile(dir, &dire);
    }
    (void)HksCloseDir(dir);
    *fileCount = count;

    return HKS_SUCCESS;
}

static int32_t GetFilePath(const struct HksBlob *processName, uint32_t storageType, struct HksStoreFileInfo *fileInfo)
{
    char *name = (char *)HksMalloc(HKS_MAX_FILE_NAME_LEN);
    if (name == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(name, HKS_MAX_FILE_NAME_LEN, 0, HKS_MAX_FILE_NAME_LEN);

    int32_t ret = ConstructName(processName, name, HKS_MAX_FILE_NAME_LEN);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("construct name failed, ret = %d.", ret);
        HKS_FREE_PTR(name);
        return ret;
    }

    if (storageType == HKS_STORAGE_TYPE_KEY) {
        ret = GetStorePath(name, HKS_KEY_STORE_KEY_PATH, fileInfo);
    } else if (storageType == HKS_STORAGE_TYPE_CERTCHAIN) {
        ret = GetStorePath(name, HKS_KEY_STORE_CERTCHAIN_PATH, fileInfo);
    } else {
        ret = HKS_ERROR_NOT_SUPPORTED;
    }

    HKS_FREE_PTR(name);
    return ret;
}

static void FileNameListFree(struct HksFileEntry **fileNameList, uint32_t keyCount)
{
    for (uint32_t i = 0; i < keyCount; ++i) {
        HKS_FREE_PTR((*fileNameList)[i].fileName);
    }
    HKS_FREE_PTR(*fileNameList);
}

static int32_t FileNameListInit(struct HksFileEntry **fileNameList, uint32_t keyCount)
{
    if (((UINT32_MAX / (sizeof(struct HksFileEntry))) < keyCount) || (keyCount == 0)) {
        HKS_LOG_E("keyCount too big or is zero.");
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    uint32_t totalSize = keyCount * sizeof(struct HksFileEntry);
    *fileNameList = (struct HksFileEntry *)HksMalloc(totalSize);
    if (*fileNameList == NULL) {
        HKS_LOG_E("malloc file name list failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memset_s(*fileNameList, totalSize, 0, totalSize);
    int32_t ret = HKS_SUCCESS;

    for (uint32_t i = 0; i < keyCount; ++i) {
        (*fileNameList)[i].fileNameLen = HKS_MAX_FILE_NAME_LEN;
        (*fileNameList)[i].fileName = (char *)HksMalloc(HKS_MAX_FILE_NAME_LEN);
        if ((*fileNameList)[i].fileName == NULL) {
            HKS_LOG_E("malloc failed.");
            ret = HKS_ERROR_MALLOC_FAIL;
            break;
        }
    }

    if (ret != HKS_SUCCESS) {
        FileNameListFree(fileNameList, keyCount);
    }
    return ret;
}

static int32_t GetAndCheckFileCount(const char *path, uint32_t *fileCount, uint32_t *inputCount)
{
    int32_t ret = GetFileCount(path, fileCount);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get storage file count, ret = %d.", ret);
        return ret;
    }

    if (*inputCount < *fileCount) {
        HKS_LOG_E("listCount space not enough");
        ret = HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return ret;
}

static int32_t GetKeyAliasByProcessName(const struct HksStoreFileInfo *fileInfo, struct HksKeyInfo *keyInfoList,
    uint32_t *listCount)
{
    uint32_t fileCount;
    int32_t ret = GetAndCheckFileCount(fileInfo->mainPath.path, &fileCount, listCount);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    if (fileCount == 0) {
        *listCount = 0;
        return HKS_SUCCESS;
    }

    struct HksFileEntry *fileNameList = NULL;
    ret = FileNameListInit(&fileNameList, fileCount);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("init file name list failed.");
        return ret;
    }

    uint32_t realFileCount = fileCount;
    do {
        ret = GetFileNameList(fileInfo->mainPath.path, fileNameList, &realFileCount);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get file name list failed, ret = %d", ret);
            break;
        }

        for (uint32_t i = 0; i < realFileCount; ++i) {
            ret = ConstructBlob(fileNameList[i].fileName, &(keyInfoList[i].alias));
            if (ret != HKS_SUCCESS) {
                HKS_LOG_E("construct blob failed, ret = %d", ret);
                break;
            }
        }
    } while (0);

    FileNameListFree(&fileNameList, fileCount);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    *listCount = realFileCount;
    return ret;
}

int32_t HksGetKeyAliasByProcessName(const struct HksBlob *processName, struct HksKeyInfo *keyInfoList,
    uint32_t *listCount)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = FileInfoInit(&fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks file info init failed, ret = %d.", ret);
            break;
        }

        ret = GetFilePath(processName, HKS_STORAGE_TYPE_KEY, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }

        ret = GetKeyAliasByProcessName(&fileInfo, keyInfoList, listCount);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get key alias by processName failed, ret = %d.", ret);
        }
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

int32_t HksGetKeyCountByProcessName(const struct HksBlob *processName, uint32_t *fileCount)
{
    /* params have been checked by caller functions */
    struct HksStoreFileInfo fileInfo;
    (void)memset_s(&fileInfo, sizeof(fileInfo), 0, sizeof(fileInfo));

    int32_t ret;
    do {
        ret = FileInfoInit(&fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks file info init failed, ret = %d.", ret);
            break;
        }

        ret = GetFilePath(processName, HKS_STORAGE_TYPE_KEY, &fileInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks get file info failed, ret = %d.", ret);
            break;
        }

        ret = GetFileCount(fileInfo.mainPath.path, fileCount);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get storage file count failed, ret = %d.", ret);
        }
    } while (0);

    FileInfoFree(&fileInfo);
    return ret;
}

static int32_t DestoryType(const char *storePath, const char *typePath, uint32_t bakFlag)
{
    char *destoryPath = (char *)HksMalloc(HKS_MAX_FILE_NAME_LEN);
    if (destoryPath == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(destoryPath, HKS_MAX_FILE_NAME_LEN, 0, HKS_MAX_FILE_NAME_LEN);

    int32_t ret = GetPath(storePath, typePath, destoryPath, HKS_MAX_FILE_NAME_LEN, bakFlag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Get Path failed! ret = 0x%X", ret);
        HKS_FREE_PTR(destoryPath);
        return ret;
    }

    ret = HksIsFileExist(NULL, destoryPath);
    if (ret != HKS_SUCCESS) {
        HKS_FREE_PTR(destoryPath);
        return HKS_SUCCESS;
    }

    ret = HksRemoveDir(destoryPath);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Destory dir failed! ret = 0x%X", ret);
    }

    HKS_FREE_PTR(destoryPath);
    return ret;
}

static int32_t StoreDestory(const char *processNameEncoded, uint32_t bakFlag)
{
    char *storePath = (char *)HksMalloc(HKS_MAX_FILE_NAME_LEN);
    if (storePath == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(storePath, HKS_MAX_FILE_NAME_LEN, 0, HKS_MAX_FILE_NAME_LEN);

    int32_t ret;
    if (bakFlag == HKS_STORAGE_BAK_FLAG_TRUE) {
        ret = GetPath(HKS_KEY_STORE_BAK_PATH, processNameEncoded, storePath, HKS_MAX_FILE_NAME_LEN, bakFlag);
    } else {
        ret = GetPath(HKS_KEY_STORE_PATH, processNameEncoded, storePath, HKS_MAX_FILE_NAME_LEN, bakFlag);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Get Path failed! ret = 0x%X", ret);
        HKS_FREE_PTR(storePath);
        return ret;
    }

    ret = DestoryType(storePath, HKS_KEY_STORE_ROOT_KEY_PATH, bakFlag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_I("Destory info dir failed! ret = 0x%X", ret); /* continue delete */
    }

    ret = DestoryType(storePath, HKS_KEY_STORE_KEY_PATH, bakFlag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_I("Destory key dir failed! ret = 0x%X", ret); /* continue delete */
    }

    ret = DestoryType(storePath, HKS_KEY_STORE_CERTCHAIN_PATH, bakFlag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_I("Destory certchain dir failed! ret = 0x%X", ret); /* continue delete */
    }

    HKS_FREE_PTR(storePath);
    return HKS_SUCCESS;
}

int32_t HksStoreDestory(const struct HksBlob *processName)
{
    char *name = (char *)HksMalloc(HKS_MAX_FILE_NAME_LEN);
    if (name == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(name, HKS_MAX_FILE_NAME_LEN, 0, HKS_MAX_FILE_NAME_LEN);

    int32_t ret;
    do {
        ret = ConstructName(processName, name, HKS_MAX_FILE_NAME_LEN);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Construct process name failed! ret = 0x%X.", ret);
            break;
        }

        ret = StoreDestory(name, HKS_STORAGE_BAK_FLAG_FLASE);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Hks destory dir failed! ret = 0x%X.", ret);
            break;
        }

#ifdef SUPPORT_STORAGE_BACKUP
        ret = StoreDestory(name, HKS_STORAGE_BAK_FLAG_TRUE);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Hks destory back dir failed! ret = 0x%X.", ret);
            break;
        }
#endif
    } while (0);

    HKS_FREE_PTR(name);
    return ret;
}
