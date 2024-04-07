/******************************************************************************\
**  版     权：深圳开鸿数字产业发展有限公司（2021）
**  文件名称：Led.c
**  功能描述：LED模块化程序，支持设置LED常亮，常灭，闪烁周期设置，闪烁次数设置等功能。
**  作     者：王滨泉
**  日     期：2018-01-01
**  版     本：V0.0.1
**  变更记录：V0.0.1/2018-01-01
              1.首次创建
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "Led.h" 
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define LED_TASK_STACK_SIZE      1024                   //任务栈大小
#define LED_TASK_PRIO            10                     //任务优先等级


//-----------------------------------------------------------------------------
//led control
typedef struct {
	uint8_t pin;
	uint8_t on;
	uint8_t off;
}TS_LED_CONTROL_INFO;


const TS_LED_CONTROL_INFO s_LedControl[] = {
	{LED_RED_PIN,	1, 0},
	{LED_GREEN_PIN,	1, 0},
	{LED_YELLOW_PIN,1, 0},
};

static TS_LED_INFO s_Led = {0};
static uint8_t s_LedDisplay[4] = {0};

int(*GET_DISPLAYDATA_CB)(uint8_t *Data,uint8_t *Len);
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/*
* 函数名称 : LED_Init
* 功能描述 : LED初始化
* 参     数 : NONE
* 返回值      : NONE
*/
/******************************************************************************/
void LED_Init(void)
/******************************************************************************/  
{    	 
	int i = 0;

	// 设置led pin为输出，并关闭led
	for(i = 0; i < LED_ID_MAX; i++)
	{
		#ifdef DISPLAY
		if(i == LED_DISPLAY)
		{
			if(TM1650_Init() != 0)
			{
				USR_LOG_INFO("TM1650_Init falled");
			}
			// GET_DISPLAYDATA_CB = GetDisplayData;
		}
		else
		#endif
		{
			IoTGpioInit(s_LedControl[i].pin);
    		IoTGpioSetDir(s_LedControl[i].pin, IOT_GPIO_DIR_OUT);
			IoTGpioSetOutputVal(s_LedControl[i].pin,s_LedControl[i].off);
			printf("Led id = %d, pin = %d, mode = %d",i,s_LedControl[i].pin,IOT_GPIO_DIR_OUT);
		}
	}
	LED_Clr();
	#ifdef DISPLAY
	LED_Set(LED_DISPLAY, LED_FLASH, 1000, 2000, 0);
	#endif
}

/*
* 函数名称 : LED_Drive
* 功能描述 : led驱动
* 参    数 : LedId - led ID
			 OnOff - 0关闭，1打开
* 返回值   : NONE
*/
/******************************************************************************/
static void LED_Drive(uint8_t LedId,uint8_t OnOff)
/******************************************************************************/  
{
	// uint8_t *data;
	uint8_t len;
	#ifdef DISPLAY
	if(LedId == LED_DISPLAY)
	{
		if(OnOff == 0)
		{
			memset(s_LedDisplay,0x0,sizeof(s_LedDisplay));
		}
		else
		{
			Get_DisplayData(s_LedDisplay,&len);
		}
		TM1650_Disp(s_LedDisplay,sizeof(s_LedDisplay));	
	}
	else
	#endif
	{
		if(OnOff == 0)
		{
			IoTGpioSetOutputVal(s_LedControl[LedId].pin,s_LedControl[LedId].off);	//普通LED灯显示
		}
		else
		{
			IoTGpioSetOutputVal(s_LedControl[LedId].pin,s_LedControl[LedId].on);	//普通LED灯显示
		}
		
	}
	
}

/*
* 函数名称 : LED_Clr
* 功能描述 : 熄灭全部LED
* 参     数 : NONE
* 返回值      : NONE
*/
/******************************************************************************/
void LED_Clr(void)
/******************************************************************************/  
{
	uint8_t i;
	for(i=0;i<LED_ID_MAX;i++)
	{
		s_Led.State[i]=LED_OFF;
		s_Led.Flash_Duty[i]=0;
		s_Led.Flash_Period[i]=0;
		s_Led.Flash_Times[i]=0;
		s_Led.Timer[i]=0;
		#ifdef DISPLAY
		if(i == LED_DISPLAY)
		{
			memset(s_LedDisplay,0x10,sizeof(s_LedDisplay));
		}
		#endif
	}
}

