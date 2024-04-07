/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  Soil.c
**  功能描述 :  土壤湿度控制
**  作    者 :  王滨泉
**  日    期 :  2021.09.27
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.09.27
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
#include "iot_gpio.h"
#include "hi_adc.h"

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define SOILHUMICTRL_TASK_STACK_SIZE        1024*4                  //任务栈大小
#define SOILHUMICTRL_TASK_PRIO              10                      //任务优先等级

#define SOIL_ADC_CHANNEL                    HI_ADC_CHANNEL_4        //土壤湿度adc采集通道
#define WATER_PUMP_PIN                      5                       //GPIO10控制电磁阀开关
#define WATER_PUMP_PIN_MODE                 IOT_GPIO_DIR_OUT        //GPIO10输出状态
#define WATER_PUMP_POWER_PIN                10                      //GPIO5控制电磁阀电源
#define WATER_PUMP_POWER_PIN_MODE           IOT_GPIO_DIR_OUT        //GPIO5输出状态
#define WATER_PUMP_ON                       1                       //水泵开
#define WATER_PUMP_OFF                      0                       //水泵关
#define WATER_PUMP_POWER_ON                 1                       //水泵开
#define WATER_PUMP_POWER_OFF                0                       //水泵关


//------------------------------------------------------------------------------
// IO配置句柄
typedef struct
{
    uint8_t         Num;                    //GPIO 编号
    IotGpioDir      Mode;                   //工作模式
    IotGpioValue    Value;                  //GPIO值
}TS_IO_PARAM;
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
    uint8_t Humidity;                   //湿度值
    uint8_t HumiThreshold;              //水泵开启对应湿度阈值   
}TS_SOIL_PARAM;

static TS_SOIL_PARAM s_Soil = {0};
static TS_ADC_PARAM  s_Soil_Adc = {0};
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Set_Water_Pump_Onoff_Threshold
* 功能描述 : 设置水阀开发湿度阈值
* 参    数 : HumiThreshold - 水泵开启对应湿度阈值 
* 返回值   : 0 - 设置成功
            -1 - 设置失败
* 示    例 : result = Set_Water_Pump_Onoff_Threshold(HumiThreshold);
*/
/******************************************************************************/ 
char Set_Water_Pump_Onoff_Threshold(uint8_t HumiThreshold)
/******************************************************************************/
{
    s_Soil.HumiThreshold = HumiThreshold;
    return 0;
}

/*
* 函数名称 : Get_Water_Pump_Onoff_Threshold
* 功能描述 : 获取水阀开发湿度阈值
* 参    数 : 空
* 返回值   : 湿度阈值
* 示    例 : HumiThreshold = Set_Water_Pump_Onoff_Threshold();
*/
/******************************************************************************/ 
uint8_t Get_Water_Pump_Onoff_Threshold(void)
/******************************************************************************/
{
    
    return s_Soil.HumiThreshold;                //水泵开启对应湿度阈值   
}

/*
* 函数名称 : Get_Soil_Humi
* 功能描述 : 获取土壤湿度
* 参    数 : 空
* 返回值   : 土壤湿度
* 示    例 : Humidity = Get_Soil_Humi();
*/
/******************************************************************************/ 
uint8_t Get_Soil_Humi(void)
/******************************************************************************/
{
    
    return s_Soil.Humidity;                     //传递湿度值
}



/*
* 函数名称 : Measure_Soil_Humi
* 功能描述 : 测量土壤湿度
* 参    数 : Humi - 数据指针
* 返回值   : 0 - 测量成功
            -1 - 测量失败
* 示    例 : result = Measure_Soil_Humi(&Humi);
*/
/******************************************************************************/ 
static char Measure_Soil_Humi(uint8_t *Humi)
/******************************************************************************/
{
    char ret = 0;
    uint16_t data;
    float voltage = 0;
    if(hi_adc_read(s_Soil_Adc.Channel,&data,s_Soil_Adc.EquMode,s_Soil_Adc.CurBais,s_Soil_Adc.DelayCnt) == HI_ERR_SUCCESS)
    {
        voltage = hi_adc_convert_to_voltage(data);  //ad值转换成电压值
        if(voltage > 3.3)
        {
            voltage = 3.3;
        }
        printf("voltage = %.2f\n",voltage);
        *Humi = (100-(uint8_t)((voltage/3.3)*100)); //电压值转成为湿度值
        ret = 0;                                    //测量成功
    }
    else
    {
        printf("Measure soil humidity fales!!!\n");
        ret = -1;                                   //测量失败                                   
    }
    return ret;
}

