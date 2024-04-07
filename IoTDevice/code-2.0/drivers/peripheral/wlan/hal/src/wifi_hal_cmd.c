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

#include "wifi_hal_cmd.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_sbuf.h"
#include "securec.h"
#include "wifi_driver_client.h"
#include "wifi_hal_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static struct DListHead g_networkHead = {0};

struct DListHead *GetNetworkHead(void)
{
    return &g_networkHead;
}

static int32_t GetNetworkInfo(struct HdfSBuf *reply)
{
    uint32_t i;
    const char *ifName = NULL;
    uint8_t *mode = NULL;
    uint32_t networkNum;
    uint32_t replayDataSize;

    if (!HdfSbufReadUint32(reply, &networkNum)) {
        HDF_LOGE("%s: get networkNum failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (!DListIsEmpty(&g_networkHead)) {
        ClearIWiFiList();
    }
    for (i = 0; i < networkNum; i++) {
        ifName = HdfSbufReadString(reply);
        if (ifName == NULL) {
            HDF_LOGE("%s: get ifName failed, line: %d", __FUNCTION__, __LINE__);
            return HDF_FAILURE;
        }
        if (!HdfSbufReadBuffer(reply, (const void **)&mode, &replayDataSize) ||
            mode == NULL || replayDataSize != PROTOCOL_80211_IFTYPE_NUM) {
            HDF_LOGE("%s: failed, line: %d", __FUNCTION__, __LINE__);
            return HDF_FAILURE;
        }
        struct IWiFiList *networkList = (struct IWiFiList *)malloc(sizeof(struct IWiFiList));
        if (networkList == NULL) {
            HDF_LOGE("%s: malloc failed, line: %d", __FUNCTION__, __LINE__);
            ClearIWiFiList();
            return HDF_FAILURE;
        }
        (void)memset_s(networkList, sizeof(struct IWiFiList), 0, sizeof(struct IWiFiList));
        DListInsertTail(&networkList->entry, &g_networkHead);
        if (memcpy_s(networkList->ifName, IFNAME_MAX_LEN, ifName, strlen(ifName)) != EOK) {
            HDF_LOGE("%s: memcpy_s failed, line: %d", __FUNCTION__, __LINE__);
            ClearIWiFiList();
            return HDF_FAILURE;
        }
        if (memcpy_s(networkList->supportMode, PROTOCOL_80211_IFTYPE_NUM, mode, replayDataSize) != EOK) {
            HDF_LOGE("%s: memcpy_s failed, line: %d", __FUNCTION__, __LINE__);
            ClearIWiFiList();
            return HDF_FAILURE;
        }
        networkList->ifeature = NULL;
    }
    return HDF_SUCCESS;
}

int32_t HalCmdGetAvailableNetwork(void)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_NETWORK_INFO, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    ret = GetNetworkInfo(reply);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

static void GetSupportTypeByList(uint8_t *supType)
{
    int32_t i;
    struct IWiFiList *networkList = NULL;

    DLIST_FOR_EACH_ENTRY(networkList, &g_networkHead, struct IWiFiList, entry) {
        for (i = 0; i < PROTOCOL_80211_IFTYPE_NUM; i++) {
            if (networkList->supportMode[i] == 1) {
                supType[i] = 1;
            }
        }
    }
}

int32_t HalCmdGetSupportType(uint8_t *supType)
{
    int32_t ret;
    uint8_t isComboValid;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    GetSupportTypeByList(supType);
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_IS_SUPPORT_COMBO, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    if (!HdfSbufReadUint8(reply, &isComboValid)) {
        HDF_LOGE("%s: HdfSbufReadUint8 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    supType[PROTOCOL_80211_IFTYPE_NUM] = isComboValid;
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return HDF_SUCCESS;
}

int32_t HalCmdGetSupportCombo(uint64_t *supCombo, uint32_t size)
{
    int32_t ret;
    uint8_t isComboValid;
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: HdfSBufObtainDefaultSize failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_SUPPORT_COMBO, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    if (!HdfSbufReadUint8(reply, &isComboValid)) {
        HDF_LOGE("%s: HdfSbufReadUint8 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!isComboValid) {
        HDF_LOGE("%s: not support combo mode!, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (memcpy_s(supCombo, size, replayData, replayDataSize) != EOK) {
        HDF_LOGE("%s: memcpy failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return HDF_SUCCESS;
}

int32_t GetDeviceMacAddr(struct HdfSBuf *reply, unsigned char *mac, uint8_t len)
{
    uint8_t isEfuseSavedMac;
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = NULL;

    if (!HdfSbufReadUint8(reply, &isEfuseSavedMac)) {
        HDF_LOGE("%s: HdfSbufReadUint8 failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (!isEfuseSavedMac) {
        HDF_LOGE("%s: not support to get device mac addr!, line: %d", __FUNCTION__, __LINE__);
        return HDF_ERR_NOT_SUPPORT;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize) || replayDataSize != len) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (memcpy_s(mac, len, replayData, replayDataSize) != EOK) {
        HDF_LOGE("%s: memcpy failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t HalCmdGetDevMacAddr(const char *ifName, int32_t type, unsigned char *mac, uint8_t len)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: Fail to obtain sbuf data, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        HDF_LOGE("%s: Fail to obtain sbuf reply, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, type)) {
        HDF_LOGE("%s: HdfSbufWriteInt32 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_DEV_MAC_ADDR, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    ret = GetDeviceMacAddr(reply, mac, len);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t HalCmdSetMacAddr(const char *ifName, unsigned char *mac, uint8_t len)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: Fail to obtain sbuf data, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: Fail to obtain sbuf reply, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(data, mac, len)) {
        HDF_LOGE("%s: HdfSbufWriteBuffer failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_SET_MAC_ADDR, data, reply);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

static int32_t GetValidFreq(struct HdfSBuf *reply, int32_t *freqs, uint32_t *num)
{
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;

    if (!HdfSbufReadUint32(reply, num)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (memcpy_s(freqs, MAX_CHANNEL_NUM * sizeof(int32_t), replayData, replayDataSize) != EOK) {
        HDF_LOGE("%s: memcpy failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t HalCmdGetValidFreqWithBand(const char *ifName, int32_t band, int32_t *freqs, uint32_t *num)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, band)) {
        HDF_LOGE("%s: HdfSbufWriteInt32 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_VALID_FREQ, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    ret = GetValidFreq(reply, freqs, num);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t HalCmdSetTxPower(const char *ifName, int32_t power)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, power)) {
        HDF_LOGE("%s: HdfSbufWriteInt32 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_SET_TX_POWER, data, reply);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

static int32_t GetAsscociatedStas(struct HdfSBuf *reply, struct StaInfo *staInfo, uint32_t count, uint32_t *num)
{
    uint32_t infoSize;
    uint32_t replayDataSize = 0;
    const uint8_t *replayData = 0;

    if (!HdfSbufReadUint32(reply, num)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    if (*num != 0) {
        infoSize = sizeof(struct StaInfo) * (*num);
        if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize) || replayDataSize != infoSize) {
            HDF_LOGE("%s: HdfSbufReadBuffer failed, relaySize=%d,line: %d", __FUNCTION__, replayDataSize, __LINE__);
            return HDF_FAILURE;
        }
        if (memcpy_s(staInfo, sizeof(struct StaInfo) * count, replayData, replayDataSize) != EOK) {
            HDF_LOGE("%s: memcpy failed, line: %d", __FUNCTION__, __LINE__);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t HalCmdGetAsscociatedStas(const char *ifName, struct StaInfo *staInfo, uint32_t count, uint32_t *num)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_ASSOC_STA, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    ret = GetAsscociatedStas(reply, staInfo, count, num);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t HalCmdSetCountryCode(const char *ifName, const char *code, uint32_t len)
{
    int32_t ret;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(data, code, len)) {
        HDF_LOGE("%s: HdfSbufWriteBuffer failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_SET_COUNTRY_CODE, data, reply);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t HalCmdSetScanningMacAddress(const char *ifName, unsigned char *scanMac, uint8_t len)
{
    int32_t ret;
    uint8_t isFuncValid;
    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(data, scanMac, len)) {
        HDF_LOGE("%s: HdfSbufWriteBuffer failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_SET_SCAN_MAC_ADDR, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    if (!HdfSbufReadUint8(reply, &isFuncValid)) {
        HDF_LOGE("%s: HdfSbufReadUint8 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!isFuncValid) {
        HDF_LOGE("%s: not support to set scan mac addr!, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_ERR_NOT_SUPPORT;
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return HDF_SUCCESS;
}

static int32_t GetIfNames(struct HdfSBuf *reply, char **ifNames, uint32_t *num)
{
    uint32_t i;
    uint32_t replayDataSize = 0;
    const char *replayData = NULL;

    if (!HdfSbufReadUint32(reply, num)) {
        HDF_LOGE("%s: HdfSbufReadUint32 failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    *ifNames = (char *)calloc(*num, IFNAME_MAX_LEN);
    if (*ifNames == NULL) {
        HDF_LOGE("%s: calloc failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }

    if (!HdfSbufReadBuffer(reply, (const void **)(&replayData), &replayDataSize) ||
        replayDataSize < (*num * IFNAME_MAX_LEN)) {
        HDF_LOGE("%s: HdfSbufReadBuffer failed, line: %d", __FUNCTION__, __LINE__);
        free(*ifNames);
        *ifNames = NULL;
        return HDF_FAILURE;
    }

    for (i = 0; i < *num; i++) {
        if (memcpy_s(*ifNames + i * IFNAME_MAX_LEN, IFNAME_MAX_LEN, replayData + i * IFNAME_MAX_LEN,
            IFNAME_MAX_LEN) != EOK) {
            HDF_LOGE("%s: memcpy failed, line: %d", __FUNCTION__, __LINE__);
            free(*ifNames);
            *ifNames = NULL;
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t HalCmdGetChipId(const char *ifName, uint8_t *chipId)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, ifName)) {
        HDF_LOGE("%s: HdfSbufWriteString failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_CHIPID, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    if (!HdfSbufReadUint8(reply, chipId)) {
        HDF_LOGE("%s: HdfSbufReadUint8 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t HalCmdGetIfNamesByChipId(const uint8_t chipId, char **ifNames, uint32_t *num)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        HDF_LOGE("%s: Fail to obtain sbuf data, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HDF_LOGE("%s: Fail to obtain sbuf reply, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint8(data, chipId)) {
        HDF_LOGE("%s: HdfSbufWriteUint8 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_GET_IFNAMES, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: WifiCmdBlockSyncSend failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return ret;
    }
    ret = GetIfNames(reply, ifNames, num);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

int32_t HalCmdSetResetDriver(const uint8_t chipId)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSBufObtainDefaultSize();
    if (data == NULL) {
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSBufObtainDefaultSize();
    if (reply == NULL) {
        HdfSBufRecycle(data);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint8(data, chipId)) {
        HDF_LOGE("%s: HdfSbufWriteUint8 failed, line: %d", __FUNCTION__, __LINE__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return HDF_FAILURE;
    }

    ret = WifiCmdBlockSyncSend(WIFI_HAL_CMD_RESET_DRIVER, data, reply);
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

void ClearIWiFiList(void)
{
    struct IWiFiList *networkList = NULL;
    struct IWiFiList *tmp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(networkList, tmp, &g_networkHead, struct IWiFiList, entry) {
        DListRemove(&networkList->entry);
        free(networkList);
        networkList = NULL;
    }
    InitIWiFiList();
}

void InitIWiFiList(void)
{
    DListHeadInit(&g_networkHead);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif