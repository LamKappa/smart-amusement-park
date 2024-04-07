/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  Gas.c
**  功能描述 :  烟雾感应
**  作    者 :  王滨泉
**  日    期 :  2021.12.7
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.12.7
                1 首次创建                 
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "hi_io.h"
#include "hi_pwm.h"
#include <hi_adc.h>
#include "iot_gpio.h"
#include "Beep.h"
#include "aht20.h"
#include "iot_i2c.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define GAS_TASK_STACK_SIZE             (1024*4)                //任务栈大小
#define GAS_TASK_PRIO                   (10)                    //任务优先等级

#define GAS_ADC_CHANNEL                 HI_ADC_CHANNEL_5        //烟雾检测通道
#define GAS_ADC_EQUMODE                 HI_ADC_EQU_MODEL_8      //adc采用模式
#define GAS_ADC_CURBAIS                 HI_ADC_CUR_BAIS_DEFAULT //模拟电源控制
#define VLT_MIN                         (100)
#define RL			                    (10)		            // RL阻值
//------------------------------------------------------------------------------
// ADC配置句柄
typedef struct
{
    hi_adc_channel_index    Channel;        //通道编号
    hi_adc_equ_model_sel    EquMode;        //平均算法模式
    hi_adc_cur_bais         CurBais;        //模拟电源控制
    uint16_t                DelayCnt;       //延时计数
}TS_ADC_PARAM;

//------------------------------------------------------------------------------
// 土壤湿度控制模组句柄
typedef struct
{
    int Temp;                             //温度值
    int Humidity;                         //湿度值
    uint16_t Resistance;                  //电阻值
}TS_GAS_PARAM;

TS_ADC_PARAM s_GasAdc = {0};
TS_GAS_PARAM s_GasParam = {0};
static float R0 = 22;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Get_Aht20_Environment_Temp
* 功能描述 : 获取环境温度
* 参    数 : 无
* 返回值   : 环境温度
* 示    例 : Temp = Get_Aht20_Environment_Temp();
*/
/******************************************************************************/ 
float Get_Aht20_Environment_Temp(void)
/******************************************************************************/
{
    return s_GasParam.Temp;
}
/*
* 函数名称 : Get_Aht20_Environment_Humidity
* 功能描述 : 获取环境湿度
* 参    数 : 无
* 返回值   : 环境湿度
* 示    例 : Humidity = Get_Aht20_Environment_Humidity();
*/
/******************************************************************************/ 
float Get_Aht20_Environment_Humidity(void)
/******************************************************************************/
{
    return s_GasParam.Humidity;
}

/*
* 函数名称 : Get_Gas_Resistance
* 功能描述 : 获取传感器阻值
* 参    数 : 无
* 返回值   : 环境湿度
* 示    例 : Humidity = Get_Gas_Resistance();
*/
/******************************************************************************/ 
float Get_Gas_Sensor_Resistance(void)
/******************************************************************************/
{
    return s_GasParam.Resistance;
}

/*
* 函数名称 : Gas_Init
* 功能描述 : 人体感应初始化
* 参    数 : 空
* 返回值   : 空
* 示    例 : Gas_Init();
*/
/******************************************************************************/ 
void Gas_Init(void)
/******************************************************************************/ 
{
    s_GasAdc.Channel = HI_ADC_CHANNEL_5;
    s_GasAdc.EquMode = HI_ADC_EQU_MODEL_8;
    s_GasAdc.CurBais = HI_ADC_CUR_BAIS_DEFAULT;
    s_GasAdc.DelayCnt = 0;
}


/*
* 函数名称 : Gas_Task
* 功能描述 : 烟雾感应任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Gas_Task(&argument);
*/
/******************************************************************************/ 
void Gas_Task(void *argument)
/******************************************************************************/ 
{
    uint8_t i = 0;
    uint16_t data;
    uint32_t retval = 0;
    float temp = 0.0f;
    float voltage;
    float vlt_max = 0;
    float vlt_min = VLT_MIN;
    hi_double ppm =0;
    hi_float aht_ratio =0;
    static hi_float ratio_d = 0.26;
    static hi_float ratio_d_s =0.28;
    IoTI2cInit(AHT20_I2C_IDX, AHT20_BAUDRATE);
    Gas_Init();
    BUZZER_Set(BUZZER_ALARM, 100, 1000, 3);      //记录空值
    while(1)
    {	
        retval = AHT20_StartMeasure();
        if (retval != IOT_SUCCESS) 
        {
            printf("trigger measure failed!\r\n");
        }

        retval = AHT20_GetMeasureResult(&s_GasParam.Temp, &s_GasParam.Humidity);
        if (retval != IOT_SUCCESS) 
        {
            printf("get humidity data failed!\r\n");
        }

        if (hi_adc_read(s_GasAdc.Channel, &data, s_GasAdc.EquMode, s_GasAdc.CurBais, s_GasAdc.DelayCnt) == HI_ERR_SUCCESS) 
        {
            float Vx = hi_adc_convert_to_voltage(data);

            printf("Vx: %.2f \r\n", Vx);
            voltage = Vx;
            vlt_max = (voltage > vlt_max) ? voltage : vlt_max;
            vlt_min = (voltage < vlt_min) ? voltage : vlt_min;
            float RS = (5 - voltage) / voltage * RL;                //规格书中电阻计算方法
            printf("RS:%03f\r\n", RS);
            printf("R0/RS:%03f\r\n", RS/R0);
            aht_ratio = (float) (RS/R0);
            if (aht_ratio >= 0.025 && aht_ratio <= 0.27) 
            {
                if (aht_ratio > 0.195 && aht_ratio <= 0.27) 
                {
                    ppm = 100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_1:%03f\r\n", ppm); 
                } 
                else if(aht_ratio >= 0.175 && aht_ratio <= 0.195) 
                {
                    ppm =100+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_2:%03f\r\n", ppm); 
                } 
                else if (aht_ratio >= 0.160 && aht_ratio <= 0.175) 
                {
                    ppm =200+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_3:%03f\r\n", ppm); 
                } 
                else if(aht_ratio >= 0.150 && aht_ratio < 0.160) 
                {
                    ppm =300+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_4:%03f\r\n", ppm); 
                }
                else if (aht_ratio >= 0.140 && aht_ratio < 0.150) 
                {
                    ppm =400+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_5:%03f\r\n", ppm); 
                } 
                else if (aht_ratio >= 0.130 && aht_ratio < 0.140) 
                {
                    ppm =400+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_6:%03f\r\n", ppm); 
                }
                else if (aht_ratio >= 0.078 && aht_ratio < 0.130) 
                {
                    ppm =700+100*pow(11.5428*35.904*voltage/(25.5-1*voltage), 0.6549);              
                    printf("ppm_7:%03f\r\n", ppm); 
                }
            }
            else
            {
                ppm = 0;
                printf("ppm_0:0\r\n"); 
            }
            s_GasParam.Resistance = (uint16_t)RS;
        }
        sleep(1);
    }
}
/*
* 函数名称 : Gas_Demo
* 功能描述 : 烟雾感应任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Gas_Demo(void);
*/
/******************************************************************************/ 
void Gas_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "Gas-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = GAS_TASK_STACK_SIZE;
    attr.priority   = GAS_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Gas_Task, NULL, &attr) == NULL) {
        printf("\r\n[Gas_Demo] Falied to create Gas task!\n");
    }else{
        printf("\r\n[Gas_Demo] Succ to create Gas task!\n");
    }
}

APP_FEATURE_INIT(Gas_Demo);
/******************************* End of File (C) ******************************/