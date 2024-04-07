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

#ifndef WIFI_HAL_CMD_H
#define WIFI_HAL_CMD_H

#include "wifi_hal_ap_feature.h"
#include "wifi_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_CHANNEL_NUM 14

enum PlatformServiceID {
    INTERFACE_SERVICE_ID = 0,
    BASE_SERVICE_ID,
    AP_SERVICE_ID,
    STA_SERVICE_ID,
    AUTO_ALLOC_SERVICE_ID_START = 300
};

enum BaseCommands {
    CMD_BASE_NEW_KEY,
    CMD_BASE_DEL_KEY,
    CMD_BASE_SET_DEFAULT_KEY,
    CMD_BASE_SEND_MLME,
    CMD_BASE_SEND_EAPOL,
    CMD_BASE_RECEIVE_EAPOL = 5,
    CMD_BASE_ENALBE_EAPOL,
    CMD_BASE_DISABLE_EAPOL,
    CMD_BASE_GET_ADDR,
    CMD_BASE_SET_MODE,
    CMD_BASE_GET_HW_FEATURE = 10,
    CMD_BASE_SET_NETDEV,
    CMD_BASE_SEND_ACTION,
    CMD_BASE_SET_CLIENT,
    CMD_BASE_GET_NETWORK_INFO = 15,
    CMD_BASE_IS_SUPPORT_COMBO,
    CMD_BASE_GET_SUPPORT_COMBO,
    CMD_BASE_GET_DEV_MAC_ADDR,
    CMD_BASE_SET_MAC_ADDR,
    CMD_BASE_GET_VALID_FREQ = 20,
    CMD_BASE_SET_TX_POWER,
    CMD_BASE_GET_CHIPID,
    CMD_BASE_GET_IFNAMES,
    CMD_BASE_RESET_DRIVER,
};

enum APCommands {
    CMD_AP_START = 0,
    CMD_AP_STOP,
    CMD_AP_CHANGE_BEACON,
    CMD_AP_DEL_STATION,
    CMD_AP_GET_ASSOC_STA,
    CMD_AP_SET_COUNTRY_CODE,
};

enum STACommands {
    CMD_STA_CONNECT = 0,
    CMD_STA_DISCONNECT,
    CMD_STA_SCAN,
    CMD_STA_ABORT_SCAN,
    CMD_STA_SET_SCAN_MAC_ADDR
};

#define MESSAGE_CMD_BITS 16
#define HDF_WIFI_CMD(SERVICEID, CMDID) ((((uint32_t)SERVICEID) << 16) | (CMDID))

typedef enum {
    WIFI_HAL_CMD_GET_NETWORK_INFO = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_GET_NETWORK_INFO),
    WIFI_HAL_CMD_IS_SUPPORT_COMBO = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_IS_SUPPORT_COMBO),
    WIFI_HAL_CMD_GET_SUPPORT_COMBO  = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_GET_SUPPORT_COMBO),
    WIFI_HAL_CMD_GET_DEV_MAC_ADDR   = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_GET_DEV_MAC_ADDR),
    WIFI_HAL_CMD_SET_MAC_ADDR = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_SET_MAC_ADDR),
    WIFI_HAL_CMD_GET_VALID_FREQ = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_GET_VALID_FREQ),
    WIFI_HAL_CMD_SET_TX_POWER = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_SET_TX_POWER),
    WIFI_HAL_CMD_GET_ASSOC_STA = HDF_WIFI_CMD(AP_SERVICE_ID, CMD_AP_GET_ASSOC_STA),
    WIFI_HAL_CMD_SET_COUNTRY_CODE = HDF_WIFI_CMD(AP_SERVICE_ID, CMD_AP_SET_COUNTRY_CODE),
    WIFI_HAL_CMD_SET_SCAN_MAC_ADDR = HDF_WIFI_CMD(STA_SERVICE_ID, CMD_STA_SET_SCAN_MAC_ADDR),
    WIFI_HAL_CMD_GET_CHIPID = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_GET_CHIPID),
    WIFI_HAL_CMD_GET_IFNAMES = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_GET_IFNAMES),
    WIFI_HAL_CMD_RESET_DRIVER = HDF_WIFI_CMD(BASE_SERVICE_ID, CMD_BASE_RESET_DRIVER),
} WifiHalCmd;

int32_t HalCmdGetAvailableNetwork(void);
int32_t HalCmdGetSupportType(uint8_t *supType);
int32_t HalCmdGetSupportCombo(uint64_t *supCombo, uint32_t size);
int32_t HalCmdGetDevMacAddr(const char *ifName, int32_t type, unsigned char *mac, uint8_t len);
int32_t HalCmdSetMacAddr(const char *ifName, unsigned char *mac, uint8_t len);
int32_t HalCmdSetTxPower(const char *ifName, int32_t power);
int32_t HalCmdSetCountryCode(const char *ifName, const char *code, uint32_t len);
int32_t HalCmdGetAsscociatedStas(const char *ifName, struct StaInfo *staInfo, uint32_t count, uint32_t *num);
int32_t HalCmdSetScanningMacAddress(const char *ifName, unsigned char *scanMac, uint8_t len);
int32_t HalCmdGetValidFreqWithBand(const char *ifName, int32_t band, int32_t *freqs, uint32_t *num);
int32_t HalCmdGetChipId(const char *ifName, uint8_t *chipId);
int32_t HalCmdGetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num);
int32_t HalCmdSetResetDriver(const uint8_t chipId);

struct DListHead *GetNetworkHead(void);
void ClearIWiFiList(void);
void InitIWiFiList(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* WIFI_HAL_CMD_H */
