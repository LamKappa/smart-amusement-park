/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  sht40.c
**  功能描述 :  sht40温湿度传感器驱动
**  作    者 :  王滨泉
**  日    期 :  2021.10.09
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.09
                1 首次创建                 
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
// #include "sht40.h"
#include <hi_io.h>
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define SHT40_TASK_STACK_SIZE       4096                //任务栈大小
#define SHT40_TASK_PRIO             10                  //任务优先等级

#define FAN_PIN                     8                   //GPIO8控制散热风扇开关
#define FAN_PIN_MODE                IOT_GPIO_DIR_OUT    //GPIO8输出状态
#define FAN_POWER_PIN               12                  //GPIO12控制散热风扇电源开关
#define FAN_POWER_PIN_MODE          IOT_GPIO_DIR_OUT    //GPIO12输出状态

#define SHT40_I2C_IDX               0                   //I2C设备号
#define SHT40_I2C_BAUDRATE          (400*1000)          //I2C波特率
#define SHT40_ADDR                  0x44                //SHT40设备地址
#define SHT40_STATUS_RESPONSE_MAX   6                   //读取传感器数据长度

#define SHT40_CMD_TRIGGER           0xFD                //高精度测量命令

//------------------------------------------------------------------------------
// 土壤湿度控制模组句柄
typedef struct
{
    uint8_t Temp;                       //温度值
    uint8_t Humidity;                   //湿度值
    uint8_t TempThreshold;              //温度报警阈值
    uint8_t HumiThreshold;              //湿度报警阈值
}TS_SHT40_PARAM;


static TS_SHT40_PARAM s_Sht40 = {0};
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Set_Sht40_Environment_Temp_Alarm_Threshold
* 功能描述 : 设置环境温度报警阈值
* 参    数 : TempThreshold - 环境温度报警阈值
* 返回值   : 0 - 设置成功
            -1 - 设置失败
* 示    例 : result = Set_Sht40_Environment_Temp_Alarm_Threshold(TempThreshold);
*/
/******************************************************************************/ 
char Set_Sht40_Environment_Temp_Alarm_Threshold(uint8_t TempThreshold)
/******************************************************************************/
{
    s_Sht40.TempThreshold = TempThreshold;
    printf("\r\n Set TempThreshold = %d \r\n", TempThreshold);
    return 0;
}
/*
* 函数名称 : Set_Sht40_Environment_Humidity_Alarm_Threshold
* 功能描述 : 设置环境湿度报警阈值
* 参    数 : HumiThreshold - 环境湿度报警阈值
* 返回值   : 0 - 设置成功
            -1 - 设置失败
* 示    例 : result = Set_Sht40_Environment_Humidity_Alarm_Threshold(HumiThreshold);
*/
/******************************************************************************/ 
char Set_Sht40_Environment_Humidity_Alarm_Threshold(uint8_t HumiThreshold)
/******************************************************************************/
{
    s_Sht40.HumiThreshold = HumiThreshold;
    printf("\r\n Set HumiThreshold = %d \r\n", HumiThreshold);
    return 0;
}
/*
* 函数名称 : Get_Sht40_Environment_Temp_Alarm_Threshold
* 功能描述 : 获取环境温度报警阈值
* 参    数 : 无
* 返回值   : 环境温度报警阈值
* 示    例 : TempThreshold = Get_Sht40_Environment_Temp_Alarm_Threshold();
*/
/******************************************************************************/ 
uint8_t Get_Sht40_Environment_Temp_Alarm_Threshold(void)
/******************************************************************************/
{
    return s_Sht40.TempThreshold;
}
/*
* 函数名称 : Get_Sht40_Environment_Humidity_Alarm_Threshold
* 功能描述 : 获取环境湿度报警阈值
* 参    数 : 无 
* 返回值   : 环境湿度报警阈值
* 示    例 : HumiThreshold = Get_Sht40_Environment_Humidity_Alarm_Threshold();
*/
/******************************************************************************/ 
uint8_t Get_Sht40_Environment_Humidity_Alarm_Threshold(void)
/******************************************************************************/
{
    return s_Sht40.HumiThreshold;
}
/*
* 函数名称 : Get_Sht40_Environment_Temp
* 功能描述 : 获取环境温度
* 参    数 : 无
* 返回值   : 环境温度
* 示    例 : Temp = Get_Sht40_Environment_Temp();
*/
/******************************************************************************/ 
uint8_t Get_Sht40_Environment_Temp(void)
/******************************************************************************/
{
    return s_Sht40.Temp;
}
/*
* 函数名称 : Get_Sht40_Environment_Humidity
* 功能描述 : 获取环境湿度
* 参    数 : 无
* 返回值   : 环境湿度
* 示    例 : Humi = Get_Sht40_Environment_Humidity();
*/
/******************************************************************************/ 
uint8_t Get_Sht40_Environment_Humidity(void)
/******************************************************************************/
{
    return s_Sht40.Humidity;
}

