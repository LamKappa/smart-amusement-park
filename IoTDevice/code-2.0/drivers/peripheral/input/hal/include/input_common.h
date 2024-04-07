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

#ifndef INPUT_COMMON_H
#define INPUT_COMMON_H

#include <pthread.h>
#include <poll.h>
#include "hdf_dlist.h"
#include "hdf_log.h"
#include "input_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SERVICE_NAME_LEN 24
#define MAX_POLLFD_NUM 10
#define SCAN_DEV 0

#define GET_MANAGER_CHECK_RETURN(manager) do { \
    manager = GetDevManager(); \
    if ((manager) == NULL) { \
        HDF_LOGE("%s: get device manager failed", __func__); \
        return INPUT_FAILURE; \
    } \
} while (0)

/**
 * @brief Describes the information nodes of input devices.
 */
typedef struct {
    DeviceInfo payload;       /* Device information payload */
    struct DListHead node;    /* Head node of a linked list */
} DeviceInfoNode;

typedef struct {
    void *service;                       /**< Service of the device */
    void *listener;                      /**< Event listener of the device */
    InputReportEventCb *callback;        /**< Callback {@link InputReportEventCb} for reporting data */
} HostDevInfo;

/**
 * @brief Describes the input device manager.
 */
typedef struct {
    struct DListHead devList;    /* Head node of the linked device list */
    uint32_t currentDevNum;      /* Total number of current devices */
    int32_t callbackNum;         /* The num of registered callback */
    pthread_t thread;            /* Monitoring thread for polling */
    struct pollfd pollFds[MAX_POLLFD_NUM];    /* The records of poll fds */
    pthread_mutex_t mutex;       /* Mutex object to synchronize */
    HostDevInfo hostDev;
} InputDevManager;

/**
 * @brief Defines the information of capacitance test.
 */
typedef struct {
    uint32_t testType;                        /* Capacitance test type */
    char testResult[SELF_TEST_RESULT_LEN];    /* Capacitance test result */
} CapacitanceTestInfo;

enum InputIoctlCmd {
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

#ifdef __cplusplus
}
#endif
#endif
