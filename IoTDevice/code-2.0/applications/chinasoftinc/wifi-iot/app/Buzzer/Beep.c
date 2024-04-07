/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  Beep.c
**  功能描述 :  蜂鸣器控制
**  作    者 :  王滨泉
**  日    期 :  2021.10.10
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.10
                1 首次创建                 
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "Beep.h"


/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define BUZZER_TASK_STACK_SIZE      1024                   	//任务栈大小
#define BUZZER_TASK_PRIO            30                     	//任务优先等级
#define BEEP_PIN                 	9                      	//蜂鸣器引脚
#define BUZZER_HZ					36000

static TS_BUZZER_INFO s_Buzzer;

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : BuzzerOn
* 功能描述 : 蜂鸣器开
* 参    数 : 无
* 返回值   : 无
* 示    例 : BuzzerOn();
*/
/******************************************************************************/ 
static void BuzzerOn(void)
/******************************************************************************/ 
{
	if(s_Buzzer.OffFlg == 0)
	{
		s_Buzzer.OffFlg = 1;
		hi_pwm_start(HI_PWM_PORT_PWM0, BUZZER_HZ/2, BUZZER_HZ);
	}
}
/*
* 函数名称 : BuzzerOff
* 功能描述 : 蜂鸣器关
* 参    数 : 无
* 返回值   : 无
* 示    例 : BuzzerOff();
*/
/******************************************************************************/ 
static void BuzzerOff(void)
/******************************************************************************/ 
{
	
	hi_pwm_stop(HI_PWM_PORT_PWM0);
	s_Buzzer.OffFlg = 0;
}

/*
* 函数名称 : BuzzerControl
* 功能描述 : 蜂鸣器控制
* 参    数 : Buz - 蜂鸣器控制结构体指针
* 返回值   : 无
* 示    例 : BuzzerControl(&Buz);
*/
/******************************************************************************/ 
static void BuzzerControl(TS_BUZZER_INFO *Buz)
/******************************************************************************/ 
{
	switch(Buz->State)
	{
		case BUZZER_ON:
			BuzzerOn();
		break;
		
		case BUZZER_OFF:
			BuzzerOff();
		break;
		
		case BUZZER_ALARM:
			if(Buz->Timer < Buz->Duty)
			{
				BuzzerOn();
			}
			else
			{
				BuzzerOff();
			}
			if(Buz->Timer >= Buz->Period)
			{
				Buz->Timer = 0;
				
				if(Buz->AlarmTimes)
				{
					Buz->AlarmTimes--;
				}
				if(Buz->AlarmTimes == 0)
				{
					Buz->State = BUZZER_OFF;
					BuzzerOff();
				}
			}
		break;
			
		case BUZZER_KEEP_ALARM:
			if(Buz->Timer < Buz->Duty)
			{
				BuzzerOn();
			}
			else
			{
				BuzzerOff();
			}
			if(Buz->Timer >= Buz->Period)
			{
				Buz->Timer = 0;
			}
		break;
			
		default:
			BuzzerOff();
		break;
	}
	
	s_Buzzer.Timer += 10;
}



/*
* 函数名称 : BUZZER_Drv
* 功能描述 : 蜂鸣器驱动，需放到10ms时间片中
* 参    数 : 无
* 返回值   : 无
* 示    例 : BUZZER_Drv();
*/
/******************************************************************************/ 
void BUZZER_Drv(void)
/******************************************************************************/ 
{
	BuzzerControl(&s_Buzzer);
}

/*
* 函数名称 : BUZZER_Init
* 功能描述 : 蜂鸣器初始化
* 参    数 : Buz - 蜂鸣器控制结构体指针
* 返回值   : 无
* 示    例 : BUZZER_Init();
*/
/******************************************************************************/ 
void BUZZER_Init(void)
/******************************************************************************/ 
{
    hi_io_set_func(BEEP_PIN_NAME,BEEP_PIN_MODE); 
    hi_pwm_init(BEEP_PIN_PWM_NUM);
    hi_pwm_set_clock(PWM_CLK_160M);  
	BuzzerOff();
	printf("\r\n[BUZZER_Init] BUZZER_Init OK!!!\n");

}

/*
* 函数名称 : BUZZER_Drv
* 功能描述 : 蜂鸣器工作方式设置
* 参    数 : State - 工作方式 
			 Duty - 鸣叫占空比 
			 Period - 鸣叫周期 
			 Times - 鸣叫次数
* 返回值   : 无
* 示    例 : BUZZER_Set(State,Duty,Period,Times)
*/
/******************************************************************************/ 
void BUZZER_Set(uint8_t State,uint16_t Duty,uint16_t Period,uint16_t Times)
/******************************************************************************/ 
{
	s_Buzzer.State = State;
	s_Buzzer.Duty = Duty;
	s_Buzzer.Period = Period;
	s_Buzzer.AlarmTimes = Times;
	s_Buzzer.Timer = 0;
}
/*
* 函数名称 : Buzzer_Task
* 功能描述 : 蜂鸣器任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Buzzer_Task(&argument);
*/
/******************************************************************************/ 
void Buzzer_Task(void *argument)
/******************************************************************************/ 
{
    BUZZER_Init();			//初始化IO

    while (1)
    {
        usleep(10*1000);
        BUZZER_Drv();
    }
}
/*
* 函数名称 : Buzzer_Demo
* 功能描述 : 创建蜂鸣器任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Buzzer_Demo(void);
*/
/******************************************************************************/ 
void Buzzer_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "Buzzer-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = BUZZER_TASK_STACK_SIZE;
    attr.priority   = BUZZER_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Buzzer_Task, NULL, &attr) == NULL) {
        printf("\r\n[Buzzer_Demo] Falied to create buzzer task!\n");
    }else{
        printf("\r\n[Buzzer_Demo] Succ to create buzzer task!\n");
    }
}

APP_FEATURE_INIT(Buzzer_Demo);
/******************************* End of File (C) ******************************/