/*
* 函数名称 : SHT40_Read
* 功能描述 : 读取温湿度数据
* 参    数 : buffer - 读取数据指针
             buffLen - 读取数据长度
* 返回值   : 0 - 读取成功
            -1 - 读取失败
* 示    例 : result = SHT40_Read(&buffer,buffLen);
*/
/******************************************************************************/ 
static uint32_t SHT40_Read(uint8_t* buffer, uint32_t buffLen)
/******************************************************************************/ 
{
    uint32_t retval;

    retval = IoTI2cRead(SHT40_I2C_IDX,(SHT40_ADDR<<1)|1,buffer,buffLen);
    if (retval != 0) {
        printf("I2cRead() failed, %0X!\n", retval);
        return retval;
    }
    return 0;
}
/*
* 函数名称 : SHT40_Write
* 功能描述 : 写命令
* 参    数 : buffer - 写入数据指针
             buffLen - 写入数据长度
* 返回值   : 0 - 写入成功
            -1 - 写入失败
* 示    例 : result = SHT40_Write(&buffer,buffLen);
*/
/******************************************************************************/ 
static uint32_t SHT40_Write(uint8_t* buffer, uint32_t buffLen)
/******************************************************************************/
{
    uint32_t retval = IoTI2cWrite(SHT40_I2C_IDX,(SHT40_ADDR<<1)|0,buffer,buffLen);
    if (retval != 0) {
        printf("I2cWrite(%02X) failed, %0X!\n", buffer[0], retval);
        return retval;
    }
    return 0;
}
/*
* 函数名称 : SHT40_GetMeasureResult
* 功能描述 : 获取测量结果，拼接转换为标准值
* 参    数 : temp - 温度值
             humi - 湿度值
* 返回值   : 0 - 测量成功
            -1 - 测量失败
* 示    例 : result = SHT40_GetMeasureResult(&temp,humi);
*/
/******************************************************************************/ 
uint32_t SHT40_GetMeasureResult(uint8_t* temp, uint8_t* humi)
/******************************************************************************/ 
{

    uint32_t retval = 0;

    float t_degC=0;
    float rh_pRH=0;
    float t_ticks=0.0;
    float rh_ticks=0.0;
    
    if (temp == NULL || humi == NULL) 
    {
        return -1;
    }

    uint8_t buffer[SHT40_STATUS_RESPONSE_MAX] = { 0 };
    memset(&buffer, 0x0, sizeof(buffer));
    retval = SHT40_Read(buffer, sizeof(buffer));  // recv status command result
    if (retval != 0) 
    {
        return retval;
    }

    t_ticks=buffer[0]*256+buffer[1];
    rh_ticks=buffer[3]*256+buffer[4];
    t_degC=-45+175*t_ticks/65535;
    rh_pRH=-6+125*rh_ticks/65535; 
    if(rh_pRH >= 100)
    {
        rh_pRH = 100;
    }
    if(rh_pRH < 0)
    {
        rh_pRH = 0;
    }
    *humi = (uint8_t)rh_pRH;

    *temp = (uint8_t)t_degC;

    return 0;
}
/*
* 函数名称 : SHT40_StartMeasure
* 功能描述 : 开始测量
* 参    数 : temp - 温度值
             humi - 湿度值
* 返回值   : 0 - 操作成功
            -1 - 操作失败
* 示    例 : result = SHT40_StartMeasure();
*/
/******************************************************************************/ 
uint32_t SHT40_StartMeasure(void)
/******************************************************************************/ 
{
    uint8_t triggerCmd[] = {SHT40_CMD_TRIGGER};
    return SHT40_Write(triggerCmd, sizeof(triggerCmd));
}
/*
* 函数名称 : SHT40_Init
* 功能描述 : 温湿度传感器硬件初始化
* 参    数 : 无
* 返回值   : 无
* 示    例 : SHT40_Init();
*/
/******************************************************************************/ 
void SHT40_Init(void)
/******************************************************************************/ 
{
    IoTI2cInit(SHT40_I2C_IDX, SHT40_I2C_BAUDRATE);          //I2C初始化
}
/*
* 函数名称 : Fan_Init
* 功能描述 : 风扇硬件初始化
* 参    数 : 无
* 返回值   : 无
* 示    例 : Fan_Init();
*/
/******************************************************************************/ 
void Fan_Init(void)
/******************************************************************************/ 
{
    IoTGpioInit(FAN_PIN);                                   //初始化散热风扇GPIO
    IoTGpioSetDir(FAN_PIN, FAN_PIN_MODE);                   //设置GPIO工作状态
    IoTGpioSetOutputVal(FAN_PIN,IOT_GPIO_VALUE0);           //关闭散热风扇

    hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO); /* GPIO12 */
    IoTGpioInit(FAN_POWER_PIN);                             //初始化散热风扇电源GPIO
    IoTGpioSetDir(FAN_POWER_PIN, FAN_POWER_PIN_MODE);       //设置GPIO工作状态
    IoTGpioSetOutputVal(FAN_POWER_PIN,IOT_GPIO_VALUE0);     //关闭散热风扇电源
}
/*
* 函数名称 : SHT40Task
* 功能描述 : 温湿度传感器任务
* 参    数 : arg - 任务句柄
* 返回值   : 空
* 示    例 : SHT40Task();
*/
/******************************************************************************/ 
void SHT40Task(void *arg)
/******************************************************************************/ 
{
    (void)arg;
    uint32_t retval = 0;
    float humidity = 0.0f;
    float temperature = 0.0f;

    Fan_Init();                         //风扇IO初始化
    SHT40_Init();                       //温湿度传感器IO初始化
    s_Sht40.TempThreshold = 20;         //温度阈值20℃

    while(1) 
    {
        SHT40_StartMeasure();
        usleep(20*1000);  
        SHT40_GetMeasureResult(&s_Sht40.Temp, &s_Sht40.Humidity);           //获取当前温湿度值 
        printf("\r\n=======================================\r\n");
        printf("\r\ntemperature is %d, humidity is %d\r\n", s_Sht40.Temp, s_Sht40.Humidity);
        printf("\r\n=======================================\r\n");
        if(s_Sht40.Temp >= s_Sht40.TempThreshold)
        {
            IoTGpioSetOutputVal(FAN_PIN,IOT_GPIO_VALUE1);                   //开启散热风扇
            IoTGpioSetOutputVal(FAN_POWER_PIN,IOT_GPIO_VALUE1);             //开启散热风扇电源
        }
        else
        {
            IoTGpioSetOutputVal(FAN_PIN,IOT_GPIO_VALUE0);                   //关闭散热风扇
            IoTGpioSetOutputVal(FAN_POWER_PIN,IOT_GPIO_VALUE0);             //关闭散热风扇电源
        }
        sleep(1);   
    }
}
/*
* 函数名称 : SHT40Demo
* 功能描述 : 创建传感器任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : SHT40Demo();
*/
/******************************************************************************/ 
void SHT40Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "SHT40Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = SHT40_TASK_STACK_SIZE;
    attr.priority   = SHT40_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)SHT40Task, NULL, &attr) == NULL) {
        printf("\r\n[SHT40Demo] Falied to create SHT40Task!\n");
    }else{
        printf("\r\n[SHT40Demo] Succ to create SHT40Task!\n");
    }
}

APP_FEATURE_INIT(SHT40Demo);
/******************************* End of File (C) ******************************/