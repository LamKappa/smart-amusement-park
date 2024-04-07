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

#include "parameter.h"

#include <securec.h>
#include <string.h>

#include "parameter_hal.h"
#include "sysparam_errno.h"
#include "sysversion.h"

#define OS_FULL_NAME_LEN 128
#define VERSION_ID_MAX_LEN 256

static const char OHOS_OS_NAME[] = {"OpenHarmony"};
static const int OHOS_SDK_API_LEVEL = 6;
static const char OHOS_BUILD_ROOT_HASH[] = {"****"};
static const char OHOS_SECURITY_PATCH_TAG[] = {"2020-09-01"};
static const char OHOS_RELEASE_TYPE[] = {"Canary1"};

static const char EMPTY_STR[] = {""};

int GetParameter(const char *key, const char *def, char *value, unsigned int len)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    int ret = HalGetParameter(key, def, value, len);
    return (ret < 0) ? ret : strlen(value);
}

int SetParameter(const char *key, const char *value)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    return HalSetParameter(key, value);
}

const char *GetDeviceType()
{
    return HalGetDeviceType();
}

const char *GetProductModel()
{
    return HalGetProductModel();
}

const char *GetManufacture()
{
    return HalGetManufacture();
}

const char *GetBrand()
{
    return HalGetBrand();
}

const char *GetMarketName()
{
    return HalGetMarketName();
}

const char *GetProductSeries()
{
    return HalGetProductSeries();
}

const char *GetSoftwareModel()
{
    return HalGetSoftwareModel();
}

const char *GetHardwareModel()
{
    return HalGetHardwareModel();
}

const char *GetHardwareProfile()
{
    return HalGetHardwareProfile();
}

const char *GetSerial()
{
    return HalGetSerial();
}

const char *GetSecurityPatchTag()
{
    return OHOS_SECURITY_PATCH_TAG;
}

const char *GetAbiList()
{
    return HalGetAbiList();
}

const char *GetBootloaderVersion()
{
    return HalGetBootloaderVersion();
}

static const char* BuildOSFullName(void)
{
    const char release[] = "Release";
    char value[OS_FULL_NAME_LEN];
    const char* releaseType = GetOsReleaseType();
    int length;
    if (strncmp(releaseType, release, sizeof(release) - 1) == 0) {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d",
            OHOS_OS_NAME, GetMajorVersion(), GetSeniorVersion(), GetFeatureVersion(), GetBuildVersion());
    } else {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d(%s)",
            OHOS_OS_NAME, GetMajorVersion(), GetSeniorVersion(), GetFeatureVersion(), GetBuildVersion(), releaseType);
    }
    if (length < 0) {
        return EMPTY_STR;
    }
    const char* osFullName = strdup(value);
    return osFullName;
}

const char *GetOSFullName()
{
    static const char *osFullName = NULL;
    if (osFullName != NULL) {
        return osFullName;
    }
    osFullName = BuildOSFullName();
    if (osFullName == NULL) {
        return EMPTY_STR;
    }
    return osFullName;
}

int GetSdkApiVersion()
{
    return OHOS_SDK_API_LEVEL;
}

int GetFirstApiVersion()
{
    return HalGetFirstApiVersion();
}

const char *GetDisplayVersion()
{
    return HalGetDisplayVersion();
}

const char *GetIncrementalVersion()
{
    return HalGetIncrementalVersion();
}

static const char* BuildVersionId(void)
{
    char value[VERSION_ID_MAX_LEN];
    int len = sprintf_s(value, VERSION_ID_MAX_LEN, "%s/%s/%s/%s/%s/%s/%s/%d/%s/%s",
        GetDeviceType(), GetManufacture(), GetBrand(), GetProductSeries(),
        GetOSFullName(), GetProductModel(), GetSoftwareModel(),
        OHOS_SDK_API_LEVEL, GetIncrementalVersion(), GetBuildType());
    if (len < 0) {
        return EMPTY_STR;
    }
    const char* versionId = strdup(value);
    return versionId;
}

const char *GetVersionId()
{
    static const char *ohosVersionId = NULL;
    if (ohosVersionId != NULL) {
        return ohosVersionId;
    }
    ohosVersionId = BuildVersionId();
    if (ohosVersionId == NULL) {
        return EMPTY_STR;
    }
    return ohosVersionId;
}

const char *GetBuildType()
{
    return HalGetBuildType();
}

const char *GetBuildUser()
{
    return HalGetBuildUser();
}

const char *GetBuildHost()
{
    return HalGetBuildHost();
}

const char *GetBuildTime()
{
    return HalGetBuildTime();
}

const char *GetBuildRootHash()
{
    return OHOS_BUILD_ROOT_HASH;
}

const char *GetOsReleaseType()
{
    return OHOS_RELEASE_TYPE;
}