/*
* 函数名称 : Gpio_Init
* 功能描述 : gpio工作模式配置
* 参    数 : Param - IO配置句柄指针
* 返回值   : 0 - 初始化成功
            -1 - 初始化失败
* 示    例 : result = Gpio_Init(&Param);
*/
/******************************************************************************/ 
char Gpio_Init(TS_IO_PARAM *Param)
/******************************************************************************/
{
    if(Param == NULL)
    {
        printf("Gpio init fales!!!\n");
        return -1;
    }
    IoTGpioInit(Param->Num);                                //初始化GPIO
    IoTGpioSetDir(Param->Num, Param->Mode);                 //设置GPIO工作状态
    if(Param->Mode == IOT_GPIO_DIR_OUT)
    {
        IoTGpioSetOutputVal(Param->Num, Param->Value);      //输出状态，设置输出电平
    }
    else
    {
        IoTGpioGetInputVal(Param->Num, &Param->Value);       //输入状态，读取电平
    }
    return 0;
}
/*
* 函数名称 : Soil_Init
* 功能描述 : 土壤湿度检测初始化
* 参    数 : 空
* 返回值   : 0 - 初始化成功
            -1 - 初始化失败
* 示    例 : result = Soil_Init();
*/
/******************************************************************************/ 
static char Soil_Init(void)
/******************************************************************************/
{
    TS_IO_PARAM param;
    uint8_t ret;
    //GPIO5 设置为输出
    param.Num  = WATER_PUMP_POWER_PIN;
    param.Mode = WATER_PUMP_POWER_PIN_MODE;
    param.Value = WATER_PUMP_POWER_OFF;
    if(Gpio_Init(&param) != 0)
    {
        return -1;
    }
    //GPIO10 设置为输出
    param.Num  = WATER_PUMP_PIN;                
    param.Mode = WATER_PUMP_PIN_MODE;
    param.Value = WATER_PUMP_OFF;
    if(Gpio_Init(&param) != 0)
    {
        return -1;
    }
    // //GPIO9 设置为输入
    // param.Num  = 9;                
    // param.Mode = IOT_GPIO_DIR_IN;
    // if(Gpio_Init(&param) != 0)
    // {
    //     return -1;
    // }

    //ADC 配置
    s_Soil_Adc.Channel  = SOIL_ADC_CHANNEL;
    s_Soil_Adc.EquMode  = HI_ADC_EQU_MODEL_8;
    s_Soil_Adc.CurBais  = HI_ADC_CUR_BAIS_DEFAULT;
    s_Soil_Adc.DelayCnt = 0;
    return 0;
}
/*
* 函数名称 : Water_Pump_OnOff_Set
* 功能描述 : 水泵开关设置
* 参    数 : State - 开关状态：0 - 关闭，1 - 打开
* 返回值   : 空
* 示    例 : Water_Pump_OnOff_Set(State);
*/
/******************************************************************************/ 
static void Water_Pump_OnOff_Set(uint8_t State)
/******************************************************************************/
{
    IoTGpioSetOutputVal(WATER_PUMP_PIN, State);
}
/*
* 函数名称 : Water_Pump_Power_OnOff_Set
* 功能描述 : 水泵电源开关设置
* 参    数 : State - 开关状态：0 - 关闭，1 - 打开
* 返回值   : 空
* 示    例 : Water_Pump_Power_OnOff_Set(State);
*/
/******************************************************************************/ 
static void Water_Pump_Power_OnOff_Set(uint8_t State)
/******************************************************************************/
{
    IoTGpioSetOutputVal(WATER_PUMP_POWER_PIN, State);
}
/*
* 函数名称 : SoilHumiCtrl_Task
* 功能描述 : 土壤湿度控制任务
* 参    数 : argument - 任务参数
* 返回值   : 空
* 示    例 : SoilHumiCtrl_Task(&argument);
*/
/******************************************************************************/ 
void SoilHumiCtrl_Task(void *argument)
/******************************************************************************/  
{
    uint8_t pump_ctrl_state, pump_power_state;
    

    Soil_Init();                                            //硬件初始化
    s_Soil.HumiThreshold = 50;                              //默认阈值50

    while (1)
    {
        sleep(1);
        
        if(Measure_Soil_Humi(&s_Soil.Humidity) != 0)        //检测土壤湿度
        {
            printf("[SoilHumiCtrl_Task] Soil humi measure false!!!\n");
        }
        
        if(s_Soil.Humidity >= s_Soil.HumiThreshold)         //判断当前湿度是否大于设置水泵开关阈值
        {
            pump_ctrl_state  = WATER_PUMP_OFF;
            pump_power_state = WATER_PUMP_POWER_OFF;
        }
        else
        {
            pump_ctrl_state  = WATER_PUMP_ON;
            pump_power_state = WATER_PUMP_POWER_ON;
        }
        Water_Pump_Power_OnOff_Set(pump_power_state);       //设置水泵电源开关
        Water_Pump_OnOff_Set(pump_ctrl_state);              //设置水泵控制开关
        printf("\r\n=======================================\r\n");
        printf("\r\nSoil humi is %d% , water pump state is %d\r\n",s_Soil.Humidity,pump_ctrl_state);
        printf("\r\n=======================================\r\n");
        
    }
    
}
/*
* 函数名称 : SoilCtrl_Config
* 功能描述 : 土壤湿度控制配置
* 参    数 : 空
* 返回值   : 空
* 示    例 : SoilHumiCtrl_Config();
*/
/******************************************************************************/ 
void SoilHumiCtrl_Config(void)
/******************************************************************************/  
{
    osThreadAttr_t attr;

    attr.name       = "SoilHumiCtrl-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = SOILHUMICTRL_TASK_STACK_SIZE;
    attr.priority   = SOILHUMICTRL_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)SoilHumiCtrl_Task, NULL, &attr) == NULL) {
        printf("[SoilHumiCtrl_Config] Falied to create SoilHumiCtrl-Task!\n");
    }else{
        printf("[SoilHumiCtrl_Config] Succ to create SoilHumiCtrl-Task!\n");
    }
}

APP_FEATURE_INIT(SoilHumiCtrl_Config);

/******************************* End of File (C) ******************************/