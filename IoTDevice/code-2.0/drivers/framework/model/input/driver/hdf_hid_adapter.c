/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_device_desc.h"
#include "osal_mem.h"
#include "hdf_log.h"
#include "event_hub.h"
#include "hdf_input_device_manager.h"
#include "hdf_hid_adapter.h"

InputDevice *cachedHid[MAX_INPUT_DEV_NUM];

static bool HaveHidCache(void)
{
    if (cachedHid[0] == NULL) {
        return false;
    }
    return true;
}

static void LoadCachedHid(void)
{
    int32_t i = 0;
    int32_t ret;
    if (!HaveHidCache()) {
        HDF_LOGI("%s: exit", __func__);
        return;
    }
    while (i < MAX_INPUT_DEV_NUM && cachedHid[i] != NULL) {
        ret = RegisterInputDevice(cachedHid[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: add %s failed", __func__, cachedHid[i]->devName);
        }
        cachedHid[i] = NULL;
        i++;
    }
}

static InputDevice* HidConstructInputDev(HidInfo info)
{
    InputDevice *inputDev = (InputDevice *)OsalMemAlloc(sizeof(InputDevice));
    if (inputDev == NULL) {
        HDF_LOGE("%s: instance input device failed", __func__);
        return NULL;
    }
    (void)memset_s(inputDev, sizeof(InputDevice), 0, sizeof(InputDevice));

    inputDev->devType = info.devType;
    inputDev->devName = info.devName;
    return inputDev;
}

static InputDevice* DoRegisterInputDev(InputDevice* inputDev)
{
    int32_t ret;

    ret = RegisterInputDevice(inputDev);
    if (ret == HDF_SUCCESS) {
        return inputDev;
    } else {
        OsalMemFree(inputDev);
        inputDev = NULL;
        return NULL;
    }
}

static InputDevice* CacheHid(InputDevice* inputDev)
{
    int32_t i = 0;
    while ((i < MAX_INPUT_DEV_NUM) && (cachedHid[i] != NULL)) {
        i++;
    }
    if (i < MAX_INPUT_DEV_NUM) {
        cachedHid[i] = inputDev;
        return inputDev;
    }
    return NULL;
}

static bool InputDriverLoaded(void)
{
    InputManager* g_inputManager = GetInputManager();
    if ((g_inputManager != NULL) && (g_inputManager->initialized != false)) {
        return true;
    }
    return false;
}

void* HidRegisterHdfInputDev(HidInfo info)
{
    InputDevice* inputDev = HidConstructInputDev(info);
    if (inputDev == NULL) {
        HDF_LOGE("%s: hid construct input Dev failed", __func__);
        return NULL;
    }

    if (InputDriverLoaded()) {
        return DoRegisterInputDev(inputDev);
    } else {
        return CacheHid(inputDev);
    }
}

void HidUnregisterHdfInputDev(const void *inputDev)
{
    if (inputDev == NULL) {
        HDF_LOGE("%s: inputDev is null", __func__);
    }
    UnregisterInputDevice((InputDevice *)inputDev);
    inputDev = NULL;
}

void HidReportEvent(const void *inputDev, uint32_t type, uint32_t code, int32_t value)
{
    PushOnePackage((InputDevice *)inputDev, type, code, value);
}

static int32_t HdfHIDDriverInit(struct HdfDeviceObject *device)
{
    (void)device;
    static bool cachedHidRegistered = false;
    if (!cachedHidRegistered) {
        cachedHidRegistered = true;
        LoadCachedHid();
    }
    return HDF_SUCCESS;
}

static int32_t HdfHIDDispatch(struct HdfDeviceIoClient *client, int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)cmd;
    if (client == NULL || data == NULL || reply == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t HdfHIDDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    static struct IDeviceIoService hidService = {
        .Dispatch = HdfHIDDispatch,
    };
    device->service = &hidService;
    return HDF_SUCCESS;
}

struct HdfDriverEntry g_hdfHIDEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_HID",
    .Bind = HdfHIDDriverBind,
    .Init = HdfHIDDriverInit,
};

HDF_INIT(g_hdfHIDEntry);