/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_DEVICE_MANAGER_H
#define SENSOR_DEVICE_MANAGER_H

#include "osal_mutex.h"
#include "sensor_driver_type.h"

enum SensorMethodCmd {
    SENSOR_CMD_GET_INFO_LIST = 0,
    SENSOR_CMD_ENABLE        = 1,
    SENSOR_CMD_DISABLE       = 2,
    SENSOR_CMD_SET_BATCH     = 3,
    SENSOR_CMD_SET_MODE      = 4,
    SENSOR_CMD_SET_OPTION    = 5,
    SENSOR_CMD_BUTT,
};

struct SensorOps {
    int32_t (*GetInfo)(struct SensorBasicInfo *sensorInfo);
    int32_t (*Enable)(void);
    int32_t (*Disable)(void);
    int32_t (*SetBatch)(int64_t samplingInterval, int64_t reportInterval);
    int32_t (*SetMode)(int32_t mode);
    int32_t (*SetOption)(uint32_t option);
};

struct SensorDeviceInfo {
    struct SensorBasicInfo sensorInfo;
    struct SensorOps ops;
};

struct SensorDevInfoNode {
    struct SensorDeviceInfo devInfo;
    struct DListHead node;
};

typedef int32_t (*SensorCmdHandle)(struct SensorDeviceInfo *info, struct HdfSBuf *reqData, struct HdfSBuf *reply);

struct SensorCmdHandleList {
    enum SensorMethodCmd cmd;
    SensorCmdHandle func;
};

struct SensorDevMgrData {
    struct IDeviceIoService ioService;
    struct HdfDeviceObject *device;
    struct DListHead sensorDevInfoHead;
    struct OsalMutex mutex;
    struct OsalMutex eventMutex;
};

int32_t AddSensorDevice(const struct SensorDeviceInfo *deviceInfo);
int32_t DeleteSensorDevice(int32_t sensorId);
int32_t ReportSensorEvent(const struct SensorReportEvent *events);

#endif /* SENSOR_DEVICE_MANAGER_H */