/*
* 函数名称 : LED_Set
* 功能描述 : LED工作方式设置
* 参     数 : LedId：ID / State：工作方式 / Duty:闪烁占空比 / Period:闪烁周期 / Times:闪烁次数
* 返回值      : NONE
*/
/******************************************************************************/
void LED_Set(uint8_t LedId,uint8_t State,uint16_t Duty,uint16_t Period,uint16_t Times)
/******************************************************************************/  
{
	s_Led.State[LedId]=State;
	s_Led.Flash_Duty[LedId]=Duty;
	s_Led.Flash_Period[LedId]=Period;
	s_Led.Flash_Times[LedId]=Times;
	s_Led.Timer[LedId]=0;
	// USR_LOG_INFO("LedId = %d, State = %d, Duty = %d, Period = %d",LedId,State,Duty,Period);
}


/*
* 函数名称 : LED_Service
* 功能描述 : LED驱动，需放在10ms时间片中
* 参     数 : NONE
* 返回值      : NONE
*/
/******************************************************************************/
void LED_Drv(void)
/******************************************************************************/  
{
	uint8_t i;

	for(i = 0;i < LED_ID_MAX;i++)
	{
		switch(s_Led.State[i])
		{
			case LED_ON:
				// LED_On(i);
				LED_Drive(i, 1);
			break;
			
			case LED_OFF:
				// LED_Off(i);
				LED_Drive(i, 0);
			break;

			case LED_FLASH:
				if(s_Led.Timer[i] < s_Led.Flash_Duty[i])
				{
					// LED_On(i);
					LED_Drive(i, 1);
				}
				else
				{
					// LED_Off(i);
					LED_Drive(i, 0);
				}
				
				if(s_Led.Timer[i] >= s_Led.Flash_Period[i])//LED.Timer[i]表示累加的时间
				{
					s_Led.Timer[i] = 0; 
					if(s_Led.Flash_Times[i] > 0)
					{
						s_Led.Flash_Times[i]--;
					}
					if(s_Led.Flash_Times[i] == 0)
					{
						s_Led.State[i] = LED_OFF;
					}
				}
			break;
			
			case LED_KEEP_FLASHING:
				if(s_Led.Timer[i] <= s_Led.Flash_Duty[i])
				{
					// LED_On(i);
					LED_Drive(i, 1);
				}
				else
				{
					// LED_Off(i);
					LED_Drive(i, 0);
				}
				if(s_Led.Timer[i] >= s_Led.Flash_Period[i])
				{
					s_Led.Timer[i] = 0;
				}
			break;
			
			default: 
				// LED_Off(i);
				LED_Drive(i, 0);
			break;
		}			
	}
    
    for(i=0;i<LED_ID_MAX;i++)
    {
        s_Led.Timer[i]+=10;
    }
}
/*
* 函数名称 : Led_Task
* 功能描述 : Led任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Led_Task(&argument);
*/
/******************************************************************************/ 
void Led_Task(void *argument)
/******************************************************************************/ 
{
    LED_Init();				//初始化IO

    while (1)
    {
        usleep(10*1000);	//10ms
        LED_Drv();					
    }
}
/*
* 函数名称 : Led_Demo
* 功能描述 : 创建Led任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Led_Demo(void);
*/
/******************************************************************************/ 
void Led_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "Led-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = LED_TASK_STACK_SIZE;
    attr.priority   = LED_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Led_Task, NULL, &attr) == NULL) {
        printf("\r\n[Led_Demo] Falied to create Led task!\n");
    }else{
        printf("\r\n[Led_Demo] Succ to create Led task!\n");
    }
}

APP_FEATURE_INIT(Led_Demo);
/******************************* End of File (C) ******************************/

