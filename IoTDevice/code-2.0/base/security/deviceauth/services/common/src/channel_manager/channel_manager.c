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

#include "channel_manager.h"
#include "callback_manager.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "soft_bus_channel.h"

int32_t InitChannelManager(void (*onChannelOpened)(int64_t, int64_t, const char *, uint32_t, bool),
                           void (*onChannelClosed)(int64_t, int64_t),
                           void (*onMsgReceived)(int64_t, const uint8_t *, uint32_t),
                           void (*onServiceDied)())
{
    if (IsSoftBusChannelSupported()) {
        int32_t res = InitSoftBusChannelModule(onChannelOpened, onChannelClosed, onMsgReceived, onServiceDied);
        if (res != HC_SUCCESS) {
            DestroyChannelManager();
        }
        return res;
    }
    return HC_SUCCESS;
}

void DestroyChannelManager()
{
    if (IsSoftBusChannelSupported()) {
        DestroySoftBusChannelModule();
    }
}

ChannelType GetChannelType(const DeviceAuthCallback *callback)
{
    if ((callback != NULL) && (callback->onTransmit != NULL)) {
        return SERVICE_CHANNEL;
    }
    if (IsSoftBusChannelSupported()) {
        return SOFT_BUS;
    }
    return NO_CHANNEL;
}

bool CanFindValidChannel(ChannelType channelType, const CJson *jsonParams, const DeviceAuthCallback *callback)
{
    switch (channelType) {
        case NO_CHANNEL:
            return false;
        case SERVICE_CHANNEL:
            return ((callback != NULL) && (callback->onTransmit != NULL)) ? true : false;
        case SOFT_BUS:
            if (GetSoftBusInstance() == NULL) {
                LOGE("The soft bus module is unavailable!");
                return false;
            }
            const char *connectParams = GetStringFromJson(jsonParams, FIELD_CONNECT_PARAMS);
            if (connectParams == NULL) {
                LOGE("Failed to get connectParams from jsonParams!");
                return false;
            }
            return true;
        default:
            LOGE("Enter the exception case!");
            return false;
    }
}

int32_t OpenChannel(ChannelType channelType, const CJson *jsonParams, int64_t requestId, int64_t *returnChannelId)
{
    int64_t channelId = DEFAULT_CHANNEL_ID;
    const char *connectParams = NULL;
    switch (channelType) {
        case NO_CHANNEL:
            LOGE("No channel!");
            return HC_ERR_CHANNEL_NOT_EXIST;
        case SERVICE_CHANNEL:
            LOGI("Use service channel, so we do not need to open it!");
            *returnChannelId = channelId;
            return HC_SUCCESS;
        case SOFT_BUS:
            connectParams = GetStringFromJson(jsonParams, FIELD_CONNECT_PARAMS);
            if (connectParams == NULL) {
                LOGE("Failed to get connectParams from jsonParams!");
                return HC_ERR_JSON_GET;
            }
            SoftBus *softBusInstance = GetSoftBusInstance();
            if (softBusInstance == NULL) {
                LOGE("The soft bus module is unavailable!");
                return HC_ERR_SOFT_BUS;
            }
            int32_t result = softBusInstance->openChannel(connectParams, requestId, &channelId);
            if (result != HC_SUCCESS) {
                LOGE("Failed to open soft bus channel!");
                return HC_ERR_CHANNEL_NOT_EXIST;
            }
            *returnChannelId = channelId;
            return HC_SUCCESS;
        default:
            LOGE("Enter the exception case!");
            return HC_ERR_CASE;
    }
}

void CloseChannel(ChannelType channelType, int64_t channelId)
{
    SoftBus *softBusInstance = NULL;
    switch (channelType) {
        case NO_CHANNEL:
            LOGE("No channel!");
            return;
        case SERVICE_CHANNEL:
            LOGI("Use service channel, so we do not need to close it!");
            return;
        case SOFT_BUS:
            softBusInstance = GetSoftBusInstance();
            if (softBusInstance == NULL) {
                LOGI("The soft bus module is unavailable!");
                return;
            }
            softBusInstance->closeChannel(channelId);
            return;
        default:
            LOGI("Enter the exception case!");
            return;
    }
}

int32_t SendMsg(ChannelType channelType, int64_t requestId, int64_t channelId,
    const DeviceAuthCallback *callback, const char *data)
{
    SoftBus *softBusInstance = NULL;
    switch (channelType) {
        case NO_CHANNEL:
            LOGE("No channel!");
            return HC_ERR_CHANNEL_NOT_EXIST;
        case SERVICE_CHANNEL:
            if (ProcessTransmitCallback(requestId, (uint8_t *)data, HcStrlen(data) + 1, callback)) {
                return HC_SUCCESS;
            }
            return HC_ERR_TRANSMIT_FAIL;
        case SOFT_BUS:
            softBusInstance = GetSoftBusInstance();
            if (softBusInstance == NULL) {
                LOGE("The soft bus module is unavailable!");
                return HC_ERR_SOFT_BUS;
            }
            return softBusInstance->sendMsg(channelId, (uint8_t *)data, HcStrlen(data) + 1);
        default:
            LOGE("Enter the exception case!");
            return HC_ERR_CASE;
    }
}

void SetAuthResult(ChannelType channelType, int64_t channelId)
{
    SoftBus *softBusInstance = NULL;
    switch (channelType) {
        case NO_CHANNEL:
            LOGI("No channel!");
            return;
        case SERVICE_CHANNEL:
            return;
        case SOFT_BUS:
            softBusInstance = GetSoftBusInstance();
            if (softBusInstance == NULL) {
                LOGI("The soft bus module is unavailable!");
                return;
            }
            softBusInstance->setAuthResult(channelId);
            return;
        default:
            LOGI("Enter the exception case!");
            return;
    }
}

int32_t GetLocalConnectInfo(char **returnInfo)
{
    if (!IsSoftBusChannelSupported()) {
        LOGE("Soft bus not supported!");
        return HC_ERR_NOT_SUPPORT;
    }
    SoftBus *softBusInstance = GetSoftBusInstance();
    if (softBusInstance == NULL) {
        LOGE("The soft bus module is unavailable!");
        return HC_ERR_SOFT_BUS;
    }
    if (returnInfo != NULL) {
        LOGI("We're going to get local connection information!");
        *returnInfo = softBusInstance->getLocalAuthInfo();
        if (*returnInfo == NULL) {
            LOGE("Failed to get local connection information!");
            return HC_ERR_SOFT_BUS;
        }
        LOGI("Get local connection information successfully!");
        return HC_SUCCESS;
    }
    LOGE("The address of the input returnLocalConnectInfo is NULL!");
    return HC_ERR_INVALID_PARAMS;
}
