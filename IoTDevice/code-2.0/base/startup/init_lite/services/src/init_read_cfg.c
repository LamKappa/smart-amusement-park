/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "init_read_cfg.h"

#include <errno.h>
#include <linux/capability.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "init_jobs.h"
#include "init_perms.h"
#include "init_service_manager.h"
#include "securec.h"
#ifndef __LINUX__
#ifdef OHOS_LITE
#include "init_stage.h"
#endif
#endif

static const long MAX_JSON_FILE_LEN = 102400;    // max init.cfg size 100KB
static const int  MAX_PATH_ARGS_CNT = 20;        // max path and args count
static const int  MAX_ONE_ARG_LEN   = 64;        // max length of one param/path
#define MAX_SERVICES_CNT_IN_FILE 100
#define MAX_CAPS_CNT_FOR_ONE_SERVICE 100
#define UID_STR_IN_CFG        "uid"
#define GID_STR_IN_CFG        "gid"
#define ONCE_STR_IN_CFG       "once"
#define IMPORTANT_STR_IN_CFG  "importance"
#define BIN_SH_NOT_ALLOWED    "/bin/sh"

static char* ReadFileToBuf()
{
    char* buffer = NULL;
    FILE* fd = NULL;
    struct stat fileStat = {0};
    do {
        if (stat(INIT_CONFIGURATION_FILE, &fileStat) != 0 ||
            fileStat.st_size <= 0 || fileStat.st_size > MAX_JSON_FILE_LEN) {
            break;
        }

        fd = fopen(INIT_CONFIGURATION_FILE, "r");
        if (fd == NULL) {
            break;
        }

        buffer = (char*)malloc(fileStat.st_size + 1);
        if (buffer == NULL) {
            break;
        }

        if (fread(buffer, fileStat.st_size, 1, fd) != 1) {
            free(buffer);
            buffer = NULL;
            break;
        }
        buffer[fileStat.st_size] = '\0';
    } while (0);

    if (fd != NULL) {
        fclose(fd);
        fd = NULL;
    }
    return buffer;
}

static cJSON* GetArrItem(const cJSON* fileRoot, int* arrSize, const char* arrName)
{
    cJSON* arrItem = cJSON_GetObjectItemCaseSensitive(fileRoot, arrName);
    if (!cJSON_IsArray(arrItem)) {
        printf("[Init] GetArrItem, item %s is not an array!\n", arrName);
        return NULL;
    }

    *arrSize = cJSON_GetArraySize(arrItem);
    if (*arrSize <= 0) {
        return NULL;
    }
    return arrItem;
}

static int IsForbidden(const char* fieldStr)
{
    size_t fieldLen = strlen(fieldStr);
    size_t forbidStrLen = strlen(BIN_SH_NOT_ALLOWED);
    if (fieldLen == forbidStrLen) {
        if (strncmp(fieldStr, BIN_SH_NOT_ALLOWED, fieldLen) == 0) {
            return 1;
        }
        return 0;
    } else if (fieldLen > forbidStrLen) {
        // "/bin/shxxxx" is valid but "/bin/sh xxxx" is invalid
        if (strncmp(fieldStr, BIN_SH_NOT_ALLOWED, forbidStrLen) == 0) {
            if (fieldStr[forbidStrLen] == ' ') {
                return 1;
            }
        }
        return 0;
    } else {
        return 0;
    }
}

static void ReleaseServiceMem(Service* curServ)
{
    if (curServ->pathArgs != NULL) {
        for (int i = 0; i < curServ->pathArgsCnt; ++i) {
            if (curServ->pathArgs[i] != NULL) {
                free(curServ->pathArgs[i]);
                curServ->pathArgs[i] = NULL;
            }
        }
        free(curServ->pathArgs);
        curServ->pathArgs = NULL;
    }
    curServ->pathArgsCnt = 0;

    if (curServ->servPerm.caps != NULL) {
        free(curServ->servPerm.caps);
        curServ->servPerm.caps = NULL;
    }
    curServ->servPerm.capsCnt = 0;
}

static int GetServiceName(const cJSON* curArrItem, Service* curServ)
{
    char* fieldStr = cJSON_GetStringValue(cJSON_GetObjectItem(curArrItem, "name"));
    if (fieldStr == NULL) {
        return SERVICE_FAILURE;
    }

    size_t strLen = strlen(fieldStr);
    if (strLen == 0 || strLen > MAX_SERVICE_NAME) {
        return SERVICE_FAILURE;
    }

    if (memcpy_s(curServ->name, MAX_SERVICE_NAME, fieldStr, strLen) != EOK) {
        return SERVICE_FAILURE;
    }
    curServ->name[strLen] = '\0';
    return SERVICE_SUCCESS;
}

