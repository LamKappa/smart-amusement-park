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

#include "dmslite_famgr.h"

#include <malloc.h>

#include "dmslite_feature.h"
#include "dmslite_log.h"
#include "dmslite_pack.h"
#include "dmslite_session.h"
#include "dmslite_tlv_common.h"
#include "dmslite_utils.h"

#include "ohos_errno.h"
#include "securec.h"

int32_t StartAbilityFromRemote(const char *bundleName, const char *abilityName,
    StartAbilityCallback onStartAbilityDone)
{
    return EC_SUCCESS;
}

int32_t StartRemoteAbilityInner(Want *want, AbilityInfo *abilityInfo)
{
    Want *reqdata = (Want *)DMS_ALLOC(sizeof(Want));
    if (memset_s(reqdata, sizeof(Want), 0x00, sizeof(Want)) != EOK) {
        HILOGE("[want memset failed]");
        return DMS_EC_FAILURE;
    }
    want->data = (void *)abilityInfo->bundleName;
    want->dataLength = strlen(abilityInfo->bundleName);
    reqdata->data = want->data;
    reqdata->element = want->element;
    reqdata->dataLength = want->dataLength;
    Request request = {
        .msgId = START_REMOTE_ABILITY,
        .data = (void *)reqdata,
        .len = sizeof(Want),
        .msgValue = 0
    };
    return SAMGR_SendRequest((const Identity*)&(GetDmsLiteFeature()->identity), &request, NULL);
}

int32_t StartRemoteAbility(const Want *want)
{
    HILOGE("[StartRemoteAbility]");
    if (want == NULL || want->data == NULL) {
        return DMS_EC_INVALID_PARAMETER;
    }
    char *bundleName = (char *)want->data;
    BundleInfo bundleInfo;
    if (memset_s(&bundleInfo, sizeof(BundleInfo), 0x00, sizeof(BundleInfo)) != EOK) {
        HILOGE("[bundleInfo memset failed]");
        return DMS_EC_FAILURE;
    }
    GetBundleInfo(bundleName, 0, &bundleInfo);
    PreprareBuild();
    PACKET_MARSHALL_HELPER(Uint16, COMMAND_ID, DMS_MSG_CMD_START_FA);
    PACKET_MARSHALL_HELPER(String, CALLEE_BUNDLE_NAME, want->element->bundleName);
    PACKET_MARSHALL_HELPER(String, CALLEE_ABILITY_NAME, want->element->abilityName);
    if (bundleInfo.appId != NULL) {
        PACKET_MARSHALL_HELPER(String, CALLER_SIGNATURE, bundleInfo.appId);
    } else {
        PACKET_MARSHALL_HELPER(String, CALLER_SIGNATURE, "");
    }
    HILOGE("[StartRemoteAbility len:%d]", GetPacketSize());
    int32_t ret = SendDmsMessage(GetPacketBufPtr(), GetPacketSize());
    return ret;
}
