/******************************************************************************\
**  版     权：深圳开鸿数字产业发展有限公司（2021）
**  文件名称：key.c
**  功能描述：按键模块化程序，支持短按，长按，连发，组合等按键功能。
**  作     者：王滨泉
**  日     期：2018-01-01
**  版     本：V0.0.1
**  变更记录：1.首次创建
\******************************************************************************/

/******************************************************************************\
                             Includes
\******************************************************************************/
#include "Key.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define KEY_TASK_STACK_SIZE      1024                   //任务栈大小
#define KEY_TASK_PRIO            11                     //任务优先等级



TS_KEY_INFO KeyInfo = {0};

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Key_Init
* 功能描述 : 按键初始化函数
* 参     数 : NONE
* 返回值      : NONE
*/
/******************************************************************************/
void Key_Init(void)
/******************************************************************************/  
{
	IoTGpioInit(KEY0_OLED_PIN);
    IoTGpioSetDir(KEY0_OLED_PIN, KEY0_OLED_MODE);

	IoTGpioInit(KEY1_TRAFFIC_PIN);
    IoTGpioSetDir(KEY1_TRAFFIC_PIN, KEY1_TRAFFIC_MODE);
	hi_io_set_pull(KEY1_TRAFFIC_PIN,HI_IO_PULL_UP);
}

/*
* 函数名称 : GetKey
* 功能描述 : 获取键值,返回值每一位代表一个按键按下，支持组合按键
* 参     数 : NONE
* 返回值      : 返回按键值
*/
/******************************************************************************/
uint32_t GetKey(void)
/******************************************************************************/  
{
	uint16_t key = KEY_NONE;
	IotGpioValue val;

	key = KEY_NONE;
	IoTGpioGetInputVal(KEY0_OLED_PIN,&val);				//获取按键状态
	key |= (val == KEY0_LED_DOWN_LEVEL)?1:0;			//得到按键值
	IoTGpioGetInputVal(KEY1_TRAFFIC_PIN,&val);			//获取按键状态
	key |= ((val == KEY1_TRAFFIC_DOWN_LEVEL)?1:0)<<1;	//得到按键值
	return key;
} 

/*
* 函数名称 : KeyModuleProcess
* 功能描述 : 按键检测流程
* 参     数 : 按键结构体指针
* 返回值      : 返回TRUE表示有触发(按下或抬起)，FALSE表示无触发
*/
/******************************************************************************/
static uint8_t KeyModuleProcess(TS_KEY_INFO* pKeyInfo) 
/******************************************************************************/  
{ 
    static uint32_t KeyLast = 0;
	uint32_t KeyCur = 0;
	

	KeyCur = GetKey();
	
	switch(pKeyInfo->KeyState) 
    { 
        case KEY_STATE_INIT: 
			KeyLast = 0;
			pKeyInfo->KeyDwTmr = 0; 
			KeyInfo.KeyEvent = KEY_EVT_NULL;
			if(KeyCur != KEY_NONE)    
			{
				pKeyInfo->KeyState = KEY_STATE_WOBBLE; 
			}				     
        break; 
        
        case KEY_STATE_WOBBLE://消抖
			pKeyInfo->KeyState = KEY_STATE_DWON;   
        break; 
        
        case KEY_STATE_DWON:
			pKeyInfo->KeyDwTmr++;
			if(KeyCur == KeyLast) 
			{  
				if(pKeyInfo->KeyDwTmr >= KEY_DOWM_TMR) //长按
				{
					pKeyInfo->KeyCur = KeyLast;
					pKeyInfo->KeyEvent |= KEY_EVT_DOWN;
					pKeyInfo->KeyState = KEY_STATE_WAITUP;
				}
			} 
			else 
			{ 
				if(KeyCur == KEY_NONE) 
				{
					if(pKeyInfo->KeyDwTmr >= KEY_PRESSED_TMR) //点按
					{
						pKeyInfo->KeyCur = KeyLast;
						pKeyInfo->KeyEvent |= KEY_EVT_PRESSED;
						pKeyInfo->KeyState = KEY_STATE_INIT;
					}
				}
				else
				{
					pKeyInfo->KeyState = KEY_STATE_INIT;
				}
			} 
        break;       	
        
        case KEY_STATE_WAITUP: 
			if(KeyCur == KeyLast) 
			{ 
				if(++pKeyInfo->KeyDwTmr >= KEY_REPEAT_TMR) //连发
				{
					pKeyInfo->KeyCur = KeyLast;
					pKeyInfo->KeyEvent |= KEY_EVT_REPEAT;
					pKeyInfo->KeyDwTmr = 0;
				}
			} 
			else
			{
				if(KeyCur == KEY_NONE)  
				{
					pKeyInfo->KeyCur = KeyLast;
					pKeyInfo->KeyEvent |= KEY_EVT_UP;
					pKeyInfo->KeyState = KEY_STATE_INIT; 
				}
			}
        break;
        
        default: 
        break; 
    } 
    
    if(KeyLast != KeyCur)
    {
        KeyLast = KeyCur;               //保存上次的按键值
    }

	
    return(false); 

	
}  

/*
* 函数名称 : KeyDrv
* 功能描述 : 按键驱动函数,需放10ms的循环时间片中
* 参     数 : NONE
* 返回值      : NONE
*/
/******************************************************************************/
void KEY_Drv(void)
/******************************************************************************/  
{	 	
    KeyModuleProcess(&KeyInfo);	
}

/*
* 函数名称 : IsKey
* 功能描述 : 按键事件判断API
* 参     数 : KeyId:按键ID / KeyEvent:按键事件类型
* 返回值      : true:有KeyEvent事件发生 / fasle:无
*/
/******************************************************************************/
bool IsKey(uint32_t KeyId, TE_KEY_EVENT KeyEvent)
/******************************************************************************/  
{
	if(KeyId == KEY_NONE)
	{
		if(KeyInfo.KeyEvent == KEY_EVT_NULL)
		{
			return true;
		}
	}

	if(KeyInfo.KeyCur == KeyId)
	{
		if(KeyInfo.KeyEvent & KeyEvent)
		{
			KeyInfo.KeyEvent = KEY_EVT_NULL;
			return true;
		}
	}

	return false;
}
/*
* 函数名称 : Key_Task
* 功能描述 : 按键任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Key_Task(&argument);
*/
/******************************************************************************/ 
void Key_Task(void *argument)
/******************************************************************************/ 
{
    Key_Init();				//初始化IO

    while (1)
    {
        usleep(10*1000);	//10ms
        KEY_Drv();					
    }
}
/*
* 函数名称 : Key_Demo
* 功能描述 : 创建按键任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Key_Demo(void);
*/
/******************************************************************************/ 
void Key_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "Key-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = KEY_TASK_STACK_SIZE;
    attr.priority   = KEY_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Key_Task, NULL, &attr) == NULL) {
        printf("\r\n[Key_Demo] Falied to create Key task!\n");
    }else{
        printf("\r\n[Key_Demo] Succ to create Key task!\n");
    }
}

APP_FEATURE_INIT(Key_Demo);
/******************************* End of File (C) ******************************/

