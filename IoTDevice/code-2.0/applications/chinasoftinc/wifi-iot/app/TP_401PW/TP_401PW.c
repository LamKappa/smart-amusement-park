/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  TP_401PW.c
**  功能描述 :  空气质量检测
**  作    者 :  王滨泉
**  日    期 :  2021.10.23
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.23
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
#include "hi_adc.h"
#include "hi_io.h"
#include "hi_pwm.h"
// #include "Bsp_Beep.h"
#include "Beep.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define TP_401PW_TASK_STACK_SIZE             4096                 //任务栈大小
#define TP_401PW_TASK_PRIO                   10                   //任务优先等级

#define TP_401PW_ADC_PIN                HI_ADC_CHANNEL_0

#define VOLTAGE_FILTER_NUM              (8)                     //平滑系数
#define EXCELLENT_THRESHOLD             100                     //优阈值
#define GOOD_THRESHOLD                  200                     //良阈值
#define MID_THRESHOLD                   300                     //中阈值
#define BAD_THRESHOLD                   400                     //差阈值
//------------------------------------------------------------------------------
// 空气质量等级
typedef enum 
{
    AIR_EXCELLENT,
    AIR_GOOD,
    AIR_MID,
    AIR_BAD,
}TE_AIR_LEVEL;
//------------------------------------------------------------------------------
// 传感器检测状态
typedef enum 
{
	DETECTION_PREHEAT = 0,              //预热阶段
    DETECTION_ZERO,                     //检测零点
	DETECTION_ACTION,                   //开始测试
}TE_DETECTION_STATE;
//------------------------------------------------------------------------------
// ADC配置句柄
typedef struct
{
    hi_adc_channel_index    Channel;        //通道编号
    hi_adc_equ_model_sel    EquMode;        //平均算法模式
    hi_adc_cur_bais         CurBais;        //模拟电源控制
    uint16_t                DelayCnt;       //延时计数
}TS_ADC_PARAM;

static TS_ADC_PARAM  s_Tp401pw_Adc = {0};
static TE_DETECTION_STATE s_Tp401pw_State = DETECTION_PREHEAT;
static uint8_t s_AirLevel = 0;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : TP_401PW_Init
* 功能描述 : 酒精检测传感器初始化
* 参    数 : 空
* 返回值   : 空
* 示    例 : TP_401PW_Init();
*/
/******************************************************************************/ 
static void TP_401PW_Init(void)
/******************************************************************************/ 
{
    //ADC 配置
    s_Tp401pw_Adc.Channel  = TP_401PW_ADC_PIN;
    s_Tp401pw_Adc.EquMode  = HI_ADC_EQU_MODEL_8;
    s_Tp401pw_Adc.CurBais  = HI_ADC_CUR_BAIS_DEFAULT;
    s_Tp401pw_Adc.DelayCnt = 0;
    return 0;
}
/*
* 函数名称 : Set_Detection_State
* 功能描述 : 设置检测状态
* 参    数 : state - 状态
* 返回值   : 空
* 示    例 : Set_Detection_State(state);
*/
/******************************************************************************/ 
static void Set_Detection_State(TE_DETECTION_STATE state)
/******************************************************************************/ 
{
    s_Tp401pw_State = state;
}

/*
* 函数名称 : Get_Detection_State
* 功能描述 : 获取检测状态
* 参    数 : 空
* 返回值   : state - 状态
* 示    例 : state = Get_Detection_State();
*/
/******************************************************************************/ 
static uint8_t Get_Detection_State(void)
/******************************************************************************/ 
{
   return s_Tp401pw_State;
}

/*
* 函数名称 : Get_Air_Quality
* 功能描述 : 获取空气质量
* 参    数 : 空
* 返回值   : 空气质量
* 示    例 : air = Get_Air_Quality();
*/
/******************************************************************************/ 
uint8_t Get_Air_Quality(void)
/******************************************************************************/ 
{
   return s_AirLevel;
}

