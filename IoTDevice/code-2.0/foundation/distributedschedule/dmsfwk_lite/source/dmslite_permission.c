/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "dmslite_permission.h"

#include <unistd.h>

#include "dmsfwk_interface.h"
#include "dmslite_log.h"

#ifdef APP_PLATFORM_WATCHGT
#include "bundle_manager.h"
#else
#include "bundle_inner_interface.h"
#endif
#include "samgr_lite.h"
#include "securec.h"

#define DELIMITER_LENGTH 1
#define GET_BUNDLE_WITHOUT_ABILITIES 0

#ifndef APP_PLATFORM_WATCHGT
static bool GetBmsInterface(struct BmsServerProxy **bmsInterface)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(BMS_SERVICE, BMS_FEATURE);
    if (iUnknown == NULL) {
        HILOGE("[GetFeatureApi failed]");
        return false;
    }

    int32_t errCode = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **) bmsInterface);
    if (errCode != EC_SUCCESS) {
        HILOGE("[QueryInterface failed]");
        return false;
    }

    return true;
}
#endif

int32_t CheckRemotePermission(const PermissionCheckInfo *permissionCheckInfo)
{
    if (permissionCheckInfo == NULL) {
        return DMS_EC_FAILURE;
    }

    BundleInfo bundleInfo;
    if (memset_s(&bundleInfo, sizeof(BundleInfo), 0x00, sizeof(BundleInfo)) != EOK) {
        HILOGE("[bundleInfo memset failed]");
        return DMS_EC_FAILURE;
    }

    int32_t errCode;
#ifndef APP_PLATFORM_WATCHGT
    uid_t callerUid = getuid();
    if (callerUid == FOUNDATION_UID) {
        /* inner-process mode */
        struct BmsServerProxy *bmsInterface = NULL;
        if (!GetBmsInterface(&bmsInterface)) {
            HILOGE("[GetBmsInterface query null]");
            return DMS_EC_GET_BMS_FAILURE;
        }
        errCode = bmsInterface->GetBundleInfo(permissionCheckInfo->calleeBundleName,
            GET_BUNDLE_WITHOUT_ABILITIES, &bundleInfo);
    } else if (callerUid == SHELL_UID) {
        /* inter-process mode (mainly called in xts testsuit process started by shell) */
        errCode = GetBundleInfo(permissionCheckInfo->calleeBundleName,
            GET_BUNDLE_WITHOUT_ABILITIES, &bundleInfo);
    } else {
        errCode = EC_FAILURE;
    }
#else
    errCode = GetBundleInfo(permissionCheckInfo->calleeBundleName,
        GET_BUNDLE_WITHOUT_ABILITIES, &bundleInfo);
#endif
    if (errCode != EC_SUCCESS) {
        HILOGE("[GetBundleInfo errCode = %d]", errCode);
        return DMS_EC_GET_BUNDLEINFO_FAILURE;
    }

    /* appId: bundleName + "_" + signature */
    const char *calleeSignature = bundleInfo.appId + strlen(permissionCheckInfo->calleeBundleName)
        + DELIMITER_LENGTH;
    if ((permissionCheckInfo->callerSignature == NULL) || (calleeSignature == NULL)) {
        HILOGE("[Signature is null]");
        return DMS_EC_FAILURE;
    }

    if (strcmp(permissionCheckInfo->callerSignature, calleeSignature) != 0) {
        HILOGE("[Signature unmatched]");
        return DMS_EC_CHECK_PERMISSION_FAILURE;
    }

    return DMS_EC_SUCCESS;
}
