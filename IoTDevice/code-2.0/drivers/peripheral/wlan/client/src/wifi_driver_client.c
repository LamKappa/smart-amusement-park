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

#include "wifi_driver_client.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "utils/hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static struct HdfIoService *g_wifiService = NULL;

static const char *DRIVER_SERVICE_NAME = "hdfwifi";

int32_t WifiMsgServiceInit(void)
{
    if (g_wifiService == NULL) {
        g_wifiService = HdfIoServiceBind(DRIVER_SERVICE_NAME);
    }
    if (g_wifiService == NULL) {
        HDF_LOGE("%s: fail to get remote service!, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WifiMsgRegisterEventListener(struct HdfDevEventlistener *listener)
{
    if (g_wifiService == NULL || listener == NULL) {
        HDF_LOGE("%s: params or g_wifiService is NULL, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (HdfDeviceRegisterEventListener(g_wifiService, listener) != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to register event listener, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void WifiMsgUnregisterEventListener(struct HdfDevEventlistener *listener)
{
    if (g_wifiService == NULL || listener == NULL) {
        HDF_LOGE("%s: params or g_wifiService is NULL, line: %d", __FUNCTION__, __LINE__);
        return;
    }
    if (HdfDeviceUnregisterEventListener(g_wifiService, listener)) {
        HDF_LOGE("%s: fail to unregister listener, line: %d", __FUNCTION__, __LINE__);
    }
}

void WifiMsgServiceDeinit(void)
{
    if (g_wifiService == NULL) {
        HDF_LOGE("%s: g_wifiService is NULL, line: %d", __FUNCTION__, __LINE__);
        return;
    }
    if (HdfIoserviceGetListenerCount(g_wifiService) != 0) {
        HDF_LOGE("%s: EventListener is not empty. cancel listener registration, line: %d", __FUNCTION__, __LINE__);
        return;
    }
    HdfIoServiceRecycle(g_wifiService);
    g_wifiService = NULL;
}

int32_t WifiCmdBlockSyncSend(const uint32_t cmd, struct HdfSBuf *reqData, struct HdfSBuf *respData)
{
    if (reqData == NULL) {
        HDF_LOGE("%s: params is NULL, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (g_wifiService == NULL || g_wifiService->dispatcher == NULL || g_wifiService->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%s: bad remote service found, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    int32_t ret = g_wifiService->dispatcher->Dispatch(&g_wifiService->object, cmd, reqData, respData);
    HDF_LOGI("%s: cmd=%u, ret=%d, line: %d", __FUNCTION__, cmd, ret, __LINE__);
    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
