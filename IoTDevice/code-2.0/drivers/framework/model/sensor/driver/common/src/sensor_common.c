/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "osal_io.h"
#include "osal_thread.h"
#include "osal_time.h"
#include "sensor_common.h"

#define HDF_LOG_TAG    sensor_common_operation_c

#define I2C_READ_MSG_NUM           2
#define I2C_READ_MSG_ADDR_IDX      0
#define I2C_READ_MSG_VALUE_IDX     1

#define I2C_WRITE_MSG_NUM  1
#define I2C_REG_BUF_LEN    4
#define I2C_BYTE_MASK      0xFF
#define I2C_BYTE_OFFSET    8

#define SENSOR_STACK_SIZE  0x2000    // 4k buffer

int32_t ReadSensor(struct SensorBusCfg *busCfg, uint16_t regAddr, uint8_t *data, uint16_t dataLen)
{
    int index = 0;
    unsigned char regBuf[I2C_REG_BUF_LEN] = {0};
    struct I2cMsg msg[I2C_READ_MSG_NUM];

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        CHECK_NULL_PTR_RETURN_VALUE(busCfg->i2cCfg.handle, HDF_FAILURE);

        msg[I2C_READ_MSG_ADDR_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[I2C_READ_MSG_ADDR_IDX].flags = 0;
        msg[I2C_READ_MSG_ADDR_IDX].len = busCfg->i2cCfg.regWidth;
        msg[I2C_READ_MSG_ADDR_IDX].buf = regBuf;

        if (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_1_BYTE) {
            regBuf[index++] = regAddr & I2C_BYTE_MASK;
        } else if (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_2_BYTE) {
            regBuf[index++] = (regAddr >> I2C_BYTE_OFFSET) & I2C_BYTE_MASK;
            regBuf[index++] = regAddr & I2C_BYTE_MASK;
        } else {
            HDF_LOGE("%s: i2c regWidth[%u] failed", __func__, busCfg->i2cCfg.regWidth);
            return HDF_FAILURE;
        }

        msg[I2C_READ_MSG_VALUE_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[I2C_READ_MSG_VALUE_IDX].flags = I2C_FLAG_READ;
        msg[I2C_READ_MSG_VALUE_IDX].len = dataLen;
        msg[I2C_READ_MSG_VALUE_IDX].buf = data;

        if (I2cTransfer(busCfg->i2cCfg.handle, msg, I2C_READ_MSG_NUM) != I2C_READ_MSG_NUM) {
            HDF_LOGE("%s: i2c[%u] read failed", __func__, busCfg->i2cCfg.busNum);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

int32_t WriteSensor(struct SensorBusCfg *busCfg, uint8_t *writeData, uint16_t dataLen)
{
    struct I2cMsg msg[I2C_WRITE_MSG_NUM];

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(writeData, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        CHECK_NULL_PTR_RETURN_VALUE(busCfg->i2cCfg.handle, HDF_FAILURE);

        msg[0].addr = busCfg->i2cCfg.devAddr;
        msg[0].flags = 0;
        msg[0].len = dataLen;
        msg[0].buf = writeData;

        if (I2cTransfer(busCfg->i2cCfg.handle, msg, I2C_WRITE_MSG_NUM) != I2C_WRITE_MSG_NUM) {
            HDF_LOGE("%s: i2c[%u] write failed", __func__, busCfg->i2cCfg.busNum);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

int32_t SetSensorPinMux(uint32_t regAddr, int32_t regSize, uint32_t regValue)
{
    uint8_t *base = NULL;
    if (regAddr == 0) {
        HDF_LOGE("%s: regAddr invalid", __func__);
        return HDF_FAILURE;
    }

    base = OsalIoRemap(regAddr, regSize);
    if (base == NULL) {
        HDF_LOGE("%s: ioremap fail", __func__);
        return HDF_FAILURE;
    }

    OSAL_WRITEL(regValue, base);
    OsalIoUnmap((void *)base);

    return HDF_SUCCESS;
}

int32_t CreateSensorThread(struct OsalThread *thread, OsalThreadEntry threadEntry, char *name, void *entryPara)
{
    struct OsalThreadParam config = {
        .name = name,
        .priority = OSAL_THREAD_PRI_DEFAULT,
        .stackSize = SENSOR_STACK_SIZE,
    };

    CHECK_NULL_PTR_RETURN_VALUE(thread, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(threadEntry, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(name, HDF_ERR_INVALID_PARAM);
    CHECK_NULL_PTR_RETURN_VALUE(entryPara, HDF_ERR_INVALID_PARAM);

    int32_t status = OsalThreadCreate(thread, threadEntry, entryPara);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor create thread failed!status=%d", __func__, status);
        return HDF_FAILURE;
    }

    status = OsalThreadStart(thread, &config);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s: sensor start thread failed!status=%d", __func__, status);
        OsalThreadDestroy(thread);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void DestroySensorThread(struct OsalThread *thread, uint8_t *status)
{
    int count = 0;
    CHECK_NULL_PTR_RETURN(thread);
    CHECK_NULL_PTR_RETURN(status);

    if (*status == SENSOR_THREAD_NONE || *status == SENSOR_THREAD_DESTROY) {
        HDF_LOGE("%s,delete thread not need!", __func__);
        return;
    }

    if (*status != SENSOR_THREAD_STOPPED) {
        *status = SENSOR_THREAD_STOPPING;
        /* wait until thread worker exit */
        while ((*status != SENSOR_THREAD_STOPPED) && (count < MAX_SENSOR_EXIT_THREAD_COUNT)) {
            OsalMSleep(MAX_SENSOR_WAIT_THREAD_TIME);
            count++;
        }
    }

    OsalThreadDestroy(thread);
    *status = SENSOR_THREAD_DESTROY;
}