static int GetServicePathAndArgs(const cJSON* curArrItem, Service* curServ)
{
    cJSON* pathItem = cJSON_GetObjectItem(curArrItem, "path");
    if (!cJSON_IsArray(pathItem)) {
        return SERVICE_FAILURE;
    }

    int arrSize = cJSON_GetArraySize(pathItem);
    if (arrSize <= 0 || arrSize > MAX_PATH_ARGS_CNT) {  // array size invalid
        return SERVICE_FAILURE;
    }

    curServ->pathArgs = (char**)malloc((arrSize + 1) * sizeof(char*));
    if (curServ->pathArgs == NULL) {
        return SERVICE_FAILURE;
    }
    for (int i = 0; i < arrSize + 1; ++i) {
        curServ->pathArgs[i] = NULL;
    }
    curServ->pathArgsCnt = arrSize + 1;

    for (int i = 0; i < arrSize; ++i) {
        char* curParam = cJSON_GetStringValue(cJSON_GetArrayItem(pathItem, i));
        if (curParam == NULL || strlen(curParam) > MAX_ONE_ARG_LEN) {
            // resources will be released by function: ReleaseServiceMem
            return SERVICE_FAILURE;
        }

        if (i == 0 && IsForbidden(curParam)) {
            // resources will be released by function: ReleaseServiceMem
            return SERVICE_FAILURE;
        }

        size_t paramLen = strlen(curParam);
        curServ->pathArgs[i] = (char*)malloc(paramLen + 1);
        if (curServ->pathArgs[i] == NULL) {
            // resources will be released by function: ReleaseServiceMem
            return SERVICE_FAILURE;
        }

        if (memcpy_s(curServ->pathArgs[i], paramLen + 1, curParam, paramLen) != EOK) {
            // resources will be released by function: ReleaseServiceMem
            return SERVICE_FAILURE;
        }
        curServ->pathArgs[i][paramLen] = '\0';
    }
    return SERVICE_SUCCESS;
}

static int GetServiceNumber(const cJSON* curArrItem, Service* curServ, const char* targetField)
{
    cJSON* filedJ = cJSON_GetObjectItem(curArrItem, targetField);
    if (!cJSON_IsNumber(filedJ)) {
        return SERVICE_FAILURE;
    }

    int value = (int)cJSON_GetNumberValue(filedJ);
    if (value < 0) {
        return SERVICE_FAILURE;
    }

    if (strncmp(targetField, UID_STR_IN_CFG, strlen(UID_STR_IN_CFG)) == 0) {
        curServ->servPerm.uID = value;
    } else if (strncmp(targetField, GID_STR_IN_CFG, strlen(GID_STR_IN_CFG)) == 0) {
        curServ->servPerm.gID = value;
    } else if (strncmp(targetField, ONCE_STR_IN_CFG, strlen(ONCE_STR_IN_CFG)) == 0) {
        if (value != 0) {
            curServ->attribute |= SERVICE_ATTR_ONCE;
        }
    } else if (strncmp(targetField, IMPORTANT_STR_IN_CFG, strlen(IMPORTANT_STR_IN_CFG)) == 0) {
        if (value != 0) {
            curServ->attribute |= SERVICE_ATTR_IMPORTANT;
        }
    } else {
        return SERVICE_FAILURE;
    }
    return SERVICE_SUCCESS;
}

static int GetServiceCaps(const cJSON* curArrItem, Service* curServ)
{
    curServ->servPerm.capsCnt = 0;
    curServ->servPerm.caps = NULL;

    cJSON* filedJ = cJSON_GetObjectItem(curArrItem, "caps");
    if (!cJSON_IsArray(filedJ)) {
        return SERVICE_FAILURE;
    }

    // caps array does not exist, means do not need any capability
    int capsCnt = cJSON_GetArraySize(filedJ);
    if (capsCnt <= 0) {
        return SERVICE_SUCCESS;
    }

    if (capsCnt > MAX_CAPS_CNT_FOR_ONE_SERVICE) {
        printf("[Init] GetServiceCaps, too many caps[cnt %d] for one service, should not exceed %d.\n",
            capsCnt, MAX_CAPS_CNT_FOR_ONE_SERVICE);
        return SERVICE_FAILURE;
    }

    curServ->servPerm.caps = (unsigned int*)malloc(sizeof(unsigned int) * capsCnt);
    if (curServ->servPerm.caps == NULL) {
        return SERVICE_FAILURE;
    }

    for (int i = 0; i < capsCnt; ++i) {
        cJSON* capJ = cJSON_GetArrayItem(filedJ, i);
        if (!cJSON_IsNumber(capJ) || cJSON_GetNumberValue(capJ) < 0) {
            // resources will be released by function: ReleaseServiceMem
            return SERVICE_FAILURE;
        }
        curServ->servPerm.caps[i] = (unsigned int)cJSON_GetNumberValue(capJ);
        if (curServ->servPerm.caps[i] > CAP_LAST_CAP && curServ->servPerm.caps[i] != FULL_CAP) {
            // resources will be released by function: ReleaseServiceMem
            return SERVICE_FAILURE;
        }
    }
    curServ->servPerm.capsCnt = capsCnt;
    return SERVICE_SUCCESS;
}

