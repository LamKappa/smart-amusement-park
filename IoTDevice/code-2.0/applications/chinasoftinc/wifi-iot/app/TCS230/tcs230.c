/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  tcs230.c
**  功能描述 :  色彩感应传感器驱动
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
#include <stdbool.h>
#include <unistd.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "hi_systick.h"
#include <hi_hrtimer.h>


/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define TCS230_TASK_STACK_SIZE      (1024)                      //任务栈大小
#define TCS230_TASK_PRIO            (10)                        //任务优先等级
#define TIMER_ISR_TIMEOUT           (1000*10)                   //10MS定时中断    


#ifndef MIN
    #define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif

//引脚定义
#define TCS230_S0                   (12)
#define TCS230_S1                   (11)
#define TCS230_S2                   (8)
#define TCS230_S3                   (6)
#define TCS230_LED                  (10)
#define TCS230_OUT                  (5)
#define TCS230_BEEP                 (9)
//白平衡计数
#define CHANGE_NUM                  (255)
//------------------------------------------------------------------------------
// 颜色数值句柄
typedef struct 
{
    uint32 Red;
    uint32 Green;
    uint32 Blue;

}TS_COLOUR_INFO;
//------------------------------------------------------------------------------
// 检测状态机
typedef enum{
    COLOUR_STATE_INIT,              //初始化
    COLOUR_STATE_RED,               //红光滤波  
    COLOUR_STATE_GREEN,             //绿光滤波
    COLOUR_STATE_BLUE,              //蓝光滤波
    COLOUR_STATE_FINISH,            //结束
    COLOUR_STATE_IDLE,              //空闲
}TE_COLOUR_STATE;


