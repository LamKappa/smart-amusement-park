/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "soft_bus_channel.h"
#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_log.h"

typedef struct {
    void (*onChannelOpened)(int64_t channelId, int64_t requestId, const char *deviceId,
        uint32_t deviceIdLen, bool isServer);
    void (*onChannelClosed)(int64_t channelId, int64_t requestId);
    void (*onMsgReceived)(int64_t channelId, const uint8_t *data, uint32_t dataLen);
    void (*onServiceDied)();
} IAuthMsgListener;

typedef struct {
    int (*addListener)(const char *packageName, const IAuthMsgListener *listener);
    int (*removeListener)(const char *packageName);
    int64_t (*openAuthChannel)(const char *packageName, const char *authInfo, int64_t requestId);
    int (*closeChannel)(int64_t channelId);
    int (*sendMsg)(int64_t channelId, const uint8_t *data, uint32_t len);
    int (*setAuthResult)(int64_t channelId, const uint8_t *data, uint32_t len);
    char *(*getLocalAuthInfo)();
} AuthMsgService;

#define NORMAL_CHANNEL_ID 1

static int AddListener(const char *packageName, const IAuthMsgListener *listener)
{
    (void)packageName;
    (void)listener;
    return HC_SUCCESS;
}

static int RemoveListener(const char *packageName)
{
    (void)packageName;
    return HC_SUCCESS;
}

static int64_t OpenAuthChannel(const char *packageName, const char *connectParams, int64_t requestId)
{
    (void)packageName;
    (void)connectParams;
    (void)requestId;
    return NORMAL_CHANNEL_ID;
}

static int CloseChannel(int64_t channelId)
{
    (void)channelId;
    return HC_SUCCESS;
}

static int SendMsg(int64_t channelId, const uint8_t *data, uint32_t dataLen)
{
    (void)channelId;
    (void)data;
    (void)dataLen;
    return HC_SUCCESS;
}

static int SetAuthResult(int64_t channelId, const uint8_t *data, uint32_t dataLen)
{
    (void)channelId;
    (void)data;
    (void)dataLen;
    return HC_SUCCESS;
}

static char *GetLocalAuthInfo()
{
    return "";
}

static AuthMsgService g_softBusInstance = {
    .addListener = AddListener,
    .removeListener = RemoveListener,
    .openAuthChannel = OpenAuthChannel,
    .closeChannel = CloseChannel,
    .sendMsg = SendMsg,
    .setAuthResult = SetAuthResult,
    .getLocalAuthInfo = GetLocalAuthInfo
};

static AuthMsgService *GetInstance(const char *packageName)
{
    (void)packageName;
    return &g_softBusInstance;
}

static int32_t OpenSoftBusChannel(const char *connectParams, int64_t requestId, int64_t *returnChannelId)
{
    if (returnChannelId == NULL) {
        LOGE("The address of the input returnChannelId is NULL!");
        return HC_ERR_NULL_PTR;
    }
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGE("Failed to get soft bus instance!");
        return HC_ERR_SOFT_BUS;
    }
    LOGI("Ready to open the soft bus channel!");
    int64_t channelId = instance->openAuthChannel(GROUP_MANAGER_PACKAGE_NAME, connectParams, requestId);
    /* If the value of channelId is less than 0, the soft bus fails to open the channel */
    if (channelId < 0) {
        LOGE("Failed to open soft bus channel!");
        return HC_ERR_SOFT_BUS;
    }
    LOGI("Open soft bus channel successfully!");
    *returnChannelId = channelId;
    return HC_SUCCESS;
}

static void CloseSoftBusChannel(int64_t channelId)
{
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGE("Failed to get soft bus instance!");
        return;
    }
    LOGI("Ready to close the soft bus channel!");
    int32_t result = instance->closeChannel(channelId);
    if (result != HC_SUCCESS) {
        LOGE("Failed to close soft bus channel!");
    }
    LOGI("Close soft bus channel success!");
}

static int32_t SendSoftBusMsg(int64_t channelId, const uint8_t *data, uint32_t dataLen)
{
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGE("Failed to get soft bus instance!");
        return HC_ERR_SOFT_BUS;
    }
    LOGI("The soft bus channel starts to send data!");
    int32_t result = instance->sendMsg(channelId, data, dataLen);
    if (result != HC_SUCCESS) {
        LOGE("An error occurs when the soft bus channel sends data!");
        return HC_ERR_SOFT_BUS;
    }
    LOGI("The soft bus channel sends data successfully!");
    return HC_SUCCESS;
}

static void SetSoftBusAuthResult(int64_t channelId)
{
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGE("Failed to get soft bus instance!");
        return;
    }
    LOGI("Prepare to notify the soft bus authentication result!");
    int32_t result = instance->setAuthResult(channelId, NULL, 0);
    if (result != HC_SUCCESS) {
        LOGE("Failed to notify the soft bus of the authentication result!");
        return;
    }
    LOGI("The soft bus is successfully notified of the authentication result!");
}

static char *GetSoftBusLocalAuthInfo()
{
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGE("Failed to get soft bus instance!");
        return "";
    }
    return instance->getLocalAuthInfo();
}

SoftBus g_softBus = {
    .openChannel = OpenSoftBusChannel,
    .closeChannel = CloseSoftBusChannel,
    .sendMsg = SendSoftBusMsg,
    .setAuthResult = SetSoftBusAuthResult,
    .getLocalAuthInfo = GetSoftBusLocalAuthInfo
};

int32_t InitSoftBusChannelModule(void (*onChannelOpened)(int64_t, int64_t, const char *, uint32_t, bool),
                                 void (*onChannelClosed)(int64_t, int64_t),
                                 void (*onMsgReceived)(int64_t, const uint8_t *, uint32_t),
                                 void (*onServiceDied)())
{
    IAuthMsgListener softBusListener = {
        .onChannelOpened = onChannelOpened,
        .onChannelClosed = onChannelClosed,
        .onMsgReceived = onMsgReceived,
        .onServiceDied = onServiceDied
    };
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGE("Failed to get soft bus instance!");
        return HC_ERR_SOFT_BUS;
    }
    LOGI("Prepare to add listener callback to soft bus service!");
    int32_t result = instance->addListener(GROUP_MANAGER_PACKAGE_NAME, &softBusListener);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add listener callback to soft bus service!");
        return HC_ERR_SOFT_BUS;
    }
    LOGI("Add listener callback to soft bus service successfully!");
    return HC_SUCCESS;
}

void DestroySoftBusChannelModule()
{
    AuthMsgService *instance = GetInstance(GROUP_MANAGER_PACKAGE_NAME);
    if (instance == NULL) {
        LOGI("Failed to get soft bus instance!");
        return;
    }
    LOGI("Prepare to remove listener callback from soft bus service!");
    int32_t result = instance->removeListener(GROUP_MANAGER_PACKAGE_NAME);
    if (result != HC_SUCCESS) {
        LOGI("Failed to remove listener callback from soft bus module!");
    }
    LOGI("Remove listener callback from soft bus service successfully!");
}

SoftBus *GetSoftBusInstance()
{
    return &g_softBus;
}

bool IsSoftBusChannelSupported()
{
    return true;
}