/*
* 函数名称 : TP_401PW_Task
* 功能描述 : 空气质量检测任务
* 参    数 : argument - 任务参数
* 返回值   : 空
* 示    例 : TP_401PW_Task(&argument);
*/
/******************************************************************************/ 
void TP_401PW_Task(void *argument)
/******************************************************************************/ 
{

    uint16_t data;
    uint16_t voltage=0;
    static uint16_t tm_cnt = 0;
    static uint16_t min_20_cnt = 0;
    static uint16_t voltage_zero = 0;
    static uint16_t voltage_before = 0;
    uint16_t voltagebuff[8] = {0};
    uint8_t filter_cnt = 0;
    uint16_t diff_val = 0;
    uint8_t rising_trend_cnt,fall_trend_cnt;

    TP_401PW_Init();

    while (1)
    {
        sleep(1);
        hi_adc_read(s_Tp401pw_Adc.Channel,&data,s_Tp401pw_Adc.EquMode,s_Tp401pw_Adc.CurBais,s_Tp401pw_Adc.DelayCnt);
        voltage=(uint16_t)(hi_adc_convert_to_voltage(data)*1000);
        printf("%d\n",voltage);
        
        switch (Get_Detection_State())
        {
            //上电预热两分钟
            case DETECTION_PREHEAT:                                     
                if(++tm_cnt > 120)                                  
                {
                    tm_cnt = 0;
                    filter_cnt = 0;
                    Set_Detection_State(DETECTION_ZERO);
                    
                }
            break;

            case DETECTION_ZERO:                                        //取零点值
                voltagebuff[filter_cnt] = voltage;
                if(++filter_cnt >=  VOLTAGE_FILTER_NUM)
                {
                    voltage_zero = 0;
                    for (filter_cnt=0; filter_cnt<VOLTAGE_FILTER_NUM; filter_cnt++)                
                    {
                        voltage_zero += voltagebuff[filter_cnt];        //零点值取平均
                    }
                    voltage_zero = voltage_zero >> 3;
                    Set_Detection_State(DETECTION_ACTION);
                }
                
            break;

            case DETECTION_ACTION:
                if(voltage > voltage_before)                   //判断是否是下降趋势
                {
                    //电压处于上升趋势
                    fall_trend_cnt = 0;                                 
                    if(((voltage - voltage_before) < 50) && (++min_20_cnt >= 1200))      //是否保持20分钟上升趋势
                    {
                        filter_cnt = 0;
                        min_20_cnt = 0;
                        Set_Detection_State(DETECTION_ZERO);    //重新计算零点
                    }
                    else
                    {
                        if(voltage >= voltage_zero)             //获取差值
                        {
                            diff_val = voltage - voltage_zero;      
                        }
                        else
                        {
                            diff_val = voltage_zero - voltage;
                        }
                        
                    }
                }
                else
                {
                    //电压处于下降趋势
                    if(voltage < voltage_zero)
                    {
                        min_20_cnt = 0;
                        rising_trend_cnt = 0;                       
                        fall_trend_cnt++;                           
                        if(fall_trend_cnt >= 30)                    //连续30秒处于下降趋势
                        {
                            filter_cnt = 0;
                            fall_trend_cnt = 0;
                            Set_Detection_State(DETECTION_ZERO);    //重新计算零点
                        }
                    }
                    
                }
                if(diff_val > MID_THRESHOLD)                    //空气质量差
                {
                    s_AirLevel = AIR_BAD;
                }
                else if(diff_val > GOOD_THRESHOLD)              //空气质量中
                {
                    s_AirLevel = AIR_MID;
                }
                else if(diff_val > EXCELLENT_THRESHOLD)         //空气质量良
                {
                    s_AirLevel = AIR_GOOD;
                }
                else                                            //空气质量优
                {
                    s_AirLevel = AIR_EXCELLENT; 
                }
                voltage_before = voltage;                       //更新电压值
            break;
        }
        printf("\r\n[TP_401PW_Task]voltage_zero = %d, diff_val = %d, State = %d, s_AirLevel = %d \n",voltage_zero, diff_val, Get_Detection_State() ,s_AirLevel);
    }
}
/*
* 函数名称 : TP_401PW_Demo
* 功能描述 : TP_401PW使用示例
* 参    数 : 空
* 返回值   : 空
* 示    例 : TP_401PW_Demo();
*/
/******************************************************************************/ 
void TP_401PW_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "TP-401PW-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = TP_401PW_TASK_STACK_SIZE;
    attr.priority   = TP_401PW_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)TP_401PW_Task, NULL, &attr) == NULL) {
        printf("\r\n[TP_401PW_Demo] Falied to create air detection task!\n");
    }else{
        printf("\r\n[TP_401PW_Demo] Succ to create air detection task!\n");
    }
}

APP_FEATURE_INIT(TP_401PW_Demo);
/******************************* End of File (C) ******************************/