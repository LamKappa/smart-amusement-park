/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_common.h"
#include "sensor_device_common.h"

#define HDF_LOG_TAG    sensor_common_handler_c

static int32_t SensorOpsNop(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    (void)busCfg;
    (void)cfgItem;
    return HDF_SUCCESS;
}

static int32_t SensorOpsRead(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint16_t value = 0;
    int32_t ret;

    ret = ReadSensor(busCfg, cfgItem->regAddr, (uint8_t *)&value, sizeof(value));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");

    return value;
}

static uint16_t GetSensorRegRealValueMask(struct SensorRegCfg *cfgItem, uint16_t *value, uint16_t busMask)
{
    uint16_t mask = cfgItem->mask & busMask;
    *value = cfgItem->value & busMask;

    if (cfgItem->shiftNum != 0) {
        if (cfgItem->calType == SENSOR_CFG_CALC_TYPE_RIGHT_SHIFT) {
            *value = *value >> cfgItem->shiftNum;
            mask = mask >> cfgItem->shiftNum;
        } else {
            *value = *value << cfgItem->shiftNum;
            mask = mask << cfgItem->shiftNum;
        }
    }

    return mask;
}

static int32_t SensorOpsWrite(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint8_t calType = cfgItem->calType;
    uint8_t value[SENSOR_VALUE_BUTT];
    int32_t ret = HDF_FAILURE;

    value[SENSOR_ADDR_INDEX] = cfgItem->regAddr;
    value[SENSOR_VALUE_INDEX] = cfgItem->value;

    switch (calType) {
        case SENSOR_CFG_CALC_TYPE_NONE:
            ret = WriteSensor(busCfg, value, sizeof(value));
            break;
        case SENSOR_CFG_CALC_TYPE_SET:
        case SENSOR_CFG_CALC_TYPE_REVERT:
        case SENSOR_CFG_CALC_TYPE_XOR:
        case SENSOR_CFG_CALC_TYPE_LEFT_SHIFT:
        case SENSOR_CFG_CALC_TYPE_RIGHT_SHIFT:
        default:
            break;
    }

    return ret;
}

static int32_t SensorOpsReadCheck(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    uint16_t value = 0;
    uint16_t originValue;
    uint16_t mask;
    uint16_t busMask = 0xffff;
    int32_t ret;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        ret = ReadSensor(busCfg, cfgItem->regAddr, (uint8_t *)&value, sizeof(value));
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read i2c reg");
        busMask = (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_1_BYTE) ? 0x00ff : 0xffff;
    }

    mask = GetSensorRegRealValueMask(cfgItem, &originValue, busMask);
    if ((value & mask) != (originValue & mask)) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t SensorOpsUpdateBitwise(struct SensorBusCfg *busCfg, struct SensorRegCfg *cfgItem)
{
    (void)busCfg;
    (void)cfgItem;
    return HDF_SUCCESS;
}

static struct SensorOpsCall g_doOpsCall[] = {
    { SENSOR_OPS_TYPE_NOP,                         SensorOpsNop },
    { SENSOR_OPS_TYPE_READ,                        SensorOpsRead },
    { SENSOR_OPS_TYPE_WRITE,                       SensorOpsWrite },
    { SENSOR_OPS_TYPE_READ_CHECK,                  SensorOpsReadCheck },
    { SENSOR_OPS_TYPE_UPDATE_BITWISE,              SensorOpsUpdateBitwise },
};

int32_t SetSensorRegCfgArray(struct SensorBusCfg *busCfg, const struct SensorRegCfgGroupNode *group)
{
    int32_t num = 0;
    uint32_t count;
    struct SensorRegCfg *cfgItem = NULL;

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(group->regCfgItem, HDF_FAILURE);

    count = sizeof(g_doOpsCall) / sizeof(g_doOpsCall[0]);

    while (num < group->itemNum) {
        cfgItem = (group->regCfgItem + num);
        if (cfgItem->opsType >= count) {
            HDF_LOGE("%s: cfg item para invalid", __func__);
            break;
        }
        if (g_doOpsCall[cfgItem->opsType].ops != NULL) {
            if (g_doOpsCall[cfgItem->opsType].ops(busCfg, cfgItem) != HDF_SUCCESS) {
                HDF_LOGE("%s: malloc sensor reg config item data failed", __func__);
                return HDF_FAILURE;
            }
        }
        if (cfgItem->delay != 0) {
            OsalMDelay(cfgItem->delay);
        }
        num++;
    }

    return HDF_SUCCESS;
}
