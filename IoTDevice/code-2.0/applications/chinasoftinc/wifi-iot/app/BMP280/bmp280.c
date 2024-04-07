/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  bmp280.c
**  功能描述 :  气压传感器驱动
**  作    者 :  王滨泉
**  日    期 :  2021.10.14
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.14
                1 首次创建                 
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
/******************************************************************************\
                                 Includes
\******************************************************************************/
#define BMP280_TASK_STACK_SIZE              4096                 //任务栈大小
#define BMP280_TASK_PRIO                    10                   //任务优先等级

#define BMP280_I2C_IDX                      0                    //I2C设备号
#define BMP280_I2C_BAUDRATE                 (400*1000)           //I2C波特率
#define BMP280_ADDR                         0x76                 //BMP280设备地址
#define BMP280_CSB                          8                    //SPI通信模式下用到的引脚，本次没用到，可以悬空    
#define BMP280_SDO                          6                    //传感器地址控制位，接GND的时候I2C中器件地址为0x76，接高电平为0x77，本次接GND
//寄存器地址
#define BMP280_CHIP_ID_ADDR                 0xD0
#define BMP280_SOFT_RESET_ADDR              0xE0
#define BMP280_STATUS_ADDR                  0xF3
#define BMP280_CTRL_MEAS_ADDR               0xF4
#define BMP280_CONFIG_ADDR                  0xF5
#define BMP280_PRES_MSB_ADDR                0xF7
#define BMP280_PRES_LSB_ADDR                0xF8
#define BMP280_PRES_XLSB_ADDR               0xF9
#define BMP280_TEMP_MSB_ADDR                0xFA
#define BMP280_TEMP_LSB_ADDR                0xFB
#define BMP280_TEMP_XLSB_ADDR               0xFC
//校准参数寄存器地址 
#define BMP280_DIG_T1_LSB_ADDR              0x88
#define BMP280_DIG_T1_MSB_ADDR              0x89
#define BMP280_DIG_T2_LSB_ADDR              0x8A
#define BMP280_DIG_T2_MSB_ADDR              0x8B
#define BMP280_DIG_T3_LSB_ADDR              0x8C
#define BMP280_DIG_T3_MSB_ADDR              0x8D
#define BMP280_DIG_P1_LSB_ADDR              0x8E
#define BMP280_DIG_P1_MSB_ADDR              0x8F
#define BMP280_DIG_P2_LSB_ADDR              0x90
#define BMP280_DIG_P2_MSB_ADDR              0x91
#define BMP280_DIG_P3_LSB_ADDR              0x92
#define BMP280_DIG_P3_MSB_ADDR              0x93
#define BMP280_DIG_P4_LSB_ADDR              0x94
#define BMP280_DIG_P4_MSB_ADDR              0x95
#define BMP280_DIG_P5_LSB_ADDR              0x96
#define BMP280_DIG_P5_MSB_ADDR              0x97
#define BMP280_DIG_P6_LSB_ADDR              0x98
#define BMP280_DIG_P6_MSB_ADDR              0x99
#define BMP280_DIG_P7_LSB_ADDR              0x9A
#define BMP280_DIG_P7_MSB_ADDR              0x9B
#define BMP280_DIG_P8_LSB_ADDR              0x9C
#define BMP280_DIG_P8_MSB_ADDR              0x9D
#define BMP280_DIG_P9_LSB_ADDR              0x9E
#define BMP280_DIG_P9_MSB_ADDR              0x9F
//校准参数个数
#define BMP280_CALIB_DATA_SIZE              24

int16 dig_T[3]={0};//温度校准参数
int16 dig_P[9]={0};//压力校准参数


typedef struct
{
    long Temp;
    long Press;
}TS_BMP280_PARAM;


TS_BMP280_PARAM s_Bmp280 = {0};
/******************************************************************************\
                             Variables definitions
\******************************************************************************/

