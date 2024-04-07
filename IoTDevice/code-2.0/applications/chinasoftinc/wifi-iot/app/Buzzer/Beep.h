/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  Beep.h
**  功能描述 :  蜂鸣器控制
**  作    者 :  王滨泉
**  日    期 :  2021.10.10
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.10
                1 首次创建                 
\******************************************************************************/
#ifndef _BUZZER_H_
#define _BUZZER_H_
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_pwm.h"
#include "hi_systick.h"
#include <hi_hrtimer.h>

/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
//定义蜂鸣器类型，有源或无源
#define ACTIVE_BUZZER					1
#define PASSIVE_BUZZER					2
#define BUZZER_TYPE						PASSIVE_BUZZER
//----------------------------------------------------------------------------------------
//io宏定义
#define BEEP_PIN_NAME                   HI_IO_NAME_GPIO_9
#define BEEP_PIN_MODE               	HI_IO_FUNC_GPIO_9_PWM0_OUT
#define BEEP_PIN_PWM_NUM              	HI_PWM_PORT_PWM0
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
//蜂鸣器工作状态
typedef enum 
{
	BUZZER_OFF = 0x0,
	BUZZER_ON,
	BUZZER_ALARM,
	BUZZER_KEEP_ALARM,
}TE_BUZZER_STATE;

//蜂鸣器控制结构体
typedef struct{
	uint8_t OffFlg;
	uint8_t State;
	uint16_t Duty;				//占空比
	uint16_t Period;			//周期
	uint16_t AlarmTimes;		//鸣叫次数
	uint16_t Timer;				//计时器
}TS_BUZZER_INFO;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern void BUZZER_Init(void);
extern void BUZZER_Drv(void);
extern void BUZZER_Set(uint8_t State,uint16_t Duty,uint16_t Period,uint16_t Times);
//----------------------------------------------------------------------------------------
#endif
/***********************************END OF FILE*******************************************/
