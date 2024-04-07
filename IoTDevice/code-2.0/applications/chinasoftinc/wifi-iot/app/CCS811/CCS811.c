/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  CCS811.c
**  功能描述 :  二氧化碳模组
**  作    者 :  王滨泉
**  日    期 :  2021.10.14
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.14
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
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define CCS811_TASK_STACK_SIZE                  (4096)                  //任务栈大小
#define CCS811_TASK_PRIO                        (10)                    //任务优先等级

#define CCS811_I2C_IDX                          (0)                     //I2C设备号
#define CCS811_I2C_BAUDRATE                     (400*1000)              //I2C波特率
#define CCS811_ADDR                             0x5A                    //CCS811设备地址

#define CCS811_WAKE                             (11)
//------------------------------------------------------------------------------
//寄存器地址
#define CCS811_REG_STATUS                        0x00
#define CCS811_REG_MEAS_MODE                     0x01
#define CCS811_REG_ALG_RESULT_DATA               0x02
#define CCS811_REG_RAW_DATA                      0x03
#define CCS811_REG_ENV_DATA                      0x05
#define CCS811_REG_THRESHOLDS                    0x10
#define CCS811_REG_BASELINE                      0x11
#define CCS811_REG_HW_ID                         0x20
#define CCS811_REG_HW_VERSION                    0x21
#define CCS811_REG_FW_BOOT_VERSION               0x23
#define CCS811_REG_FW_APP_VERSION                0x24
#define CCS811_REG_INTERNAL_STATE                0xA0
#define CCS811_REG_ERROR_ID                      0xE0
#define CCS811_REG_SW_RESET                      0xFF

#define CCS811_BOOTLOADER_APP_ERASE              0xF1
#define CCS811_BOOTLOADER_APP_DATA               0xF2
#define CCS811_BOOTLOADER_APP_VERIFY             0xF3
#define CCS811_BOOTLOADER_APP_START              0xF4
//------------------------------------------------------------------------------
//读取数据状态
typedef enum{
    CCS811_INIT,      
    CCS811_READ_ID,      
    CCS811_READ_STATUS,     
    CCS811_APP_START, 
    CCS811_READ_DATA,
    CCS811_READ_ERROR_ID,
    CCS811_READ_SW_RESET,
}TE_READDATA_STEP;
//------------------------------------------------------------------------------
//温度、湿度结构体
typedef struct 
{
    float temperature;
    float humidity;
}ccs811_envdata;
//------------------------------------------------------------------------------
//模式结构体
typedef enum
{
    CCS811_MODE_0,      /* Idle (Measurements are disabled in this mode) */
    CCS811_MODE_1,      /* Constant power mode, IAQ measurement every second */
    CCS811_MODE_2,      /* Pulse heating mode IAQ measurement every 10 seconds */
    CCS811_MODE_3,      /* Low power pulse heating mode IAQ measurement every 60 seconds */
    CCS811_MODE_4       /* Constant power mode, sensor measurement every 250ms */
} ccs811_mode_t;
//------------------------------------------------------------------------------
//模式结构体
typedef struct 
{
    uint8    thresh;
    uint8    interrupt;
    ccs811_mode_t mode;
}ccs811_meas_mode;
//------------------------------------------------------------------------------
//CCS811参数
typedef struct 
{
    uint16_t eco2;
    uint16_t tvoc;
}TS_CCS811_PARAM;


uint8 Information[10]={0};//芯片信息
uint8 Status;//设备状态
TS_CCS811_PARAM s_Ccs811 = {0};
/******************************************************************************\
                             Functions definitions
\******************************************************************************/ 	

