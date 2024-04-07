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

#ifndef HDI_SENSOR_CORE_H
#define HDI_SENSOR_CORE_H

#include <stdint.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "sensor_if.h"
#include "sensor_type.h"

enum SensorMethodIoCmd {
    SENSOR_IO_GET_INFO_LIST = 0xFFFF,
    SENSOR_IO_ENABLE     = 1,
    SENSOR_IO_DISABLE    = 2,
    SENSOR_IO_SET_BATCH  = 3,
    SENSOR_IO_SET_MODE   = 4,
    SENSOR_IO_SET_OPTION = 5,
    SENSOR_IO_END,
};

/* the basic description of a sensor is the same as that of the kernel of definition */
struct SensorBasicInformation {
    char sensorName[SENSOR_NAME_MAX_LEN]; /* Sensor name */
    char vendorName[SENSOR_NAME_MAX_LEN]; /* Sensor vendor */
    char firmwareVersion[SENSOR_VERSION_MAX_LEN]; /* Sensor firmware version */
    char hardwareVersion[SENSOR_VERSION_MAX_LEN]; /* Sensor hardware version */
    int32_t sensorTypeId;   /* Sensor type ID (described in {@link SensorTypeTag}) */
    int32_t sensorId;       /* Sensor ID, defined by the sensor driver developer */
    int32_t maxRange;       /* Maximum measurement range of the sensor */
    int32_t accuracy;       /* Sensor accuracy */
    int32_t power;          /* Sensor power */
};

void GetSensorDeviceMethods(struct SensorInterface *device);
void ReleaseAllSensorInfo(void);

#endif /* HDI_SENSOR_CORE_H */