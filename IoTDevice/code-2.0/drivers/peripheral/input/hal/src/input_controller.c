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
#include "input_controller.h"
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <securec.h>
#include "hdf_io_service_if.h"
#include "input_common.h"

InputDevManager *GetDevManager(void);

static bool FillSbufData(struct HdfSBuf *data, int32_t cmd, const void *in)
{
    bool ret = false;
    switch (cmd) {
        case SET_PWR_STATUS:
        case SET_GESTURE_MODE:
        case RUN_CAPAC_TEST:
            ret = HdfSbufWriteUint32(data, *(uint32_t *)(in));
            break;
        case RUN_EXTRA_CMD:
            ret = HdfSbufWriteString(data, ((InputExtraCmd *)in)->cmdCode);
            if (ret) {
                ret = HdfSbufWriteString(data, ((InputExtraCmd *)in)->cmdValue);
            }
            break;
        default:
            break;
    }
    if (!ret) {
        HDF_LOGE("%s: cmd = %d sbuf write failed", __func__, cmd);
    }
    return ret;
}

static bool ObtainSbufData(struct HdfSBuf *reply, int32_t cmd, void *out, uint32_t length)
{
    bool ret = false;
    uint32_t tempInt;
    const char *tempStr = NULL;
    switch (cmd) {
        case GET_PWR_STATUS:
        case GET_DEV_TYPE:
            ret = HdfSbufReadUint32(reply, &tempInt);
            if (ret) {
                *((uint32_t *)out) = tempInt;
            }
            break;
        case GET_CHIP_INFO:
        case GET_VENDOR_NAME:
        case GET_CHIP_NAME:
        case RUN_CAPAC_TEST:
            tempStr = HdfSbufReadString(reply);
            if (tempStr != NULL) {
                if (strncpy_s(out, length, tempStr, strlen(tempStr)) == EOK) {
                    ret = true;
                } else {
                    HDF_LOGE("%s: strncpy_s fail", __func__);
                }
            }
            break;
        default:
            break;
    }
    if (!ret) {
        HDF_LOGE("%s: sbuf write or strncpy_s failed", __func__);
    }
    return ret;
}

static int32_t InstanceCmdSbuf(struct HdfSBuf **data, struct HdfSBuf **reply)
{
    *data = HdfSBufObtainDefaultSize();
    if (*data == NULL) {
        HDF_LOGE("%s: fail to obtain sbuf data", __func__);
        return INPUT_NULL_PTR;
    }

    *reply = HdfSBufObtainDefaultSize();
    if (*reply == NULL) {
        HDF_LOGE("%s: fail to obtain sbuf reply", __func__);
        goto EXIT;
    }
    return INPUT_SUCCESS;

EXIT:
    HdfSBufRecycle(*data);
    HdfSBufRecycle(*reply);
    *data = NULL;
    *reply = NULL;
    return INPUT_FAILURE;
}

static int32_t IoServiceOps(struct HdfIoService *service, int32_t cmd, const void *in, void *out, uint32_t outLen)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (InstanceCmdSbuf(&data, &reply) != INPUT_SUCCESS) {
        return INPUT_FAILURE;
    }

    if (in != NULL) {
        if (!FillSbufData(data, cmd, in)) {
            ret = INPUT_FAILURE;
            goto EXIT;
        }
    }

    ret = service->dispatcher->Dispatch(&service->object, cmd, data, reply);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGE("%s: dispatch fail", __func__);
        goto EXIT;
    }

    if (out != NULL) {
        if (!ObtainSbufData(reply, cmd, out, outLen)) {
            ret = INPUT_FAILURE;
        }
    }

EXIT:
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
    return ret;
}

static int32_t SetPowerStatus(uint32_t devIndex, uint32_t status)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    struct HdfIoService *service = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (status >= INPUT_POWER_STATUS_UNKNOWN)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, SET_PWR_STATUS, &status, NULL, 0)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: set power status failed", __func__);
            return INPUT_FAILURE;
        }
        pos->payload.powerStatus = status;
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't set power status", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetPowerStatus(uint32_t devIndex, uint32_t *status)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    struct HdfIoService *service = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (status == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }
    GET_MANAGER_CHECK_RETURN(manager);

    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, GET_PWR_STATUS, NULL, status, sizeof(uint32_t))) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get power status failed", __func__);
            return INPUT_FAILURE;
        }
        if (*status >= INPUT_POWER_STATUS_UNKNOWN) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: power status is unknown", __func__);
            return INPUT_FAILURE;
        }
        pos->payload.powerStatus = *status;
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get power status", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetDeviceType(uint32_t devIndex, uint32_t *deviceType)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    struct HdfIoService *service = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (deviceType == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, GET_DEV_TYPE, NULL, deviceType, sizeof(uint32_t))) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get device type failed", __func__);
            return INPUT_FAILURE;
        }
        if (*deviceType >= INDEV_TYPE_UNKNOWN) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: device type is unknown", __func__);
            return INPUT_FAILURE;
        }
        pos->payload.devType = *deviceType;
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get device type", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetGeneralInfo(uint32_t devIndex, char *generalInfo, uint32_t length, uint32_t lengthLimit, int32_t cmd)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    void *info = NULL;
    struct HdfIoService *service = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (generalInfo == NULL) || (length < lengthLimit)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        switch (cmd) {
            case GET_CHIP_NAME:
                info = pos->payload.chipName;
                break;
            case GET_CHIP_INFO:
                info = pos->payload.chipInfo;
                break;
            case GET_VENDOR_NAME:
                info = pos->payload.vendorName;
                break;
            default:
                pthread_mutex_unlock(&manager->mutex);
                HDF_LOGE("%s: cmd = %d invalid param", __func__, cmd);
                return INPUT_FAILURE;
        }

        service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, cmd, NULL, info, lengthLimit)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: get information fail", __func__);
            return INPUT_FAILURE;
        }

        if (strncpy_s(generalInfo, length, info, lengthLimit - 1) != EOK) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: strncpy_s fail", __func__);
            return INPUT_FAILURE;
        }

        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGI("%s: device%u get information success", __func__, devIndex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't get information", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t GetChipName(uint32_t devIndex, char *chipName, uint32_t length)
{
    return GetGeneralInfo(devIndex, chipName, length, CHIP_NAME_LEN, GET_CHIP_NAME);
}

static int32_t GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length)
{
    return GetGeneralInfo(devIndex, chipInfo, length, CHIP_INFO_LEN, GET_CHIP_INFO);
}

