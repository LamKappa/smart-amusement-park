/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_INPUT_DEVICE_MANAGER_H
#define HDF_INPUT_DEVICE_MANAGER_H

#include "input-event-codes.h"
#include "osal_mutex.h"
#include "hdf_types.h"
#include "hdf_device_desc.h"

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif
#define HDF_LOG_TAG HDF_INPUT_DRV
#define INPUT_DEV_PATH_LEN 64
#define MAX_INPUT_DEV_NUM  32
#define DEV_NAME_LEN 16
#define ONLINE    0
#define OFFLINE   1

#define CHECK_RETURN_VALUE(ret) do { \
    if ((ret) != HDF_SUCCESS) { \
        return ret; \
    } \
} while (0)

typedef struct {
    uint32_t devId;
    uint32_t devType;
} DevDesc;

typedef struct {
    uint32_t devId;
    uint32_t devType;
    uint32_t status;
} HotPlugEvent;

typedef struct {
    struct IDeviceIoService ioService;
    uint32_t (*getDeviceCount)(void);
} IInputManagerService;

typedef struct InputDeviceInfo {
    struct HdfDeviceObject *hdfDevObj;
    uint32_t devId;
    uint32_t devType;
    const char *devNode;
    const char *devName;
    uint16_t pkgNum;
    uint16_t pkgCount;
    bool errFrameFlag;
    struct HdfSBuf *pkgBuf;
    void *pvtData;
    struct InputDeviceInfo *next;
} InputDevice;

typedef struct {
    struct HdfDeviceObject *hdfDevObj;
    uint32_t devCount;
    struct OsalMutex mutex;
    bool initialized;
    InputDevice *inputDevList;
} InputManager;

enum InputDevType {
    INDEV_TYPE_TOUCH,               /* Touchscreen */
    INDEV_TYPE_KEY,                 /* Physical key */
    INDEV_TYPE_BUTTON,              /* Virtual button */
    INDEV_TYPE_CROWN,               /* Watch crown */
    INDEV_TYPE_ENCODER,             /* Customized type of a specific function or event */
    INDEV_TYPE_HID_BEGIN_POS = 33,  /* HID type start position */
    INDEV_TYPE_MOUSE,               /* Mouse */
    INDEV_TYPE_KEYBOARD,            /* Keyboard */
    INDEV_TYPE_UNKNOWN,             /* Unknown input device type */
};

enum InputIOsvcCmdId {
    GET_DEV_TYPE,
    SET_PWR_STATUS,
    GET_PWR_STATUS,
    GET_CHIP_INFO,
    GET_VENDOR_NAME,
    GET_CHIP_NAME,
    SET_GESTURE_MODE,
    RUN_CAPAC_TEST,
    RUN_EXTRA_CMD,
};

enum TouchIoctlCmd {
    INPUT_IOCTL_GET_EVENT_DATA,
    INPUT_IOCTL_SET_POWER_STATUS,
    INPUT_IOCTL_GET_POWER_STATUS,
    INPUT_IOCTL_GET_DEVICE_TYPE,
    INPUT_IOCTL_GET_CHIP_INFO,
    INPUT_IOCTL_GET_VENDOR_NAME,
    INPUT_IOCTL_GET_CHIP_NAME,
    INPUT_IOCTL_SET_GESTURE_MODE,
    INPUT_IOCTL_RUN_CAPACITANCE_TEST,
    INPUT_IOCTL_RUN_EXTRA_CMD,
};
InputManager* GetInputManager(void);
int32_t RegisterInputDevice(InputDevice *device);
void UnregisterInputDevice(InputDevice *device);

#endif