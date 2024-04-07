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

#include "wifi_hal_event.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "securec.h"
#include "wifi_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static struct CallbackEvent g_callbackEvent = {0};

struct CallbackEvent *GetCallbackFunc(void)
{
    return &g_callbackEvent;
}

int32_t WifiHalEventRecv(struct HdfDevEventlistener *listener,
    struct HdfIoService *service, uint32_t id, struct HdfSBuf *data)
{
    (void)listener;
    (void)service;
    if (data == NULL) {
        HDF_LOGE("%s: input parameter invalid, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    int32_t ret;
    struct HdfSBuf *copyData = HdfSBufCopy(data);
    if (copyData == NULL) {
        HDF_LOGE("%s: fail to copy data, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    struct CallbackEvent *callbackFunc = GetCallbackFunc();
    if (callbackFunc != NULL && callbackFunc->cbFunc != NULL) {
        ret = callbackFunc->cbFunc(id, copyData);
    } else {
        HDF_LOGE("%s: callbackFunc have not initialized, line: %d", __FUNCTION__, __LINE__);
        ret = HDF_FAILURE;
    }
    HdfSBufRecycle(copyData);
    return ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif