/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  aht20.c
**  功能描述 :  aht20温湿度传感器驱动
**  作    者 :  王滨泉
**  日    期 :  2021.12.7
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.12.7
                1 首次创建                 
\******************************************************************************/
/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "aht20.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "iot_i2c.h"
#include "iot_errno.h"

// #include "wifiiot_i2c.h"
// #include "wifiiot_errno.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define AHT20_STARTUP_TIME          20*1000 // 上电启动时间
#define AHT20_CALIBRATION_TIME      40*1000 // 初始化（校准）时间
#define AHT20_MEASURE_TIME          75*1000 // 测量时间

#define AHT20_DEVICE_ADDR           0x38
#define AHT20_READ_ADDR             ((0x38<<1)|0x1)
#define AHT20_WRITE_ADDR            ((0x38<<1)|0x0)

#define AHT20_CMD_CALIBRATION       0xBE // 初始化（校准）命令
#define AHT20_CMD_CALIBRATION_ARG0  0x08
#define AHT20_CMD_CALIBRATION_ARG1  0x00

/**
 * 传感器在采集时需要时间,主机发出测量指令（0xAC）后，延时75毫秒以上再读取转换后的数据并判断返回的状态位是否正常。
 * 若状态比特位[Bit7]为0代表数据可正常读取，为1时传感器为忙状态，主机需要等待数据处理完成。
 **/
#define AHT20_CMD_TRIGGER           0xAC // 触发测量命令
#define AHT20_CMD_TRIGGER_ARG0      0x33
#define AHT20_CMD_TRIGGER_ARG1      0x00

// 用于在无需关闭和再次打开电源的情况下，重新启动传感器系统，软复位所需时间不超过20 毫秒
#define AHT20_CMD_RESET             0xBA // 软复位命令

#define AHT20_CMD_STATUS            0x71 // 获取状态命令

/**
 * STATUS 命令回复：
 * 1. 初始化后触发测量之前，STATUS 只回复 1B 状态值；
 * 2. 触发测量之后，STATUS 回复6B： 1B 状态值 + 2B 湿度 + 4b湿度 + 4b温度 + 2B 温度
 *      RH = Srh / 2^20 * 100%
 *      T  = St  / 2^20 * 200 - 50
 **/
#define AHT20_STATUS_BUSY_SHIFT     (7)       // bit[7] Busy indication
#define AHT20_STATUS_BUSY_MASK      (0x1<<AHT20_STATUS_BUSY_SHIFT)
#define AHT20_STATUS_BUSY(status)   ((status & AHT20_STATUS_BUSY_MASK) >> AHT20_STATUS_BUSY_SHIFT)

#define AHT20_STATUS_MODE_SHIFT     (5)       // bit[6:5] Mode Status
#define AHT20_STATUS_MODE_MASK      (0x3<<AHT20_STATUS_MODE_SHIFT)
#define AHT20_STATUS_MODE(status)   ((status & AHT20_STATUS_MODE_MASK) >> AHT20_STATUS_MODE_SHIFT)

                                            // bit[4] Reserved
#define AHT20_STATUS_CALI_SHIFT     (3)       // bit[3] CAL Enable
#define AHT20_STATUS_CALI_MASK      (0x1<<AHT20_STATUS_CALI_SHIFT)
#define AHT20_STATUS_CALI(status)   ((status & AHT20_STATUS_CALI_MASK) >> AHT20_STATUS_CALI_SHIFT)
                                            // bit[2:0] Reserved

#define AHT20_STATUS_RESPONSE_MAX   (6)

#define AHT20_RESLUTION             (1<<20)  // 2^20

#define AHT20_MAX_RETRY             (10)
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : AHT20_Read
* 功能描述 : AHT20读取数据
* 参    数 : buffer - 数据指针
             buffLen - 数据长度
* 返回值   : 读取结果
* 示    例 : AHT20_Read(&buffer, buffLen);
*/
/******************************************************************************/ 
static uint32_t AHT20_Read(uint8_t* buffer, uint32_t buffLen)
/******************************************************************************/ 
{
    uint32_t retval;

    retval = IoTI2cRead(AHT20_I2C_IDX,AHT20_READ_ADDR,buffer,buffLen);
    if (retval != IOT_SUCCESS) {
        printf("I2cRead() failed, %0X!\n", retval);
        return retval;
    }
    return IOT_SUCCESS;
}
/*
* 函数名称 : AHT20_Write
* 功能描述 : AHT20写入数据
* 参    数 : buffer - 数据指针
             buffLen - 数据长度
* 返回值   : 写入结果
* 示    例 : AHT20_Write(&buffer, buffLen);
*/
/******************************************************************************/ 
static uint32_t AHT20_Write(uint8_t* buffer, uint32_t buffLen)
/******************************************************************************/ 
{
    uint32_t retval = IoTI2cWrite(AHT20_I2C_IDX,AHT20_WRITE_ADDR,buffer,buffLen);
    if (retval != 0) {
        printf("I2cWrite(%02X) failed, %0X!\n", buffer[0], retval);
        return retval;
    }
    return IOT_SUCCESS;
}
/*
* 函数名称 : AHT20_StatusCommand
* 功能描述 : 发送获取状态命令
* 参    数 : 无
* 返回值   : 发送结果
* 示    例 : AHT20_Write();
*/
/******************************************************************************/ 
static uint32_t AHT20_StatusCommand(void)
/******************************************************************************/ 
{
    // 发送获取状态命令
    uint8_t statusCmd[] = { AHT20_CMD_STATUS };
    return AHT20_Write(statusCmd, sizeof(statusCmd));
}
/*
* 函数名称 : AHT20_ResetCommand
* 功能描述 : 发送软复位命令
* 参    数 : 无
* 返回值   : 发送结果
* 示    例 : AHT20_ResetCommand(void);
*/
/******************************************************************************/ 
static uint32_t AHT20_ResetCommand()
/******************************************************************************/ 
{
    // 发送软复位命令
    uint8_t resetCmd[] = {AHT20_CMD_RESET};
    return AHT20_Write(resetCmd, sizeof(resetCmd));
}
/*
* 函数名称 : AHT20_CalibrateCommand
* 功能描述 : 发送初始化校准命令
* 参    数 : 无
* 返回值   : 发送结果
* 示    例 : AHT20_CalibrateCommand();
*/
/******************************************************************************/ 
static uint32_t AHT20_CalibrateCommand(void)
/******************************************************************************/ 
{
    // 发送初始化校准命令
    uint8_t clibrateCmd[] = {AHT20_CMD_CALIBRATION, AHT20_CMD_CALIBRATION_ARG0, AHT20_CMD_CALIBRATION_ARG1};
    return AHT20_Write(clibrateCmd, sizeof(clibrateCmd));
}
/*
* 函数名称 : AHT20_Calibrate
* 功能描述 : 校准
* 参    数 : 无
* 返回值   : 结果
* 示    例 : AHT20_Calibrate();
*/
/******************************************************************************/ 
uint32_t AHT20_Calibrate(void)
/******************************************************************************/ 
{
    // 读取温湿度值之前， 首先要看状态字的校准使能位Bit[3]是否为 1(通过发送0x71可以获取一个字节的状态字)，
    // 如果不为1，要发送0xBE命令(初始化)，此命令参数有两个字节， 第一个字节为0x08，第二个字节为0x00。
    uint32_t retval = 0;
    uint8_t buffer[AHT20_STATUS_RESPONSE_MAX] = { AHT20_CMD_STATUS };
    memset(&buffer, 0x0, sizeof(buffer));

    retval = AHT20_StatusCommand();
    if (retval != IOT_SUCCESS) 
    {
        return retval;
    }

    retval = AHT20_Read(buffer, sizeof(buffer));
    if (retval != IOT_SUCCESS) 
    {
        return retval;
    }

    if (AHT20_STATUS_BUSY(buffer[0]) || !AHT20_STATUS_CALI(buffer[0])) 
    {
        retval = AHT20_ResetCommand();
        if (retval != IOT_SUCCESS) 
        {
            return retval;
        }
        usleep(AHT20_STARTUP_TIME);
        retval = AHT20_CalibrateCommand();
        usleep(AHT20_CALIBRATION_TIME);
        return retval;
    }

    return IOT_SUCCESS;
}
/*
* 函数名称 : AHT20_StartMeasure
* 功能描述 : 开始测量
* 参    数 : 无
* 返回值   : 结果
* 示    例 : AHT20_StartMeasure();
*/
/******************************************************************************/ 
uint32_t AHT20_StartMeasure(void)
/******************************************************************************/ 
{
    // 发送 触发测量 命令，开始测量
    uint8_t triggerCmd[] = {AHT20_CMD_TRIGGER, AHT20_CMD_TRIGGER_ARG0, AHT20_CMD_TRIGGER_ARG1};
    return AHT20_Write(triggerCmd, sizeof(triggerCmd));
}
/*
* 函数名称 : AHT20_GetMeasureResult
* 功能描述 : 开始测量
* 参    数 : 无
* 返回值   : 结果
* 示    例 : AHT20_GetMeasureResult();
*/
/******************************************************************************/ 
uint32_t AHT20_GetMeasureResult(int* temp, int* humi)
/******************************************************************************/ 
{
    // 接收测量结果，拼接转换为标准值
    uint32_t retval = 0, i = 0;
    if (temp == NULL || humi == NULL) 
    {
        return IOT_FAILURE;
    }

    uint8_t buffer[AHT20_STATUS_RESPONSE_MAX] = { 0 };
    memset(&buffer, 0x0, sizeof(buffer));
    retval = AHT20_Read(buffer, sizeof(buffer));  // recv status command result
    if (retval != IOT_SUCCESS) 
    {
        return retval;
    }

    for (i = 0; AHT20_STATUS_BUSY(buffer[0]) && i < AHT20_MAX_RETRY; i++) 
    {
        // printf("AHT20 device busy, retry %d/%d!\r\n", i, AHT20_MAX_RETRY);
        usleep(AHT20_MEASURE_TIME);
        retval = AHT20_Read(buffer, sizeof(buffer));  // recv status command result
        if (retval != IOT_SUCCESS) 
        {
            return retval;
        }
    }
    if (i >= AHT20_MAX_RETRY) 
    {
        printf("AHT20 device always busy!\r\n");
        return IOT_FAILURE;
    }

    uint32_t humiRaw = buffer[1];
    humiRaw = (humiRaw << 8) | buffer[2];
    humiRaw = (humiRaw << 4) | ((buffer[3] & 0xF0) >> 4);
    *humi = humiRaw / (float)AHT20_RESLUTION * 100;

    uint32_t tempRaw = buffer[3] & 0x0F;
    tempRaw = (tempRaw << 8) | buffer[4];
    tempRaw = (tempRaw << 8) | buffer[5];
    *temp = tempRaw / (float)AHT20_RESLUTION * 200 - 50;
    printf("humi = %05X, %d, temp= %05X, %d\r\n", humiRaw, *humi, tempRaw, *temp);
    return IOT_SUCCESS;
}
/******************************* End of File (C) ******************************/
