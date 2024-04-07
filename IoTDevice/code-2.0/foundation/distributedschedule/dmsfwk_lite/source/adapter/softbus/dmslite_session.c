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

#include "dmslite_session.h"

#include <unistd.h>

#include "distributed_service_interface.h"
#include "dmslite_devmgr.h"
#include "dmslite_inner_common.h"
#include "dmslite_log.h"
#include "dmslite_parser.h"

#include "discovery_service.h"
#include "ohos_errno.h"
#include "session.h"

#define DMS_SESSION_NAME "com.huawei.harmonyos.foundation.dms"
#define DMS_MODULE_NAME "dms"
#define ACCEPT_SESSION_OPEN 0

#define DMS_PUBLISHID 1
#define CAPABILITY "ddmpCapability"
#define CAPABILITY_DATA ""
#define CAPABILITY_DATA_LENGTH 0
#define MODULE_NAME "dms"

static void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);
static void OnSessionClosed(int32_t sessionId);
static int32_t OnSessionOpened(int32_t sessionId);

static void OnStartAbilityDone(int8_t errCode);
static void RegisterTcpCallback();

static void OnPublishSuccess(int32_t publishId);
static void OnPublishFail(int32_t publishId, PublishFailReason reason);

static PublishInfo g_publishInfo = {
    .publishId = DMS_PUBLISHID,
    .mode = DISCOVER_MODE_ACTIVE,
    .medium = COAP,
    .freq = MID,
    .capability = CAPABILITY,
    .capabilityData = (unsigned char *)CAPABILITY_DATA,
    .dataLen = CAPABILITY_DATA_LENGTH,
};

static IPublishCallback g_publishCallback = {
    .onPublishSuccess = OnPublishSuccess,
    .onPublishFail = OnPublishFail,
};

static struct ISessionListener g_sessionCallback = {
    .onBytesReceived = OnBytesReceived,
    .onSessionOpened = OnSessionOpened,
    .onSessionClosed = OnSessionClosed
};

static IDmsFeatureCallback g_dmsFeatureCallback = {
    /* in non-test mode, there is no need set a TlvParseCallback */
    .onTlvParseDone = NULL,
    .onStartAbilityDone = OnStartAbilityDone,
};

void OnStartAbilityDone(int8_t errCode)
{
    HILOGD("[onStartAbilityDone errCode = %d]", errCode);
}

void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    CommuMessage commuMessage;
    commuMessage.payloadLength = dataLen;
    commuMessage.payload = data;

    int32_t errCode = ProcessCommuMsg(&commuMessage, &g_dmsFeatureCallback);
    HILOGI("[ProcessCommuMsg errCode = %d]", errCode);
}

void OnSessionClosed(int32_t sessionId)
{
    HILOGD("[function called]");
}

int32_t OnSessionOpened(int32_t sessionId)
{
    HILOGD("[function called]");
    /* only when we explicitly accept can the incoming session open successfully */
    return ACCEPT_SESSION_OPEN;
}

static void OnPublishSuccess(int32_t publishId)
{
    RegisterTcpCallback();
    HILOGI("[dms service publish success]");
}

static void OnPublishFail(int32_t publishId, PublishFailReason reason)
{
    HILOGW("[dms service publish failed reason = %d]", (int32_t) reason);
}

void InitSoftbusService()
{
    int32_t ret = PublishService(MODULE_NAME, &g_publishInfo, &g_publishCallback);
    if (ret != EC_SUCCESS) {
        HILOGW("[PublishService failed]");
    }
}

void RegisterTcpCallback()
{
    if (getuid() != FOUNDATION_UID) {
        HILOGE("[Only dtbschedsrv can register dms bus]");
        return;
    }

    int32_t errCode = CreateSessionServer(DMS_MODULE_NAME, DMS_SESSION_NAME, &g_sessionCallback);
    HILOGD("[Register %s, errCode = %d]", (errCode == EC_SUCCESS) ? "success" : "failed", errCode);
}

void HandleBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    HILOGD("[function called]");
}

void HandleSessionClosed(int32_t sessionId)
{
    HILOGD("[function called]");
}

int32_t HandleSessionOpened(int32_t sessionId)
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

int32_t CreateDMSSessionServer()
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

int32_t CloseDMSSessionServer()
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

int32_t OpenDMSSession()
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

int32_t SendDmsMessage(char *data, int32_t len)
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

void CloseDMSSession()
{
    HILOGD("[function called]");
}

int32_t AddDevMgrListener()
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

int32_t UnRegisterDevMgrListener()
{
    HILOGD("[function called]");
    return EC_SUCCESS;
}

char* GetPeerId()
{
    return "";
}