bool detection_flag = false;        //检测标志
bool whitebalance_flag = false;     //白平衡标志
uint32 balance_param[6]={0};
uint32 balance_RGB[6] = {0};
uint16 pulse_cnt=0;
hi_u32 g_timer_handle;//定时器句柄
TS_COLOUR_INFO s_Colour_data = {0};
static TE_COLOUR_STATE s_ColourState = COLOUR_STATE_INIT;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Get_Red_Data
* 功能描述 : 获取红色值
* 参    数 : 空
* 返回值   : 红色值
* 示    例 : result = Get_Red_Data();
*/
/******************************************************************************/ 
int Get_Red_Data(void)
/******************************************************************************/ 
{
    return s_Colour_data.Red;
}
/*
* 函数名称 : Get_Green_Data
* 功能描述 : 获取绿色值
* 参    数 : 空
* 返回值   : 绿色值
* 示    例 : result = Get_Green_Data();
*/
/******************************************************************************/ 
int Get_Green_Data(void)
/******************************************************************************/
{
    return s_Colour_data.Green;
}
/*
* 函数名称 : Get_Blue_Data
* 功能描述 : 获取蓝色值
* 参    数 : 空
* 返回值   : 蓝色值
* 示    例 : result = Get_Blue_Data();
*/
/******************************************************************************/ 
int Get_Blue_Data(void)
/******************************************************************************/ 
{
    return s_Colour_data.Blue;
}
/*
* 函数名称 : Set_Colour_State
* 功能描述 : 设置状态机状态
* 参    数 : state - 状态机状态
* 返回值   : 空
* 示    例 : state = Set_Colour_State();
*/
/******************************************************************************/ 
void Set_Colour_State(TE_COLOUR_STATE State)
/******************************************************************************/ 
{
    s_ColourState = State;
}
/*
* 函数名称 : Get_Colour_State
* 功能描述 : 获取状态机状态
* 参    数 : 空
* 返回值   : 状态机状态
* 示    例 : state = Get_Colour_State();
*/
/******************************************************************************/ 
TE_COLOUR_STATE Get_Colour_State(void)
/******************************************************************************/ 
{
    return s_ColourState;
} 
/*
* 函数名称 : out_isr_func
* 功能描述 : 中断记录脉冲数
* 参    数 : 空
* 返回值   : 空
* 示    例 : out_isr_func();
*/
/******************************************************************************/ 
void out_isr_func(void)
/******************************************************************************/ 
{
    if(detection_flag == true)
    {
        pulse_cnt++;                //计算脉冲个数 
    }
}
/*
* 函数名称 : timer_isr_func
* 功能描述 : 通过定时器计算获取RGB值
* 参    数 : 空
* 返回值   : 空
* 示    例 : timer_isr_func();
*/
/******************************************************************************/ 
void timer_isr_func(void)
/******************************************************************************/ 
{
    hi_hrtimer_stop(g_timer_handle);

    balance_RGB[Get_Colour_State()] = pulse_cnt;
    printf("\r\n balance_RGB[%d] = %d \r\n", Get_Colour_State(), pulse_cnt);
    pulse_cnt = 0;
    

    switch(Get_Colour_State())
    {
        case COLOUR_STATE_INIT:
            //开启电源
            IoTGpioSetOutputVal(TCS230_S0,1);
            IoTGpioSetOutputVal(TCS230_S1,0);
            //开启补光灯
            IoTGpioSetOutputVal(TCS230_LED,1);
            Set_Colour_State(COLOUR_STATE_RED);
        break;
        //红光滤波
        case COLOUR_STATE_RED:      
            detection_flag = true;
            IoTGpioSetOutputVal(TCS230_S2,0);
            IoTGpioSetOutputVal(TCS230_S3,0);
            Set_Colour_State(COLOUR_STATE_GREEN);
        break;
        //绿光滤波
        case COLOUR_STATE_GREEN:
            IoTGpioSetOutputVal(TCS230_S2,1);
            IoTGpioSetOutputVal(TCS230_S3,1);
            Set_Colour_State(COLOUR_STATE_BLUE);
        break;
        //蓝光滤波
        case COLOUR_STATE_BLUE:
            IoTGpioSetOutputVal(TCS230_S2,0);
            IoTGpioSetOutputVal(TCS230_S3,1);
            Set_Colour_State(COLOUR_STATE_FINISH);
        break;
        
        case COLOUR_STATE_FINISH:
            detection_flag = false;
            //关闭电源
            IoTGpioSetOutputVal(TCS230_S0,0);
            IoTGpioSetOutputVal(TCS230_S1,0);
            //关闭补光灯
            IoTGpioSetOutputVal(TCS230_LED,0);
            Set_Colour_State(COLOUR_STATE_IDLE);
        break;

        case COLOUR_STATE_IDLE:
            Set_Colour_State(COLOUR_STATE_IDLE);
        break;


    }
    hi_hrtimer_start(g_timer_handle, TIMER_ISR_TIMEOUT, timer_isr_func, 0);
}