/*
* 函数名称 : CCS811_command
* 功能描述 : CCS811 iic 通讯
* 参    数 : cmd - 发送的指令
             cmdlen - 指令长度
             delayms - 等待时间
             readdata - 接收的数据指针
             readlen - 接收长度
* 返回值   : 错误代码
* 示    例 : CCS811_command(cmd, cmdlen, delayms, &readdata, readlen);
*/
/******************************************************************************/ 
static unsigned int CCS811_command(uint8 cmd[], uint8 cmdlen, uint8 delayms, uint8 *readdata, uint8 readlen)
/******************************************************************************/ 
{
    unsigned int ret=0;
    /* Request */
    ret=IoTI2cWrite(CCS811_I2C_IDX,(CCS811_ADDR<<1)|0,cmd,cmdlen);
    if (ret)
    {
        printf("CCS811 write falled.\n");
        return IOT_FAILURE;
    }

    /* If not need reply */
    if (readlen == 0) return IOT_SUCCESS;
    usleep(delayms*1000);

    /* Response */
    ret=IoTI2cRead(CCS811_I2C_IDX,(CCS811_ADDR<<1)|1,readdata,readlen);
    if (ret)
    {
        printf("CCS811 read falled.\n");
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}


/*
* 函数名称 : CCS811_Init
* 功能描述 : CCS811初始化
* 参    数 : 空
* 返回值   : 空
* 示    例 : CCS811_Init();
*/
/******************************************************************************/ 
static unsigned int CCS811_Init(void)
/******************************************************************************/ 
{
    unsigned int ret=0;
    uint8 cmd[5]={0};

    //读取硬件ID
    cmd[0]=CCS811_REG_HW_ID;
    ret+=CCS811_command(cmd,1,1,Information,1);
    printf("CCS811_REG_HW_ID:%d\n",Information[0]);
    // //软件复位
    // cmd[0] = CCS811_REG_SW_RESET;
    // cmd[1] = 0x11;
    // cmd[2] = 0xE5;
    // cmd[3] = 0x72;
    // cmd[4] = 0x8A;
    // ret+=CCS811_command(cmd,5,1,0,0);
    // sleep(1);
    
    //读取固件boot版本
    cmd[0]=CCS811_REG_FW_BOOT_VERSION;
    ret+=CCS811_command(cmd,1,1,&Information[1],2);
    printf("CCS811_REG_FW_BOOT_VERSION:%d\n",Information[1]<<8|Information[2]);
    //读取固件app版本
    cmd[0]=CCS811_REG_FW_APP_VERSION;	
	ret+=CCS811_command(cmd,1,1,&Information[3],2); 
    printf("CCS811_REG_FW_APP_VERSION:%d\n",Information[3]<<8|Information[4]);
    //读取设备状态
    cmd[0]=CCS811_REG_STATUS;
    ret+=CCS811_command(cmd,1,1,&Status,1);
    printf("CCS811_REG_STATUS:%d\n",Status);
    //启动APP
    cmd[0]=CCS811_BOOTLOADER_APP_START;
    if(Status&0x10)	 CCS811_command(cmd,1,1,0,0);
    //读取设备状态
    cmd[0]=CCS811_REG_STATUS;
    ret+=CCS811_command(cmd,1,1,&Status,1);
    printf("CCS811_REG_STATUS:%d\n",Status);
    //设置模式
    cmd[0] = CCS811_REG_MEAS_MODE;
    ccs811_meas_mode meas={0,0,CCS811_MODE_4};
    cmd[1] = (meas.thresh << 2) | (meas.interrupt << 3) | (meas.mode << 4);
    ret+=CCS811_command(cmd,2,1,0,0);
    // 设置温度、湿度平衡值
    cmd[0] = CCS811_REG_MEAS_MODE;
    ccs811_envdata envdata={25, 50};
    cmd[1]=(int)(envdata.humidity+25)<<1;
    cmd[2]=0;
    cmd[3]=(int)envdata.temperature<<1;
    cmd[4]=0;
    ret+=CCS811_command(cmd,5,1,0,0);
    if (ret)
    {
        printf("CCS811 init falled.\n");
    }
    return ret;
}
/*
* 函数名称 : CCS811_SW_Reset
* 功能描述 : 软件复位
* 参    数 : 空
* 返回值   : 空
* 示    例 : CCS811_SW_Reset();
*/
/******************************************************************************/ 
void CCS811_SW_Reset(void)
/******************************************************************************/ 
{
    uint8 cmd[5]={0};

    //软件复位
    cmd[0] = CCS811_REG_SW_RESET;
    cmd[1] = 0x11;
    cmd[2] = 0xE5;
    cmd[3] = 0x72;
    cmd[4] = 0x8A;
    CCS811_command(cmd,5,1,0,0);
    printf("\r\n SW RESET!!!");
    sleep(1);
}
/*
* 函数名称 : Get_CCS811_CO2
* 功能描述 : 获取二氧化碳浓度
* 参    数 : 空
* 返回值   : CO2值
* 示    例 : CO2 = Get_CCS811_CO2();
*/
/******************************************************************************/ 
uint16_t Get_CCS811_CO2(void)
/******************************************************************************/ 
{
    return s_Ccs811.eco2;
}
/*
* 函数名称 : Get_CCS811_TVOC
* 功能描述 : 获取TVOC
* 参    数 : 空
* 返回值   : TVOC值
* 示    例 : TVOC = Get_CCS811_TVOC();
*/
/******************************************************************************/ 
uint16_t Get_CCS811_TVOC(void)
/******************************************************************************/ 
{
    return s_Ccs811.tvoc;
}

/*
* 函数名称 : CCS811Task
* 功能描述 : CCS811任务
* 参    数 : 空
* 返回值   : 空
* 示    例 : CCS811Task(&argument);
*/
/******************************************************************************/ 
void CCS811Task(void *arg)
/******************************************************************************/ 
{
    uint8_t step = CCS811_INIT;
    uint8_t error;
    uint8 cmd[5]={0};
    uint8 buf[8]={0};
    unsigned int ret=0;
    uint8 first_flag = 0;
    IoTI2cInit(CCS811_I2C_IDX, CCS811_I2C_BAUDRATE);//I2C初始化
    IoTGpioInit(CCS811_WAKE);
    IoTGpioSetDir(CCS811_WAKE, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(CCS811_WAKE,IOT_GPIO_VALUE0);
    sleep(1);
    // CCS811_Init();  //initialize the CCS811
    
    while (1)
    {
        ret = 1;
        do{
            switch(step)
            {
                
                case CCS811_INIT:
                    CCS811_Init();
                    step = CCS811_READ_ID;
                    break;
                case CCS811_READ_ID:
                    cmd[0]=CCS811_REG_HW_ID;
                    CCS811_command(cmd,1,1,Information,1);
                    printf("\r\nCCS811_REG_HW_ID:%d\n",Information[0]);
                    if(Information[0] != 129)                   //数据异常
                    {
                        printf("\r\nCCS811_REG_HW_ID is error!!\n",Information[0]);
                        step = CCS811_READ_SW_RESET;
                    }
                    else
                    {
                        step = CCS811_READ_STATUS;
                    }
                    break;
                case CCS811_READ_STATUS:
                    do{
                        usleep(1000*5);
                        cmd[0]=CCS811_REG_STATUS;
                        CCS811_command(cmd,1,1,&Status,1);
                        printf("CCS811_REG_STATUS:%x\n",Status);
                    }while(!((Status & 0x90) == 0x90));
                    step = CCS811_APP_START;
                    break;
                case CCS811_APP_START:
                    cmd[0]=CCS811_BOOTLOADER_APP_START;
                    CCS811_command(cmd,1,1,0,0);
                    step = CCS811_READ_ERROR_ID;
                    break;
                case CCS811_READ_ERROR_ID:
                    cmd[0]=CCS811_REG_ERROR_ID;
                    uint8 buf[8]={0};
                    CCS811_command(cmd,1,1,&error,1);
                    step = CCS811_READ_DATA;
                    printf("CCS811_REG_ERROR_ID:%x\n",error);
                    break;
                case CCS811_READ_DATA:
                    cmd[0]=CCS811_REG_ALG_RESULT_DATA;
                    CCS811_command(cmd,1,1,buf,8);
                    if(first_flag)
                    {
                        s_Ccs811.eco2 = ((uint16)buf[0] << 8)+buf[1];
                        s_Ccs811.tvoc = ((uint16)buf[2] << 8)+buf[3];
                    }
                    else
                    {
                        first_flag = 1;
                    }
                    printf("\r\n[CCS811Task]eco2 = %d, tvoc:%d\n",((uint16)buf[0] << 8)+buf[1], ((uint16)buf[2] << 8)+buf[3]);
                    step = CCS811_READ_ID;
                    ret = 0;
                    break;
                case CCS811_READ_SW_RESET:
                    CCS811_SW_Reset();
                    first_flag = 0;
                    step = CCS811_INIT;
                    break;
            }
            usleep(1000*10);
        }while(ret);

        sleep(1);
        // printf("CCS811Task run 1s\n");
        
    }
}
/*
* 函数名称 : CCS811Demo
* 功能描述 : CCS811示例配置
* 参    数 : 空
* 返回值   : 空
* 示    例 : CCS811Demo();
*/
/******************************************************************************/ 
void CCS811Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "CCS811Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = CCS811_TASK_STACK_SIZE;
    attr.priority   = CCS811_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)CCS811Task, NULL, &attr) == NULL) {
        printf("\r\n[CCS811Demo] Falied to create CCS811Task!\n");
    }else{
        printf("\r\n[CCS811Demo] Succ to create CCS811Task!\n");
    }
}

APP_FEATURE_INIT(CCS811Demo);
/******************************* End of File (C) ******************************/