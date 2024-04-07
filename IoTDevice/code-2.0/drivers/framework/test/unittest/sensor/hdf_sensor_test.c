/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <securec.h>
#include "hdf_base.h"
#include "hdf_device_desc.h"
#include "hdf_sensor_test.h"
#include "osal_math.h"
#include "osal_time.h"
#include "osal_timer.h"
#include "sensor_common.h"
#include "sensor_device_manager.h"
#include "sensor_driver_type.h"

#define HDF_LOG_TAG    hdf_sensor_test_c

#define HDF_SENSOR_TEST_VALUE    1024000000 // 1g = 9.8m/s^2
#define SENSOR_TEST_MAX_RANGE    8
#define SENSOR_TEST_MAX_POWER    230

static struct SensorTestDrvData *GetSensorTestDrvData(void)
{
    static struct SensorTestDrvData sensorTestDrvData = {
        .threadStatus = SENSOR_THREAD_NONE,
        .initStatus = false,
        .interval = SENSOR_TEST_SAMPLING_200_MS,
    };

    return &sensorTestDrvData;
}
static void SensorReadTestData(void)
{
    int32_t value = HDF_SENSOR_TEST_VALUE;
    struct SensorReportEvent event;
    OsalTimespec time;

    (void)memset_s(&event, sizeof(event), 0, sizeof(event));
    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)OsalGetTime(&time);

    event.timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT; // nanosecond
    event.sensorId = SENSOR_TAG_NONE;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;
    event.dataLen = sizeof(value);
    event.data = (uint8_t *)&value;
    ReportSensorEvent(&event);
}

static int32_t SensorReadDataThreadTestWorker(void *arg)
{
    (void)arg;
    int64_t interval;
    CHECK_NULL_PTR_RETURN_VALUE(arg, HDF_ERR_INVALID_PARAM);
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    drvData->threadStatus = SENSOR_THREAD_START;
    while (true) {
        if (drvData->threadStatus == SENSOR_THREAD_RUNNING) {
            SensorReadTestData();
            interval = OsalDivS64(drvData->interval, (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT));
            OsalMSleep(interval);
        } else if (drvData->threadStatus == SENSOR_THREAD_STOPPING) {
            drvData->threadStatus = SENSOR_THREAD_STOPPED;
            break;
        } else {
            OsalMSleep(SENSOR_TEST_SAMPLING_200_MS / SENSOR_CONVERT_UNIT / SENSOR_CONVERT_UNIT);
        }

        if ((!drvData->initStatus) || (drvData->interval < 0) || drvData->threadStatus != SENSOR_THREAD_RUNNING) {
            continue;
        }
    }

    HDF_LOGE("%s: Sensor test thread have exited", __func__);
    return HDF_SUCCESS;
}

static int32_t SensorInitTestConfig(void)
{
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    if (drvData->threadStatus != SENSOR_THREAD_NONE && drvData->threadStatus != SENSOR_THREAD_DESTROY) {
        HDF_LOGE("%s: Sensor test thread have created", __func__);
        return HDF_SUCCESS;
    }

    int32_t ret = CreateSensorThread(&drvData->thread, SensorReadDataThreadTestWorker, "hdf_sensor_test", drvData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Sensor test create thread failed", __func__);
        drvData->threadStatus = SENSOR_THREAD_NONE;
        return HDF_FAILURE;
    }
    drvData->initStatus = true;

    return HDF_SUCCESS;
}

static int32_t SensorGetInfoTest(struct SensorBasicInfo *info)
{
    (void)info;

    return HDF_SUCCESS;
}

static int32_t SensorEnableTest(void)
{
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    drvData->threadStatus = SENSOR_THREAD_RUNNING;
    return HDF_SUCCESS;
}

static int32_t SensorDisableTest(void)
{
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    drvData->threadStatus = SENSOR_THREAD_STOPPED;

    return HDF_SUCCESS;
}

static int32_t SensorSetBatchTest(int64_t samplingInterval, int64_t interval)
{
    (void)interval;
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    drvData->interval = samplingInterval;
    return HDF_SUCCESS;
}

static int32_t SensorSetModeTest(int32_t mode)
{
    return (mode == SENSOR_WORK_MODE_REALTIME) ? HDF_SUCCESS : HDF_FAILURE;
}

static int32_t SensorSetOptionTest(uint32_t option)
{
    (void)option;
    return HDF_SUCCESS;
}

static int32_t SensorTestDispatch(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t BindSensorDriverTest(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    static struct IDeviceIoService service = {
        .object = {0},
        .Dispatch = SensorTestDispatch,
    };
    device->service = &service;
    return HDF_SUCCESS;
}

int32_t InitSensorDriverTest(struct HdfDeviceObject *device)
{
    int32_t ret;
    (void)device;
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    struct SensorDeviceInfo deviceInfo = {
        .sensorInfo = {
            .sensorName = "sensor_test",
            .vendorName = "default",
            .firmwareVersion = "1.0",
            .hardwareVersion = "1.0",
            .sensorTypeId = SENSOR_TAG_NONE,
            .sensorId = SENSOR_TAG_NONE,
            .maxRange = SENSOR_TEST_MAX_RANGE,
            .accuracy = 1,
            .power = SENSOR_TEST_MAX_POWER,
        },
        .ops = {
            .GetInfo = SensorGetInfoTest,
            .Enable = SensorEnableTest,
            .Disable = SensorDisableTest,
            .SetBatch = SensorSetBatchTest,
            .SetMode = SensorSetModeTest,
            .SetOption = SensorSetOptionTest,
        },
    };

    ret = SensorInitTestConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test config failed", __func__);
        return HDF_FAILURE;
    }

    ret = AddSensorDevice(&deviceInfo);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor test register failed", __func__);
        (void)DestroySensorThread(&drvData->thread, &drvData->threadStatus);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: init sensor test driver success", __func__);
    return HDF_SUCCESS;
}

void ReleaseSensorDriverTest(struct HdfDeviceObject *device)
{
    (void)device;
    struct SensorTestDrvData *drvData = GetSensorTestDrvData();

    (void)DestroySensorThread(&drvData->thread, &drvData->threadStatus);
    (void)DeleteSensorDevice(SENSOR_TAG_NONE);
}

struct HdfDriverEntry g_sensorTestDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_TEST_DRIVER",
    .Bind = BindSensorDriverTest,
    .Init = InitSensorDriverTest,
    .Release = ReleaseSensorDriverTest,
};

HDF_INIT(g_sensorTestDevEntry);