/*
* 函数名称 : tcs230_init
* 功能描述 : tcs230初始化
* 参    数 : 空
* 返回值   : 空
* 示    例 : tcs230_init();
*/
/******************************************************************************/ 
void tcs230_init(void)
/******************************************************************************/ 
{
    //配置引脚初始化
    IoTGpioInit(TCS230_S0);
    IoTGpioSetDir(TCS230_S0, IOT_GPIO_DIR_OUT);
    IoTGpioInit(TCS230_S1);
    IoTGpioSetDir(TCS230_S1, IOT_GPIO_DIR_OUT);
    IoTGpioInit(TCS230_S2);
    IoTGpioSetDir(TCS230_S2, IOT_GPIO_DIR_OUT);
    IoTGpioInit(TCS230_S3);
    IoTGpioSetDir(TCS230_S3, IOT_GPIO_DIR_OUT);
    //关断电源
    IoTGpioSetOutputVal(TCS230_S0,0);
    IoTGpioSetOutputVal(TCS230_S1,0);
    //默认过滤无
    IoTGpioSetOutputVal(TCS230_S2,1);
    IoTGpioSetOutputVal(TCS230_S3,0);
    //LED引脚初始化，开启补光灯
    IoTGpioInit(TCS230_LED);
    IoTGpioSetDir(TCS230_LED, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(TCS230_LED,0);

    //OUT引脚初始化
    IoTGpioInit(TCS230_OUT);
    IoTGpioSetDir(TCS230_OUT, IOT_GPIO_DIR_IN);
    //out引脚中断
    IoTGpioRegisterIsrFunc(TCS230_OUT,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_RISE_LEVEL_HIGH,out_isr_func,NULL);
    Set_Colour_State(COLOUR_STATE_INIT);
    //定时器中断
    hi_hrtimer_create(&g_timer_handle);
    // hi_hrtimer_start(g_timer_handle, TIMER_ISR_TIMEOUT, timer_isr_func, 0);

    printf("\r\n tcs230_init \r\n");
}

/*
* 函数名称 : TCS230Task
* 功能描述 : TCS230任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : TCS230Task(&argument);
*/
/******************************************************************************/ 
void TCS230Task(void *arg)
/******************************************************************************/ 
{
    static uint8_t whitebalance_num = 0;
    uint32_t temp;
    tcs230_init();
    sleep(1);
    hi_hrtimer_start(g_timer_handle, TIMER_ISR_TIMEOUT, timer_isr_func, 0);
    while(1)
    {
        usleep(1000);
        if(detection_flag == false && Get_Colour_State() >= COLOUR_STATE_FINISH)          
        {
            hi_hrtimer_stop(g_timer_handle);
            printf("\r\nhi_hrtimer_stop\n");
            if(whitebalance_flag == false)                                              //第一次采集值记录为白平衡值
            {
                //过滤掉前4次采集值
                if(++whitebalance_num > 4)
                {
                    balance_param[0] += balance_RGB[0];
                    balance_param[1] += balance_RGB[1];
                    balance_param[2] += balance_RGB[2];
                    balance_param[3] += balance_RGB[3];
                    balance_param[4] += balance_RGB[4];
                }
                //后6次值做平均
                if(whitebalance_num >= 10)
                {
                    whitebalance_flag = true;
                    whitebalance_num = 0;
                    balance_param[0] = (balance_param[0]/6);
                    balance_param[1] = (balance_param[1]/6);
                    balance_param[2] = (balance_param[2]/6);
                    balance_param[3] = (balance_param[3]/6);
                    balance_param[4] = (balance_param[4]/6);
                }
                
                printf("\r\nRed = %d \nGreen = %d \nBlue = %d \n",balance_param[2] ,balance_param[3] ,balance_param[4]);
            }
            else
            {
                temp = balance_RGB[2]*255/balance_param[2];    //计算红色值
                if(temp > 255)
                {
                    s_Colour_data.Red = 255;
                }
                else
                {
                    s_Colour_data.Red = temp;
                }

                temp = balance_RGB[3]*255/balance_param[3];  //计算绿色值
                if(temp > 255)
                {
                    s_Colour_data.Green = 255;
                }
                else
                {
                    s_Colour_data.Green = temp;
                }

                temp = balance_RGB[4]*255/balance_param[4];   //计算蓝色值
                if(temp > 255)
                {
                    s_Colour_data.Blue = 255;
                }
                else
                {
                    s_Colour_data.Blue = temp;
                }
                printf("\r\nRed = %d \nGreen = %d \nBlue = %d \n",s_Colour_data.Red ,s_Colour_data.Green ,s_Colour_data.Blue);
                sleep(1);
            }
            //重新开始计量
            Set_Colour_State(COLOUR_STATE_INIT);                              
            hi_hrtimer_start(g_timer_handle, TIMER_ISR_TIMEOUT, timer_isr_func, 0);
            printf("\r\nhi_hrtimer_start\n");
        }
    }
}
/*
* 函数名称 : TCS230Demo
* 功能描述 : TCS230示例配置
* 参    数 : 空
* 返回值   : 空
* 示    例 : TCS230Demo();
*/
/******************************************************************************/ 
void TCS230Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "TCS230Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = TCS230_TASK_STACK_SIZE;
    attr.priority   = TCS230_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)TCS230Task, NULL, &attr) == NULL) {
        printf("\r\n[TCS230Demo] Falied to create TCS230Task!\n");
    }else{
        printf("\r\n[TCS230Demo] Succ to create TCS230Task!\n");
    }
}

APP_FEATURE_INIT(TCS230Demo);
/******************************* End of File (C) ******************************/