/*
* 函数名称 : BMP280_write_reg
* 功能描述 : BMP280写寄存器
* 参    数 : uch_addr - 寄存器地址
             uch_data - 写入的数据
* 返回值   : 0 - 写入成功
            -1 - 写入失败
* 示    例 : result = BMP280_write_reg(uch_addr,uch_data);
*/
/******************************************************************************/ 
static unsigned int BMP280_write_reg(uint8 uch_addr, uint8 uch_data)
/******************************************************************************/ 
{
    uint8 buffer[2]={uch_addr,uch_data};
    if(IoTI2cWrite(BMP280_I2C_IDX,(BMP280_ADDR<<1)|0,buffer,ARRAY_SIZE(buffer))){
        printf("BMP280 write falled.\n");
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}

/*
* 函数名称 : BMP280_read_reg
* 功能描述 : BMP280读寄存器
* 参    数 : uch_addr - 寄存器地址
             data - 读取数据指针
             len - 读取数据长度
* 返回值   : 0 - 读取成功
            -1 - 读取失败
* 示    例 : result = BMP280_read_reg(uch_addr,&data,len);
*/
/******************************************************************************/ 
static unsigned int BMP280_read_reg(uint8 uch_addr, uint8 *data, uint16 len)
/******************************************************************************/ 
{
    unsigned int ret=0;
    ret+=IoTI2cWrite(BMP280_I2C_IDX,(BMP280_ADDR<<1)|0,&uch_addr,1);
    ret+=IoTI2cRead(BMP280_I2C_IDX,(BMP280_ADDR<<1)|1,data,len);
    if (ret)
    {
        printf("BMP280 read falled.\n");
    }
    return ret;
}

/*
* 函数名称 : BMP280_Init
* 功能描述 : BMP280初始化
* 参    数 : 空
* 返回值   : 初始化结果
* 示    例 : result = BMP280_Init();
*/
/******************************************************************************/ 
static unsigned int BMP280_Init(void)
/******************************************************************************/ 
{
    unsigned int ret=0;
    //状态全部清零
    ret+=BMP280_write_reg(BMP280_SOFT_RESET_ADDR, 0xb6);
    // ret+=BMP280_write_reg(BMP280_CTRL_MEAS_ADDR, 0xff);
    // ret+=BMP280_write_reg(BMP280_CONFIG_ADDR, 0x00);
    ret+=BMP280_write_reg(BMP280_CTRL_MEAS_ADDR, 0b10110011);
    ret+=BMP280_write_reg(BMP280_CONFIG_ADDR, 5<<2);
    if (ret)
    {
        printf("BMP280 init falled.\n");
    }
    return ret;
}

/*
* 函数名称 : get_calib_param
* 功能描述 : 读取校准参数
* 参    数 : 空
* 返回值   : 0 - 操作成功
            -1 - 操作失败
* 示    例 : result = get_calib_param();
*/
/******************************************************************************/ 
static unsigned int get_calib_param(void)
/******************************************************************************/ 
{
    int8 rslt;
    uint8 temp[BMP280_CALIB_DATA_SIZE] = { 0 };
    uint8 i = 0;
    rslt = BMP280_read_reg(BMP280_DIG_T1_LSB_ADDR, temp, BMP280_CALIB_DATA_SIZE);
    if (rslt)
    {
        printf("BMP280 get calib param falled.\n");
        return rslt;
    }
    for (i = 0; i < ARRAY_SIZE(dig_T); i++)
    {
        dig_T[i]=(int16_t) (((int16_t) temp[2*i+1] << 8) | ((int16_t) temp[2*i]));
    }
    for (i = 3; i < ARRAY_SIZE(dig_P)+3; i++)
    {
        dig_P[i-3]=(int16_t) (((int16_t) temp[2*i+1] << 8) | ((int16_t) temp[2*i]));
    }
    return rslt;
}

/*
* 函数名称 : bmp280_GetValue
* 功能描述 : 读取温度、气压
* 参    数 : Temperature - 温度数据指针
             Pressure - 气压数据指针 , 帕
* 返回值   : 1 - 操作成功
             0 - 操作失败
* 示    例 : result = bmp280_GetValue(&Temperature, &Pressure);
*/
/******************************************************************************/ 
int bmp280_GetValue(long *Temperature,long *Pressure)
/******************************************************************************/ 
{
    long adc_T;
    long adc_P;
    long var1, var2, t_fine, T, p;
    uint8 buf[3];
    BMP280_read_reg(BMP280_TEMP_MSB_ADDR,buf,3);
    // adc_T=buf[0]<<12|buf[1]<<8|buf[2];
    adc_T=(buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
    BMP280_read_reg(BMP280_PRES_MSB_ADDR,buf,3);
    // adc_P=buf[0]<<12|buf[1]<<8|buf[2];
    adc_P=(buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
    if(adc_P == 0)
    {
        return 0;
    }

    //Temperature
    var1 = (((double)adc_T)/16384.0-((double)(uint16)dig_T[0])/1024.0)*((double)dig_T[1]);
    var2 = ((((double)adc_T)/131072.0-((double)(uint16)dig_T[0])/8192.0)*(((double)adc_T)
                /131072.0-((double)(uint16)dig_T[0])/8192.0))*((double)dig_T[2]);

    t_fine = (unsigned long)(var1+var2);
    T = (var1+var2)/5120.0;

    //Pressure
    var1 = ((double)t_fine/2.0)-64000.0;
    var2 = var1*var1*((double)dig_P[5])/32768.0;
    var2 = var2 +var1*((double)dig_P[4])*2.0;
    var2 = (var2/4.0)+(((double)dig_P[3])*65536.0);
    var1 = (((double)dig_P[2])*var1*var1/524288.0+((double)dig_P[1])*var1)/524288.0;
    var1 = (1.0+var1/32768.0)*((double)(uint16)dig_P[0]);
    p = 1048576.0-(double)adc_P;
    p = (p-(var2/4096.0))*6250.0/var1;
    var1 = ((double)dig_P[8])*p*p/2147483648.0;
    var2 = p*((double)dig_P[7])/32768.0;
    p = p+(var1+var2+((double)dig_P[6]))/16.0;

    *Temperature=T;
    *Pressure=p;
    return 1;
}   

/*
* 函数名称 : Get_Bmp220_Pressure
* 功能描述 : 读取气压值
* 参    数 : 空
* 返回值   : 气压值
* 示    例 : Pressure = Get_Bmp220_Pressure();
*/
/******************************************************************************/ 
long Get_Bmp220_Pressure(void) 
/******************************************************************************/ 
{
    return s_Bmp280.Press;
}
/*
* 函数名称 : BMP280Task
* 功能描述 : BMP280传感器操作任务
* 参    数 : arg - 任务参数
* 返回值   : 空
* 示    例 : BMP280Task();
*/
/******************************************************************************/ 
void BMP280Task(void *arg)
/******************************************************************************/ 
{
    uint8 id=0;
    uint8_t tmp;
    
    
    while(id != 0x58)
    {
        sleep(1);

        IoTI2cInit(BMP280_I2C_IDX, BMP280_I2C_BAUDRATE);//I2C初始化
        BMP280_read_reg(BMP280_CHIP_ID_ADDR,&id,1);
        printf("CHIP_ID:%x\n",id);
    }
    
    get_calib_param();
    BMP280_Init();  //initialize the BMP280
    
    
    while (1)
    {
        bmp280_GetValue(&s_Bmp280.Temp,&s_Bmp280.Press);
        printf("Temp = %d, Press = %d\n",s_Bmp280.Temp,s_Bmp280.Press);
        sleep(1);
    }
}
/*
* 函数名称 : BMP280Demo
* 功能描述 : BMP280传感器示例
* 参    数 : 空
* 返回值   : 空
* 示    例 : BMP280Demo();
*/
/******************************************************************************/ 
void BMP280Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "BMP280Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = BMP280_TASK_STACK_SIZE;
    attr.priority   = BMP280_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)BMP280Task, NULL, &attr) == NULL) {
        printf("[BMP280Demo] Falied to create BMP280Task!\n");
    }else{
        printf("[BMP280Demo] Succ to create BMP280Task!\n");
    }
}

// APP_FEATURE_INIT(BMP280Demo);
/******************************* End of File (C) ******************************/