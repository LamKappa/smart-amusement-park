/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  IR_MeasureTemp.c
**  功能描述 :  红外测温
**  作    者 :  王滨泉
**  日    期 :  2021.10.16
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.16
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
#include "hi_pwm.h"
#include "hi_io.h"
#include "Beep.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define IR_TEMP_TASK_STACK_SIZE             (1024*4)                //任务栈大小
#define IR_TEMP_TASK_PRIO                   (10)                    //任务优先等级

#define NTC_ADC_CHANNEL                     HI_ADC_CHANNEL_5        //NTC脚位
#define THERMOPILE_1_ADC_CHANNEL            HI_ADC_CHANNEL_2        //热电堆+脚位
#define REFERENCE_ADC_CHANNEL               HI_ADC_CHANNEL_0        //参考电平

#define BEEP_PIN                            (9)                     //蜂鸣器引脚
#define BUZZER_HZ			                (36000)
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
static TS_ADC_PARAM  s_Ir_Adc = {0};
static uint8_t s_Temp;

extern uint8_t Temperature_Transverter(uint16_t NtcVal, uint16_t VoltDiff, uint8_t *Num);
/******************************************************************************\
                             Functions definitions
\******************************************************************************/

/*
* 函数名称 : Get_IR_Measure_Temp
* 功能描述 : 获取检测温度
* 参    数 : 空
* 返回值   : 温度
* 示    例 : Temp = Get_IR_Measure_Temp();
*/
/******************************************************************************/ 
uint8_t Get_IR_Measure_Temp(void)
/******************************************************************************/
{              
    return s_Temp;      //传递温度值
}



/*
* 函数名称 : Measure_Temp
* 功能描述 : 测量温度
* 参    数 : Temp - 数据指针(电压值放大1000倍)
* 返回值   : 0 - 测量成功
            -1 - 测量失败
* 示    例 : result = Measure_Temp(&Temp);
*/
/******************************************************************************/ 
static char Measure_Temp(uint8_t adc_ch, uint16_t *Temp)
/******************************************************************************/
{
    char ret = 0;
    uint16_t data;
    float voltage = 0;
    uint8_t temp;
    if(hi_adc_read(adc_ch,&data,s_Ir_Adc.EquMode,s_Ir_Adc.CurBais,s_Ir_Adc.DelayCnt) == HI_ERR_SUCCESS)
    {
        *Temp = (uint16_t)(hi_adc_convert_to_voltage(data)*10000);  //ad值转换成电压值
        // printf("\r\nvoltage = %d\n", *Temp);
        ret = 0;                                    //测量成功
    }
    else
    {
        printf("Measure temp fales!!!\n");
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
* 函数名称 : IR_Init
* 功能描述 : 红外传感器初始化
* 参    数 : 空
* 返回值   : 0 - 初始化成功
            -1 - 初始化失败
* 示    例 : result = IR_Init();
*/
/******************************************************************************/ 
static char IR_Temp_Init(void)
/******************************************************************************/
{
    TS_IO_PARAM param;
    uint8_t ret;
    
    //ADC 配置
    // s_Ir_Adc.Channel  = THERMOPILE_1_ADC_CHANNEL;
    s_Ir_Adc.EquMode  = HI_ADC_EQU_MODEL_8;
    s_Ir_Adc.CurBais  = HI_ADC_CUR_BAIS_DEFAULT;
    s_Ir_Adc.DelayCnt = 0;
    return 0;
}

/*
* 函数名称 : Ir_Task
* 功能描述 : 红外传感器检测任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : Ir_Task(&argument);
*/
/******************************************************************************/ 
void Ir_Task(void *argument)
/******************************************************************************/  
{
    uint16_t nr_volt_1, nr_volt_2, ntc_volt, volt_diff,base_volt;
    uint8_t Num;
    uint8_t i;
    IR_Temp_Init();                                              //硬件初始化
    BUZZER_Init();
    while (1)
    {

        BUZZER_Set(BUZZER_ALARM, 100, 200, 2);      //记录空值
        for(i = 0; i < 40; i++)
        {
            Measure_Temp(THERMOPILE_1_ADC_CHANNEL, &nr_volt_1);
            usleep(1000*50);
        }
        printf("\r\n[Ir_Task]nr_volt_1 = %d\n",nr_volt_1);

        BUZZER_Set(BUZZER_ALARM, 1000, 1000, 1);    //开始测试
        for(i = 0; i < 40; i++)
        {
            Measure_Temp(THERMOPILE_1_ADC_CHANNEL, &nr_volt_2);
            Measure_Temp(NTC_ADC_CHANNEL, &ntc_volt);
            usleep(1000*5);
        }
        printf("\r\n[Ir_Task]nr_volt_2 = %d\n",nr_volt_2);
        if(nr_volt_2 > nr_volt_1)
        {
            volt_diff = nr_volt_2 - nr_volt_1;
        }
        else
        {
            volt_diff = nr_volt_1 - nr_volt_2;
        }
        printf("\r\n[Ir_Task]volt_diff = %d\n",volt_diff);
        Measure_Temp(REFERENCE_ADC_CHANNEL, &base_volt);
        // volt_diff = 11966;
        Temperature_Transverter(ntc_volt/10, volt_diff, &s_Temp);
        printf("\r\n[Ir_Task]volt_diff = %d, ntc_volt = %d, base_volt = %d, num = %d\n",volt_diff, ntc_volt, base_volt, s_Temp);

        
    }
    
}
/*
* 函数名称 : SoilCtrl_Config
* 功能描述 : 土壤湿度控制配置
* 参    数 : 无
* 返回值   : 空
* 示    例 : HET_CP_AddressReverse(pdata,ptdata,length);
*/
/******************************************************************************/ 
void Ir_Measure_Temp_Config(void)
/******************************************************************************/  
{
    osThreadAttr_t attr;

    attr.name       = "Ir-Temp-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = IR_TEMP_TASK_STACK_SIZE;
    attr.priority   = IR_TEMP_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Ir_Task, NULL, &attr) == NULL) {
        printf("\r\n [Ir_Measure_Temp_Config] Falied to create Ir-Task!\n");
    }else{
        printf("\r\n [Ir_Measure_Temp_Config] Succ to create Ir-Task!\n");
    }
}

APP_FEATURE_INIT(Ir_Measure_Temp_Config);

/******************************* End of File (C) ******************************/