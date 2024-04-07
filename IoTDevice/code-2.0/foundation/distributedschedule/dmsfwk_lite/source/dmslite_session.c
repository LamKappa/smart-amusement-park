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

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "dmsfwk_interface.h"
#include "dmslite_devmgr.h"
#include "dmslite_feature.h"
#include "dmslite_log.h"
#include "dmslite_pack.h"
#include "dmslite_parser.h"
#include "dmslite_utils.h"

#include "securec.h"
#include "softbus_common.h"
#include "softbus_session.h"
#include "softbus_sys.h"

#define DMS_SESSION_NAME "com.huawei.harmonyos.foundation.dms"
#define DMS_MODULE_NAME "dms"

#define TIME_SS_US 1000000
#define TIME_US_MS 1000
#define TIME_US_NS 1000000000
#define TIME_OUT 10000
#define INVALID_SESSION_ID (-1)
#define MAX_DATA_SIZE 256

static int32_t g_curSessionId = INVALID_SESSION_ID;
static bool g_curBusy = false;

/* session callback */
static void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);
static void OnSessionClosed(int32_t sessionId);
static int32_t OnSessionOpened(int32_t sessionId, int result);
static void OnMessageReceived(int sessionId, const void *data, unsigned int len);

static void NotifyConnected(void);
static void WaitForConnected(int timeOut);
static void GetCondTime(struct timespec *tv, int timeDelay);

static void OnStartAbilityDone(int8_t errCode);

static ISessionListener g_sessionCallback = {
    .onBytesReceived = OnBytesReceived,
    .onSessionOpened = OnSessionOpened,
    .onSessionClosed = OnSessionClosed,
    .onMessageReceived = OnMessageReceived
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

void InitSoftbusService()
{
    InitSoftBus(DMS_MODULE_NAME);
    AddDevMgrListener();
}

void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    if (dataLen > MAX_DATA_SIZE) {
        return;
    }
    char *message = (char *)DMS_ALLOC(dataLen);
    if (message == NULL) {
        return;
    }
    if (strncpy_s(message, dataLen, (char *)data, dataLen) != EOK) {
        return;
    }
    Request request = {
        .msgId = BYTES_RECEIVED,
        .len = dataLen,
        .data = message,
        .msgValue = sessionId
    };
    int32_t result = SAMGR_SendRequest((const Identity*)&(GetDmsLiteFeature()->identity), &request, NULL);
    if (result != EC_SUCCESS) {
        HILOGD("[OnBytesReceived errCode = %d]", result);
    }
}

void HandleBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    CommuMessage commuMessage;
    commuMessage.payloadLength = dataLen;
    commuMessage.payload = (uint8_t *)data;
    int32_t errCode = ProcessCommuMsg(&commuMessage, &g_dmsFeatureCallback);
    HILOGI("[ProcessCommuMsg errCode = %d]", errCode);
}

void OnSessionClosed(int32_t sessionId)
{
    Request request = {
        .msgId = SESSION_CLOSE,
        .len = 0,
        .data = NULL,
        .msgValue = sessionId
    };
    int32_t result = SAMGR_SendRequest((const Identity*)&(GetDmsLiteFeature()->identity), &request, NULL);
    if (result != EC_SUCCESS) {
        HILOGD("[OnSessionClosed errCode = %d]", result);
    }
}

void HandleSessionClosed(int32_t sessionId)
{
    if (g_curSessionId == sessionId) {
        g_curSessionId = INVALID_SESSION_ID;
        g_curBusy = false;
    }
}

int32_t OnSessionOpened(int32_t sessionId, int result)
{
    Request request = {
        .msgId = SESSION_OPEN,
        .len = 0,
        .data = NULL,
        .msgValue = sessionId
    };
    return SAMGR_SendRequest((const Identity*)&(GetDmsLiteFeature()->identity), &request, NULL);
}

int32_t HandleSessionOpened(int32_t sessionId)
{
    if (g_curSessionId != sessionId) {
        return EC_SUCCESS;
    }
    int32_t ret = SendBytes(g_curSessionId, data, len);
    CleanBuild();
    return ret;
}

void OnMessageReceived(int sessionId, const void *data, unsigned int len)
{
    return;
}

int32_t CreateDMSSessionServer()
{
    return CreateSessionServer(DMS_MODULE_NAME, DMS_SESSION_NAME, &g_sessionCallback);
}

int32_t CloseDMSSessionServer()
{
    return RemoveSessionServer(DMS_MODULE_NAME, DMS_SESSION_NAME);
}

int32_t SendDmsMessage(char *data, int32_t len)
{
    HILOGI("[SendMessage]");
    if (g_curBusy) {
        return EC_FAILURE;
    }
    g_curBusy = true;
    SessionAttribute attr = { .dataType = TYPE_BYTES };
    g_curSessionId = OpenSession(DMS_SESSION_NAME, DMS_SESSION_NAME, GetPeerId(), DMS_MODULE_NAME, &attr);
    if (g_curSessionId < 0) {
        return EC_FAILURE;
    }
    return EC_SUCCESS;
}

void CloseDMSSession()
{
    CloseSession(g_curSessionId);
    g_curSessionId = INVALID_SESSION_ID;
    g_curBusy = false;
}
