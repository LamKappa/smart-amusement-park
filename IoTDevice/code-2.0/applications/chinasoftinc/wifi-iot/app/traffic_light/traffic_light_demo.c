/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  traffic_light_demo.c
**  功能描述 :  交通灯示例
**  作    者 :  王滨泉
**  日    期 :  2021.12.4
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.12.4
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
#include "Beep.h"
#include "Key.h"
#include "Led.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define TRAFFIC_LIGHT_TASK_STACK_SIZE             1024*4                  //任务栈大小
#define TRAFFIC_LIGHT_TASK_PRIO                   10                      //任务优先等级


//------------------------------------------------------------------------------
//交通灯模式
typedef enum{
    TRAFFICLIGHT_ALL_OFF,               //全关
    TRAFFICLIGHT_REG_ON,                //亮红灯
    TRAFFICLIGHT_GREEN_ON,              //亮绿灯
    TRAFFICLIGHT_YELLOW_ON,             //亮黄灯
    TRAFFICLIGHT_MAX
}TE_TRAFFICLIGHT_MODE;

static uint8_t s_TrafficLight_Mode = TRAFFICLIGHT_ALL_OFF;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Set_Traffic_Light_Mode
* 功能描述 : 设置交通灯模式
* 参    数 : Mode - 交通灯模式
* 返回值   : 空
* 示    例 : Set_Traffic_Light_Mode(Mode);
*/
/******************************************************************************/ 
void Set_Traffic_Light_Mode(TE_TRAFFICLIGHT_MODE Mode)
/******************************************************************************/ 
{
    s_TrafficLight_Mode = Mode;
}
/*
* 函数名称 : Get_Traffic_Light_Mode
* 功能描述 : 获取交通灯模式
* 参    数 : 空
* 返回值   : 交通灯模式
* 示    例 : Mode = Get_Traffic_Light_Mode();
*/
/******************************************************************************/ 
uint8_t Get_Traffic_Light_Mode(void)
/******************************************************************************/ 
{
    return s_TrafficLight_Mode;
}

/*
* 函数名称 : Traffic_Light_Init
* 功能描述 : 交通灯初始化
* 参    数 : 空
* 返回值   : 空
* 示    例 : Traffic_Light_Init(&argument);
*/
/******************************************************************************/ 
void Traffic_Light_Init(void)
/******************************************************************************/ 
{
    Set_Traffic_Light_Mode(TRAFFICLIGHT_ALL_OFF);
}




/*
* 函数名称 : Key_Handle
* 功能描述 : 按键处理
* 参    数 : 空
* 返回值   : 空
* 示    例 : Key_Handle();
*/
/******************************************************************************/ 
void Key_Handle(void)
/******************************************************************************/ 
{
    TE_TRAFFICLIGHT_MODE Mode;
    if(IsKey(KEY_TRAFFIC,KEY_EVT_PRESSED) == true)
    {
        BUZZER_Set(BUZZER_ALARM,200,400,1);					//蜂鸣器短鸣
        printf("\r\nKEY_EVT_PRESSED\n");
        Mode = Get_Traffic_Light_Mode();
        if(++Mode >= TRAFFICLIGHT_MAX)
        {
            Mode = TRAFFICLIGHT_REG_ON;
        }
        Set_Traffic_Light_Mode(Mode);
    }
}
/*
* 函数名称 : Traffic_Light_Task
* 功能描述 : 交通灯任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Traffic_Light_Task(&argument);
*/
/******************************************************************************/ 
void Traffic_Light_Task(void *argument)
/******************************************************************************/ 
{
    uint8_t i = 0;
    Traffic_Light_Init();
    LED_Set(LED_RED, LED_FLASH, 200, 900, 4);
    usleep(300*1000);
    LED_Set(LED_GREEN, LED_FLASH, 200, 900, 4);
    usleep(300*1000);
    LED_Set(LED_YELLOW, LED_FLASH, 200, 900, 4);
    sleep(4);
    while(1)
    {
        usleep(10*1000);	//10ms
		Key_Handle();

        switch(Get_Traffic_Light_Mode())
        {
            case TRAFFICLIGHT_ALL_OFF:
                LED_Set(LED_RED, LED_OFF, 0, 0, 0);
                LED_Set(LED_GREEN, LED_OFF, 0, 0, 0);
                LED_Set(LED_YELLOW, LED_OFF, 0, 0, 0);
            break;

            case TRAFFICLIGHT_REG_ON:
                LED_Set(LED_RED, LED_ON, 0, 0, 0);
                LED_Set(LED_GREEN, LED_OFF, 0, 0, 0);
                LED_Set(LED_YELLOW, LED_OFF, 0, 0, 0);
            break;

            case TRAFFICLIGHT_GREEN_ON:
                LED_Set(LED_RED, LED_OFF, 0, 0, 0);
                LED_Set(LED_GREEN, LED_ON, 0, 0, 0);
                LED_Set(LED_YELLOW, LED_OFF, 0, 0, 0);
            break;

            case TRAFFICLIGHT_YELLOW_ON:
                LED_Set(LED_RED, LED_OFF, 0, 0, 0);
                LED_Set(LED_GREEN, LED_OFF, 0, 0, 0);
                LED_Set(LED_YELLOW, LED_ON, 0, 0, 0);
            break;
        }
		
    }
}
/*
* 函数名称 : Traffic_Light_Demo
* 功能描述 : 创建交通灯任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Traffic_Light_Demo(void);
*/
/******************************************************************************/ 
void Traffic_Light_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "Traffic-Light-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = TRAFFIC_LIGHT_TASK_STACK_SIZE;
    attr.priority   = TRAFFIC_LIGHT_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Traffic_Light_Task, NULL, &attr) == NULL) {
        printf("\r\n[Traffic_Light_Demo] Falied to create traffic light task!\n");
    }else{
        printf("\r\n[Traffic_Light_Demo] Succ to create traffic light task!\n");
    }
}

APP_FEATURE_INIT(Traffic_Light_Demo);
/******************************* End of File (C) ******************************/