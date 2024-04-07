/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  PIR.c
**  功能描述 :  人体感应
**  作    者 :  王滨泉
**  日    期 :  2021.12.6
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.12.6
                1 首次创建                 
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "hi_io.h"
#include "hi_pwm.h"
#include <hi_adc.h>
#include "iot_gpio.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define PIR_TASK_STACK_SIZE             1024*4                  //任务栈大小
#define PIR_TASK_PRIO                   10                      //任务优先等级

#define RED_PIN_NAME                    HI_IO_NAME_GPIO_11
#define RED_PIN_FUNC               	    HI_IO_FUNC_GPIO_11_PWM2_OUT
#define RED_PIN_PWM_NUM              	HI_PWM_PORT_PWM2

#define GREEN_PIN_NAME                  HI_IO_NAME_GPIO_10
#define GREEN_PIN_FUNC               	HI_IO_FUNC_GPIO_10_PWM1_OUT
#define GREEN_PIN_PWM_NUM              	HI_PWM_PORT_PWM1

#define BLUE_PIN_NAME                   HI_IO_NAME_GPIO_12
#define BLUE_PIN_FUNC               	HI_IO_FUNC_GPIO_12_PWM3_OUT
#define BLUE_PIN_PWM_NUM              	HI_PWM_PORT_PWM3

#define LIGTH_ADC_CHANNEL               HI_ADC_CHANNEL_4        //光敏检测
#define HUMAN_ADC_CHANNEL               HI_ADC_CHANNEL_3        //人体感应

#define LED_DELAY_TIME_US               300*1000
#define PWM_FREQ_DIVITION               64000
//------------------------------------------------------------------------------
//COLOUR id
typedef enum
{
    COLOUR_RED = 0,				    //红
	COLOUR_GREEN,					//绿
	COLOUR_BLUE,					//黄
	COLOUR_ID_MAX
}TE_COLOUR_ID;

//------------------------------------------------------------------------------
//human status
typedef enum
{
    HUMAN_LEAVE = 0,				//离开
	HUMAN_COME,					    //有人
}TE_HUMAN_STATUS;
//------------------------------------------------------------------------------
//light state
typedef enum
{
    LIGHT_DAY = 0,				    //白天
	LIGHT_NIGHT,					//夜晚
}TE_LIGHT_STATE;

//-----------------------------------------------------------------------------
//led info
typedef struct {
	uint8_t pin;
    uint8_t mode;
    uint8_t func;
    uint8_t pwm_num;
	uint8_t on;
	uint8_t off;
}TS_LED_CONTROL_INFO;


const static TS_LED_CONTROL_INFO s_LedControl[] = {
	{RED_PIN_NAME,	    IOT_GPIO_DIR_OUT,   RED_PIN_FUNC,   RED_PIN_PWM_NUM,    1, 0},
	{GREEN_PIN_NAME,	IOT_GPIO_DIR_OUT,   GREEN_PIN_FUNC, GREEN_PIN_PWM_NUM,  1, 0},
	{BLUE_PIN_NAME,     IOT_GPIO_DIR_OUT,   BLUE_PIN_FUNC,  BLUE_PIN_PWM_NUM,   1, 0},
};

