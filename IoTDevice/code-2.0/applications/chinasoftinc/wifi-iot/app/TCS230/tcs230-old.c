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
#ifndef MIN
    #define MIN(a, b)   ((a) < (b) ? (a) : (b))
#endif

//引脚定义
#define TCS230_S0       12
#define TCS230_S1       11
#define TCS230_S2       8
#define TCS230_S3       6
#define TCS230_LED      10
#define TCS230_OUT      5
#define TCS230_BEEP     9
//白平衡计数
#define CHANGE_NUM      255
//------------------------------------------------------------------------------
// 颜色数值句柄
typedef struct 
{
    uint32 Red;
    uint32 Green;
    uint32 Blue;
    uint32 white;
}TS_COLOUR_INFO;

uint32 raw_RGB[4]={0};
uint32 balance_param[4]={1000,1000,1000,1000};

uint32 balance_RGB[4] = {0};
uint8_t num = 0;
uint8  balance_flag=0;
uint16 count=0;
uint16 step=0;
hi_u32 g_timer_handle;//定时器句柄
uint32_t RGB[4] = {0};
TS_COLOUR_INFO s_Colour_data = {0};
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
    
    static uint32 balance_RGB_buff[4]={0};

    if (balance_flag==1)
    {
        balance_RGB[step]=count;
        count=0;
        step++;
        if (step==0)
        {
            IoTGpioSetOutputVal(TCS230_S2,0);
            IoTGpioSetOutputVal(TCS230_S3,0);
        }else if (step==1)
        {
            IoTGpioSetOutputVal(TCS230_S2,1);
            IoTGpioSetOutputVal(TCS230_S3,1);
        }else if (step==2)
        {
            IoTGpioSetOutputVal(TCS230_S2,0);
            IoTGpioSetOutputVal(TCS230_S3,1);
            
        }else if (step==3)
        {
            IoTGpioSetOutputVal(TCS230_S2,1);
            IoTGpioSetOutputVal(TCS230_S3,0);
            
        }
        if (step>3)
        {
            step=0;

            balance_RGB_buff[0] += balance_RGB[0];
            balance_RGB_buff[1] += balance_RGB[1];
            balance_RGB_buff[2] += balance_RGB[2];
            balance_RGB_buff[3] += balance_RGB[3];
            
            if(++num >= 32)
            {
                s_Colour_data.Red = balance_RGB_buff[0] >> 5;
                s_Colour_data.Green = balance_RGB_buff[1] >> 5;
                s_Colour_data.Blue = balance_RGB_buff[2] >> 5;
                s_Colour_data.white = balance_RGB_buff[3] >> 5;

                memset(balance_RGB_buff, 0, sizeof(balance_RGB_buff));

                num = 0;
                printf("balance:R = %d, G = %d, B = %d\n",s_Colour_data.Red,s_Colour_data.Green,s_Colour_data.Blue);
            } 
        }
        // printf("step1=%d\n",step);
        hi_hrtimer_start(g_timer_handle, balance_param[step]*1000, timer_isr_func, 0);
    }
    
}
/*
* 函数名称 : out_isr_func
* 功能描述 : 通过定时器计算获取白平衡值
* 参    数 : 空
* 返回值   : 空
* 示    例 : out_isr_func();
*/
/******************************************************************************/ 
void out_isr_func(void)
/******************************************************************************/ 
{
    static uint64 start_time,stop_time;
    static uint8 start_flag=0;
    static uint8 white_balance_times=0;
    if (start_flag==0)
    {
        start_time=hi_systick_get_cur_ms();
        start_flag=1;
    }
    count++; 
    if (count>CHANGE_NUM && balance_flag==0)
    {
        stop_time=hi_systick_get_cur_ms();
        raw_RGB[step]=stop_time-start_time;
        step++;
        if (step==0)
        {
            IoTGpioSetOutputVal(TCS230_S2,0);
            IoTGpioSetOutputVal(TCS230_S3,0);
        }else if (step==1)
        {
            IoTGpioSetOutputVal(TCS230_S2,1);
            IoTGpioSetOutputVal(TCS230_S3,1);
        }else if (step==2)
        {
            IoTGpioSetOutputVal(TCS230_S2,0);
            IoTGpioSetOutputVal(TCS230_S3,1);
            
        }else if (step==3)
        {
            IoTGpioSetOutputVal(TCS230_S2,1);
            IoTGpioSetOutputVal(TCS230_S3,0);
            
        }
        else
        {
            // printf("raw:%d,%d,%d,%d\n",raw_RGB[0],raw_RGB[1],raw_RGB[2],raw_RGB[3]);
            step=0;
            if (white_balance_times<10)
            {
                white_balance_times++;
                balance_param[0]=MIN(raw_RGB[0],balance_param[0]);
                balance_param[1]=MIN(raw_RGB[1],balance_param[1]);
                balance_param[2]=MIN(raw_RGB[2],balance_param[2]);
                balance_param[3]=MIN(raw_RGB[3],balance_param[3]);
                printf("balance_param:%d,%d,%d,%d\n",balance_param[0],balance_param[1],balance_param[2],balance_param[3]);
            }else{
                balance_flag=1;
                hi_hrtimer_create(&g_timer_handle);
                hi_hrtimer_start(g_timer_handle, balance_param[0]*1000, timer_isr_func, 0);
            }
        }
        printf("step:%d\n",step);
        count=0;
        start_flag=0;
    }
    
    
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
    //100%频率输出
    IoTGpioSetOutputVal(TCS230_S0,1);
    IoTGpioSetOutputVal(TCS230_S1,1);
    //默认过滤红光
    IoTGpioSetOutputVal(TCS230_S2,1);
    IoTGpioSetOutputVal(TCS230_S3,1);
    //LED引脚初始化，开启补光灯
    IoTGpioInit(TCS230_LED);
    IoTGpioSetDir(TCS230_LED, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(TCS230_LED,1);

    //OUT引脚初始化
    IoTGpioInit(TCS230_OUT);
    IoTGpioSetDir(TCS230_OUT, IOT_GPIO_DIR_IN);
    //out引脚中断
    IoTGpioRegisterIsrFunc(TCS230_OUT,IOT_INT_TYPE_EDGE,IOT_GPIO_EDGE_RISE_LEVEL_HIGH,out_isr_func,NULL);
    printf("\r\n tcs230_init \r\n");
}

APP_FEATURE_INIT(tcs230_init);
/******************************* End of File (C) ******************************/