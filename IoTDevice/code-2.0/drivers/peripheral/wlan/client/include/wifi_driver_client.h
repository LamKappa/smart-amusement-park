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

#ifndef _WIFI_MSG_SERVICE_H_
#define _WIFI_MSG_SERVICE_H_

#include "hdf_sbuf.h"
#include "hdf_io_service_if.h"
#include "wifi_common_cmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int32_t WifiMsgServiceInit(void);
void WifiMsgServiceDeinit(void);
int32_t WifiMsgRegisterEventListener(struct HdfDevEventlistener *listener);
void WifiMsgUnregisterEventListener(struct HdfDevEventlistener *listener);
int32_t WifiCmdBlockSyncSend(const uint32_t cmd, struct HdfSBuf *data, struct HdfSBuf *respData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wifi_driver_client.h */