static uint8_t s_HumanStatus = 0;
static uint8_t s_LightState = 0;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Set_Light_State
* 功能描述 : 设置光照状态
* 参    数 : State - 0：白天，1：夜晚
* 返回值   : 空
* 示    例 : Set_Light_State(State);
*/
/******************************************************************************/ 
void Set_Light_State(uint8_t State)
/******************************************************************************/ 
{
    s_LightState = State;
}
/*
* 函数名称 : Get_Light_State
* 功能描述 : 获取光照状态
* 参    数 : 空
* 返回值   : 光照状态 - 0：白天，1：夜晚
* 示    例 : State = Get_Light_State();
*/
/******************************************************************************/ 
uint8_t Get_Light_State(void)
/******************************************************************************/ 
{
    return s_LightState;
}
/*
* 函数名称 : Set_Human_Status
* 功能描述 : 设置感应状态
* 参    数 : Status - 0：无人，1：有人经过
* 返回值   : 空
* 示    例 : Set_Human_Status(State);
*/
/******************************************************************************/ 
void Set_Human_Status(uint8_t Status)
/******************************************************************************/ 
{
    s_HumanStatus = Status;
}
/*
* 函数名称 : Get_Human_Status
* 功能描述 : 获取感应状态
* 参    数 : 空
* 返回值   : 感应状态 - 0：无人，1：有人经过
* 示    例 : Status = Get_Human_Status();
*/
/******************************************************************************/ 
uint8_t Get_Human_Status(void)
/******************************************************************************/ 
{
    return s_HumanStatus;
}
/*
* 函数名称 : PIR_Init
* 功能描述 : 人体感应初始化
* 参    数 : 空
* 返回值   : 空
* 示    例 : PIR_Init();
*/
/******************************************************************************/ 
void PIR_Init(void)
/******************************************************************************/ 
{
    uint8_t i;
    for(i =0; i<COLOUR_ID_MAX; i++)
    {
        hi_io_set_func(s_LedControl[i].pin,s_LedControl[i].func); 
        hi_pwm_init(s_LedControl[i].pwm_num);
        // hi_pwm_set_clock(PWM_CLK_160M);
        hi_pwm_set_clock(PWM_CLK_XTAL);
        hi_pwm_stop(s_LedControl[i].pwm_num);
    }
    for(i =0; i<2; i++)
    {
        for(uint8_t j =0; j<COLOUR_ID_MAX; j++)
        {
            hi_pwm_start(s_LedControl[j].pwm_num,PWM_FREQ_DIVITION/2, PWM_FREQ_DIVITION);
            usleep(LED_DELAY_TIME_US);
            hi_pwm_stop(s_LedControl[j].pwm_num);
            usleep(LED_DELAY_TIME_US);
        }
    }

}


/*
* 函数名称 : PIR_Task
* 功能描述 : 人体感应任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : PIR_Task(&argument);
*/
/******************************************************************************/ 
void PIR_Task(void *argument)
/******************************************************************************/ 
{
    uint8_t i = 0;
    uint16_t data;
    static uint16_t light_cnt = 0;

    PIR_Init();

    while(1)
    {
        usleep(10*1000);	//10ms

		if(hi_adc_read(HUMAN_ADC_CHANNEL,&data,HI_ADC_EQU_MODEL_8,HI_ADC_CUR_BAIS_DEFAULT,1) == HI_ERR_SUCCESS)
        {
            if(data < 500)
            {
                Set_Human_Status(HUMAN_LEAVE);
                hi_pwm_stop(s_LedControl[COLOUR_GREEN].pwm_num);
            }
            else
            {
                Set_Human_Status(HUMAN_COME);
                hi_pwm_start(s_LedControl[COLOUR_GREEN].pwm_num,PWM_FREQ_DIVITION/2, PWM_FREQ_DIVITION);
            }
        }
        // usleep(50);
        // if(hi_adc_read(LIGTH_ADC_CHANNEL,&data,HI_ADC_EQU_MODEL_8,HI_ADC_CUR_BAIS_DEFAULT,0) == HI_ERR_SUCCESS)
        // {
        //     if(data < 1000)
        //     {
        //         light_cnt = 0;
        //         Set_Light_State(LIGHT_DAY);
        //         hi_pwm_stop(s_LedControl[COLOUR_RED].pwm_num);
        //     }
        //     else
        //     {
        //         if(++light_cnt >= 50)
        //         {
        //             light_cnt = 0;
        //             Set_Light_State(LIGHT_NIGHT);
        //             hi_pwm_start(s_LedControl[COLOUR_RED].pwm_num,PWM_FREQ_DIVITION/2, PWM_FREQ_DIVITION);
        //         }
        //     }
        // }
    }
}
/*
* 函数名称 : PIR_Demo
* 功能描述 : 人体感应任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : PIR_Demo(void);
*/
/******************************************************************************/ 
void PIR_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "PIR-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = PIR_TASK_STACK_SIZE;
    attr.priority   = PIR_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)PIR_Task, NULL, &attr) == NULL) {
        printf("\r\n[PIR_Demo] Falied to create PIR task!\n");
    }else{
        printf("\r\n[PIR_Demo] Succ to create PIR task!\n");
    }
}

APP_FEATURE_INIT(PIR_Demo);
/******************************* End of File (C) ******************************/