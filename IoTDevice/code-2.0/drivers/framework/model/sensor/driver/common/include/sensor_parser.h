/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_PARSER_H
#define SENSOR_PARSER_H

#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "i2c_if.h"
#include "sensor_driver_type.h"
#include "spi_if.h"

#define SENSOR_CONFIG_MAX_ITEM 100

enum SensorBusType {
    SENSOR_BUS_I2C = 0,
    SENSOR_BUS_SPI = 1,
};

enum SensorRegOpsType {
    SENSOR_INIT_GROUP = 0,
    SENSOR_ENABLE_GROUP,
    SENSOR_DISABLE_GROUP,
    SENSOR_GROUP_MAX,
};

struct SensorI2cCfg {
    DevHandle handle;
    uint16_t busNum;
    uint16_t devAddr;  // Address of the I2C device
    uint16_t regWidth; // length of the register address
};

struct SensorSpiCfg {
    DevHandle handle;
    uint32_t busNum;
    uint32_t csNum;
};

struct SensorBusCfg {
    uint8_t busType; // enum SensorBusType
    uint8_t regBigEndian;
    union {
        struct SensorI2cCfg i2cCfg;
        struct SensorSpiCfg spiCfg;
    };
};

struct SensorAttr {
    const char *chipName;
    uint16_t chipIdReg;
    uint16_t chipIdValue;
};

enum SensorRegCfgIndex {
    SENSOR_REG_CFG_ADDR_INDEX = 0,
    SENSOR_REG_CFG_VALUE_INDEX,
    SENSOR_REG_CFG_MASK_INDEX,
    SENSOR_REG_CFG_LEN_INDEX,
    SENSOR_REG_CFG_DELAY_INDEX,
    SENSOR_REG_CFG_OPS_INDEX,
    SENSOR_REG_CFG_CAL_INDEX,
    SENSOR_REG_CFG_SHIFT_INDEX,
    SENSOR_REG_CFG_DEBUG_INDEX,
    SENSOR_REG_CFG_SAVE_INDEX,
    SENSOR_REG_CFG_INDEX_MAX,
};

struct SensorRegCfg {
    uint16_t regAddr;
    uint16_t value;
    uint16_t mask;
    uint16_t len;
    uint32_t delay : 12;
    uint32_t opsType : 5;
    uint32_t calType : 3;
    uint32_t shiftNum : 4;
    uint32_t debug : 1;
    uint32_t save : 2;
};

struct SensorRegCfgGroupNode {
    uint8_t itemNum;
    struct SensorRegCfg *regCfgItem;
};

struct SensorCfgData {
    struct SensorBusCfg busCfg;
    struct SensorBasicInfo sensorInfo;
    struct SensorAttr sensorAttr;
    struct SensorRegCfgGroupNode **regCfgGroup;
    const struct DeviceResourceNode *root;
};

int32_t GetSensorBaseConfigData(const struct DeviceResourceNode *node, struct SensorCfgData *config);
int32_t ParseSensorRegConfig(struct SensorCfgData *config);
void ReleaseSensorAllRegConfig(struct SensorCfgData *config);
int32_t GetSensorBusHandle(struct SensorBusCfg *busCfg);
int32_t ReleaseSensorBusHandle(struct SensorBusCfg *busCfg);
int32_t DetectSensorDevice(struct SensorCfgData *config);

#endif /* SENSOR_PARSER_H */
