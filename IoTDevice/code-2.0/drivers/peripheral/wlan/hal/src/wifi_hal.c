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

#include <stdbool.h>
#include "wifi_hal.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "securec.h"
#include "unistd.h"
#include "wifi_hal_cmd.h"
#include "wifi_hal_common.h"
#include "wifi_hal_event.h"
#include "wifi_hal_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_AUTH_NUM 1000

static struct HdfDevEventlistener g_wifiHalEventListener = {0};
static bool g_wifiIsStarted = false;

static int32_t StartInner(struct IWiFi *iwifi)
{
    int32_t ret;

    if (iwifi == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (g_wifiIsStarted) {
        HDF_LOGE("%s: wifi has started already, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    ret = WifiMsgServiceInit();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiMsgServiceInit failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    g_wifiHalEventListener.onReceive = WifiHalEventRecv;
    ret = WifiMsgRegisterEventListener(&g_wifiHalEventListener);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiMsgRegisterEventListener failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
        WifiMsgServiceDeinit();
        return ret;
    }
    ret = HalCmdGetAvailableNetwork();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: HalCmdGetAvailableNetwork failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
        WifiMsgUnregisterEventListener(&g_wifiHalEventListener);
        WifiMsgServiceDeinit();
        return ret;
    }
    g_wifiIsStarted = true;
    return ret;
}

static int32_t StopInner(struct IWiFi *iwifi)
{
    if (iwifi == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!g_wifiIsStarted) {
        HDF_LOGE("%s: wifi has stopped already, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    WifiMsgUnregisterEventListener(&g_wifiHalEventListener);
    WifiMsgServiceDeinit();
    ClearIWiFiList();
    g_wifiIsStarted = false;
    return HDF_SUCCESS;
}

static int32_t GetSupportFeatureInner(uint8_t *supType, uint32_t size)
{
    if (supType == NULL || size <= PROTOCOL_80211_IFTYPE_NUM) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetSupportType(supType);
}

static int32_t GetSupportComboInner(uint64_t *combo, uint32_t size)
{
    if (combo == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HalCmdGetSupportCombo(combo, size);
}

static int32_t InitFeatureByType(int32_t type, struct IWiFiBaseFeature **ifeature)
{
    int32_t ret;

    switch (type) {
        case PROTOCOL_80211_IFTYPE_AP:
            *ifeature = (struct IWiFiBaseFeature *)malloc(sizeof(struct IWiFiAp));
            if (*ifeature == NULL) {
                HDF_LOGE("%s: malloc failed, line: %d", __FUNCTION__, __LINE__);
                return HDF_FAILURE;
            }
            (void)memset_s(*ifeature, sizeof(struct IWiFiAp), 0, sizeof(struct IWiFiAp));
            ret = InitApFeature((struct IWiFiAp **)ifeature);
            break;
        case PROTOCOL_80211_IFTYPE_STATION:
            *ifeature = (struct IWiFiBaseFeature *)malloc(sizeof(struct IWiFiSta));
            if (*ifeature == NULL) {
                HDF_LOGE("%s: malloc failed, line: %d", __FUNCTION__, __LINE__);
                return HDF_FAILURE;
            }
            (void)memset_s(*ifeature, sizeof(struct IWiFiSta), 0, sizeof(struct IWiFiSta));
            ret = InitStaFeature((struct IWiFiSta **)ifeature);
            break;
        default:
            HDF_LOGE("%s: type not support, line: %d", __FUNCTION__, __LINE__);
            return HDF_FAILURE;
    }
    if (ret != HDF_SUCCESS) {
        free(*ifeature);
        *ifeature = NULL;
    }
    return ret;
}

static int32_t FindValidNetwork(int32_t type, struct IWiFiBaseFeature **feature)
{
    struct DListHead *networkHead = GetNetworkHead();
    struct IWiFiList *networkNode = NULL;

    DLIST_FOR_EACH_ENTRY(networkNode, networkHead, struct IWiFiList, entry) {
        if (networkNode->ifeature == NULL && networkNode->supportMode[type] == 1) {
            if (memcpy_s((*feature)->ifName, IFNAME_MAX_LEN, networkNode->ifName, strlen(networkNode->ifName)) != EOK) {
                HDF_LOGE("%s: memcpy_s failed, line: %d", __FUNCTION__, __LINE__);
                return HDF_FAILURE;
            }
            (*feature)->type = type;
            networkNode->ifeature = *feature;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: cannot find available network, line: %d", __FUNCTION__, __LINE__);
    return HDF_FAILURE;
}

static int32_t CreateFeatureInner(int32_t type, struct IWiFiBaseFeature **ifeature)
{
    int32_t ret;

    if (ifeature == NULL) {
        HDF_LOGE("%s: ifeature is null, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    ret = InitFeatureByType(type, ifeature);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: init feature failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    ret = FindValidNetwork(type, ifeature);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: create feature failed, line: %d, error no: %d", __FUNCTION__, __LINE__, ret);
        if (*ifeature != NULL) {
            free(*ifeature);
            *ifeature = NULL;
        }
        return ret;
    }
    return HDF_SUCCESS;
}

static int32_t GetFeatureByIfNameInner(const char *ifName, struct IWiFiBaseFeature **ifeature)
{
    struct DListHead *networkHead = GetNetworkHead();
    struct IWiFiList *networkNode = NULL;

    if (ifName == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    DLIST_FOR_EACH_ENTRY(networkNode, networkHead, struct IWiFiList, entry) {
        if (strcmp(networkNode->ifName, ifName) == HDF_SUCCESS) {
            *ifeature = networkNode->ifeature;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: cannot find feature by ifName, line: %d", __FUNCTION__, __LINE__);
    return HDF_FAILURE;
}

static int32_t DestroyFeatureInner(struct IWiFiBaseFeature *ifeature)
{
    struct DListHead *networkHead = GetNetworkHead();
    struct IWiFiList *networkNode = NULL;

    if (ifeature == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    DLIST_FOR_EACH_ENTRY(networkNode, networkHead, struct IWiFiList, entry) {
        if (strcmp(networkNode->ifName, ifeature->ifName) == HDF_SUCCESS) {
            free(ifeature);
            ifeature = NULL;
            networkNode->ifeature = NULL;
            return HDF_SUCCESS;
        }
    }
    HDF_LOGE("%s: cannot find feature to destroy, line: %d", __FUNCTION__, __LINE__);
    return HDF_FAILURE;
}

static int32_t RegisterEventCallbackInner(CallbackFunc cbFunc)
{
    if (cbFunc == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct CallbackEvent *callbackFunc = GetCallbackFunc();
    if (callbackFunc != NULL) {
        if (callbackFunc->cbFunc != NULL) {
            HDF_LOGE("%s: callback function has been registered, line: %d", __FUNCTION__, __LINE__);
            return HDF_FAILURE;
        }
        callbackFunc->cbFunc = cbFunc;
    } else {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t UnregisterEventCallbackInner(void)
{
    struct CallbackEvent *callbackFunc = GetCallbackFunc();
    if (callbackFunc != NULL) {
        callbackFunc->cbFunc = NULL;
    }
    return HDF_SUCCESS;
}

static int32_t ResetDriverInner(uint8_t chipId)
{
    return HalCmdSetResetDriver(chipId);
}

static int32_t Start(struct IWiFi *iwifi)
{
    HalMutexLock();
    int32_t ret = StartInner(iwifi);
    HalMutexUnlock();
    return ret;
}

static int32_t Stop(struct IWiFi *iwifi)
{
    HalMutexLock();
    int32_t ret = StopInner(iwifi);
    HalMutexUnlock();
    return ret;
}

static int32_t GetSupportFeature(uint8_t *supType, uint32_t size)
{
    HalMutexLock();
    int32_t ret = GetSupportFeatureInner(supType, size);
    HalMutexUnlock();
    return ret;
}

static int32_t GetSupportCombo(uint64_t *combo, uint32_t size)
{
    HalMutexLock();
    int32_t ret = GetSupportComboInner(combo, size);
    HalMutexUnlock();
    return ret;
}

static int32_t CreateFeature(int32_t type, struct IWiFiBaseFeature **ifeature)
{
    HalMutexLock();
    int32_t ret = CreateFeatureInner(type, ifeature);
    HalMutexUnlock();
    return ret;
}

static int32_t GetFeatureByIfName(const char *ifName, struct IWiFiBaseFeature **ifeature)
{
    HalMutexLock();
    int32_t ret = GetFeatureByIfNameInner(ifName, ifeature);
    HalMutexUnlock();
    return ret;
}

static int32_t DestroyFeature(struct IWiFiBaseFeature *ifeature)
{
    HalMutexLock();
    int32_t ret = DestroyFeatureInner(ifeature);
    HalMutexUnlock();
    return ret;
}

static int32_t RegisterEventCallback(CallbackFunc cbFunc)
{
    HalMutexLock();
    int32_t ret = RegisterEventCallbackInner(cbFunc);
    HalMutexUnlock();
    return ret;
}

static int32_t UnregisterEventCallback(void)
{
    HalMutexLock();
    int32_t ret = UnregisterEventCallbackInner();
    HalMutexUnlock();
    return ret;
}

static int32_t ResetDriver(const uint8_t chipId)
{
    if (getuid() >= MAX_AUTH_NUM) {
        HDF_LOGE("%s: don't have authorized access, line: %d", __FUNCTION__, __LINE__);
        return ERR_UNAUTH_ACCESS;
    }

    HalMutexLock();
    int32_t ret = ResetDriverInner(chipId);
    HalMutexUnlock();
    return ret;
}

int32_t WifiConstruct(struct IWiFi **wifiInstance)
{
    static bool isInited = false;
    static struct IWiFi singleWifiInstance;

    if (!isInited) {
        if (HalMutexInit() != HDF_SUCCESS) {
            HDF_LOGE("%s: HalMutexInit failed, line: %d\n", __FUNCTION__, __LINE__);
            return HDF_FAILURE;
        }

        singleWifiInstance.start = Start;
        singleWifiInstance.stop = Stop;
        singleWifiInstance.getSupportFeature = GetSupportFeature;
        singleWifiInstance.getSupportCombo = GetSupportCombo;
        singleWifiInstance.createFeature = CreateFeature;
        singleWifiInstance.getFeatureByIfName = GetFeatureByIfName;
        singleWifiInstance.destroyFeature = DestroyFeature;
        singleWifiInstance.registerEventCallback = RegisterEventCallback;
        singleWifiInstance.unregisterEventCallback = UnregisterEventCallback;
        singleWifiInstance.resetDriver = ResetDriver;
        InitIWiFiList();
        isInited = true;
    }
    (*wifiInstance) = &singleWifiInstance;
    return HDF_SUCCESS;
}

int32_t WifiDestruct(struct IWiFi **wifiInstance)
{
    if (wifiInstance == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    *wifiInstance = NULL;
    if (HalMutexDestroy() != HDF_SUCCESS) {
        HDF_LOGE("%s: HalMutexDestroy failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
