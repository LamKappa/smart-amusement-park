/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_COMMON_H
#define SENSOR_COMMON_H

#include "hdf_log.h"
#include "i2c_if.h"
#include "osal_thread.h"
#include "sensor_parser.h"

#define CHECK_NULL_PTR_RETURN_VALUE(ptr, ret) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return ret", __func__, __LINE__); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_PTR_RETURN(ptr) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return", __func__, __LINE__); \
        return; \
    } \
} while (0)

#define CHECK_PARSER_RESULT_RETURN_VALUE(ret, str) do { \
    if (ret != HDF_SUCCESS) { \
        HDF_LOGE("%s:line %d %s fail, ret = %d!", __func__, __LINE__, str, ret); \
        return HDF_FAILURE; \
    } \
} while (0)

#define SENSOR_DATA_SHIFT_LEFT(d, s)    ((d) << (s))
#define SENSOR_DATA_SHIFT_RIGHT(d, s)   ((d) >> (s))

#define SENSOR_ADDR_WIDTH_1_BYTE        1 // 8 bit
#define SENSOR_ADDR_WIDTH_2_BYTE        2 // 16 bit
#define SENSOR_ADDR_WIDTH_4_BYTE        4 // 16 bit
#define SENSOR_DATA_WIDTH_8_BIT         8 // 8 bit
#define SENSOR_CONVERT_UNIT             1000
#define SENSOR_1K_UNIT                  1024
#define SENSOR_SECOND_CONVERT_NANOSECOND    (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT)

#define MAX_SENSOR_EXIT_THREAD_COUNT    10
#define MAX_SENSOR_WAIT_THREAD_TIME     100 // 100MS
typedef int (*sensorEntry)(void *);

enum SensorThreadStatus {
    SENSOR_THREAD_NONE     = 0,
    SENSOR_THREAD_START    = 1,
    SENSOR_THREAD_RUNNING  = 2,
    SENSOR_THREAD_STOPPING = 3,
    SENSOR_THREAD_STOPPED  = 4,
    SENSOR_THREAD_DESTROY  = 5,
    SENSOR_THREAD_STATUS_BUT,
};

enum SENSORConfigValueIndex {
    SENSOR_ADDR_INDEX,
    SENSOR_VALUE_INDEX,
    SENSOR_VALUE_BUTT,
};

int32_t ReadSensor(struct SensorBusCfg *busCfg, uint16_t regAddr, uint8_t *data, uint16_t dataLen);
int32_t WriteSensor(struct SensorBusCfg *busCfg, uint8_t *writeData, uint16_t len);
int32_t SetSensorPinMux(uint32_t regAddr, int32_t regSize, uint32_t regValue);
int32_t CreateSensorThread(struct OsalThread *thread, OsalThreadEntry threadEntry, char *name, void *entryPara);
void DestroySensorThread(struct OsalThread *thread, uint8_t *status);

#endif /* SENSOR_COMMON_H */