static void ParseAllServices(const cJSON* fileRoot)
{
    int servArrSize = 0;
    cJSON* serviceArr = GetArrItem(fileRoot, &servArrSize, SERVICES_ARR_NAME_IN_JSON);
    if (serviceArr == NULL) {
        printf("[Init] InitReadCfg, get array %s failed.\n", SERVICES_ARR_NAME_IN_JSON);
        return;
    }

    if (servArrSize > MAX_SERVICES_CNT_IN_FILE) {
        printf("[Init] InitReadCfg, too many services[cnt %d] detected, should not exceed %d.\n",
            servArrSize, MAX_SERVICES_CNT_IN_FILE);
        return;
    }

    Service* retServices = (Service*)malloc(sizeof(Service) * servArrSize);
    if (retServices == NULL) {
        printf("[Init] InitReadCfg, malloc for %s arr failed! %d.\n", SERVICES_ARR_NAME_IN_JSON, servArrSize);
        return;
    }

    if (memset_s(retServices, sizeof(Service) * servArrSize, 0, sizeof(Service) * servArrSize) != EOK) {
        free(retServices);
        retServices = NULL;
        return;
    }

    for (int i = 0; i < servArrSize; ++i) {
        cJSON* curItem = cJSON_GetArrayItem(serviceArr, i);
        if (GetServiceName(curItem, &retServices[i]) != SERVICE_SUCCESS ||
            GetServicePathAndArgs(curItem, &retServices[i]) != SERVICE_SUCCESS ||
            GetServiceNumber(curItem, &retServices[i], UID_STR_IN_CFG) != SERVICE_SUCCESS ||
            GetServiceNumber(curItem, &retServices[i], GID_STR_IN_CFG) != SERVICE_SUCCESS ||
            GetServiceNumber(curItem, &retServices[i], ONCE_STR_IN_CFG) != SERVICE_SUCCESS ||
            GetServiceNumber(curItem, &retServices[i], IMPORTANT_STR_IN_CFG) != SERVICE_SUCCESS ||
            GetServiceCaps(curItem, &retServices[i]) != SERVICE_SUCCESS) {
            // release resources if it fails
            ReleaseServiceMem(&retServices[i]);
            retServices[i].attribute |= SERVICE_ATTR_INVALID;
            printf("[Init] InitReadCfg, parse information for service %d failed.\n", i);
            continue;
        }
    }
    RegisterServices(retServices, servArrSize);
}

void InitReadCfg()
{
    // read configuration file in json format
    char* fileBuf = ReadFileToBuf();
    if (fileBuf == NULL) {
        printf("[Init] InitReadCfg, read file %s failed! err %d.\n", INIT_CONFIGURATION_FILE, errno);
        return;
    }

    cJSON* fileRoot = cJSON_Parse(fileBuf);
    free(fileBuf);
    fileBuf = NULL;

    if (fileRoot == NULL) {
        printf("[Init] InitReadCfg, parse failed! please check file %s format.\n", INIT_CONFIGURATION_FILE);
        return;
    }

    // parse services
    ParseAllServices(fileRoot);

    // parse jobs
    ParseAllJobs(fileRoot);

    // release memory
    cJSON_Delete(fileRoot);

    // do jobs
    DoJob("pre-init");
#ifndef __LINUX__
#ifdef OHOS_LITE
    TriggerStage(EVENT1, EVENT1_WAITTIME, QS_STAGE1);
#endif
#endif

    DoJob("init");
#ifndef __LINUX__
#ifdef OHOS_LITE
    TriggerStage(EVENT2, EVENT2_WAITTIME, QS_STAGE2);
#endif
#endif

    DoJob("post-init");
#ifndef __LINUX__
#ifdef OHOS_LITE
    TriggerStage(EVENT3, EVENT3_WAITTIME, QS_STAGE3);

    InitStageFinished();
#endif
#endif
    ReleaseAllJobs();
}