static int32_t GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length)
{
    return GetGeneralInfo(devIndex, vendorName, length, VENDOR_NAME_LEN, GET_VENDOR_NAME);
}

static int32_t SetGestureMode(uint32_t devIndex, uint32_t gestureMode)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    struct HdfIoService *service = NULL;

    if (devIndex >= MAX_INPUT_DEV_NUM) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, SET_GESTURE_MODE, &gestureMode, NULL, 0)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: set gesture mode failed", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't set gesture mode", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;
    CapacitanceTestInfo testInfo;

    if (devIndex >= (MAX_INPUT_DEV_NUM) || (testType >= TEST_TYPE_UNKNOWN) ||
        (result == NULL) || (length < SELF_TEST_RESULT_LEN)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    (void)memset_s(&testInfo, sizeof(CapacitanceTestInfo), 0, sizeof(CapacitanceTestInfo));
    testInfo.testType = testType;
    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        struct HdfIoService *service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, RUN_CAPAC_TEST, &testInfo.testType, testInfo.testResult, SELF_TEST_RESULT_LEN)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: run capacitance test failed", __func__);
            return INPUT_FAILURE;
        }

        if (strncpy_s(result, length, testInfo.testResult, SELF_TEST_RESULT_LEN - 1) != EOK) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: strncpy_s fail", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        HDF_LOGI("%s: capacitance test result is %s", __func__, result);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%u doesn't exist, can't run capacitance test", __func__, devIndex);
    return INPUT_FAILURE;
}

static int32_t RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmdInfo)
{
    DeviceInfoNode *pos = NULL;
    DeviceInfoNode *next = NULL;
    InputDevManager *manager = NULL;

    if ((devIndex >= MAX_INPUT_DEV_NUM) || (cmdInfo == NULL) || (cmdInfo->cmdCode == NULL) ||
        (cmdInfo->cmdValue == NULL)) {
        HDF_LOGE("%s: invalid param", __func__);
        return INPUT_INVALID_PARAM;
    }

    GET_MANAGER_CHECK_RETURN(manager);
    pthread_mutex_lock(&manager->mutex);
    DLIST_FOR_EACH_ENTRY_SAFE(pos, next, &manager->devList, DeviceInfoNode, node) {
        if (pos->payload.devIndex != devIndex) {
            continue;
        }
        struct HdfIoService *service = (struct HdfIoService *)pos->payload.service;
        if (IoServiceOps(service, RUN_EXTRA_CMD, cmdInfo, NULL, 0)) {
            pthread_mutex_unlock(&manager->mutex);
            HDF_LOGE("%s: run extra cmd failed", __func__);
            return INPUT_FAILURE;
        }
        pthread_mutex_unlock(&manager->mutex);
        return INPUT_SUCCESS;
    }

    pthread_mutex_unlock(&manager->mutex);
    HDF_LOGE("%s: device%d doesn't exist, can't run extra cmd", __func__, devIndex);
    return INPUT_FAILURE;
}

int32_t InstanceControllerHdi(InputController **controller)
{
    InputController *controllerHdi = (InputController *)malloc(sizeof(InputController));
    if (controllerHdi == NULL) {
        HDF_LOGE("%s: malloc fail", __func__);
        return INPUT_NOMEM;
    }

    (void)memset_s(controllerHdi, sizeof(InputController), 0, sizeof(InputController));

    controllerHdi->GetDeviceType = GetDeviceType;
    controllerHdi->SetPowerStatus = SetPowerStatus;
    controllerHdi->GetPowerStatus = GetPowerStatus;
    controllerHdi->GetChipInfo = GetChipInfo;
    controllerHdi->GetVendorName = GetVendorName;
    controllerHdi->GetChipName = GetChipName;
    controllerHdi->SetGestureMode = SetGestureMode;
    controllerHdi->RunCapacitanceTest = RunCapacitanceTest;
    controllerHdi->RunExtraCommand = RunExtraCommand;
    *controller = controllerHdi;
    return INPUT_SUCCESS;
}
