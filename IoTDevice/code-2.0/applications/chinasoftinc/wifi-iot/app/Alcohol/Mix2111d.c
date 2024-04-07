/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  Mix2111d.c
**  功能描述 :  Mix2111d酒精浓度模组驱动
**  作    者 :  王滨泉
**  日    期 :  2021.11.01
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.11.01
                1 首次创建                 
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/
#include "Mix2111d.h"
#include "Beep.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define MIX2111D_TASK_STACK_SIZE         4096*2                 //任务栈大小
#define MIX2111D_TASK_PRIO               10                   	//任务优先等级
#define MIX2111D_UART_NUM                HI_UART_IDX_2       	//使用串口2与模组通讯
#define MIX2111D_UART_BAUDRATE           9600                	//串口通讯波特率
#define MIX2111D_UART_TX                 12                  	//GPIO12
#define MIX2111D_UART_RX                 11                  	//GPIO11


//-----------------------------------------------------------------------------
//传感器操作命令
const uint8_t s_Action_Cmd[2][9] = 
{
	//读取传感器浓度值
	{0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79},
	//用户自标定传感器零点值
	{0xFF, 0x01, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A},
};

static uint32_t s_Thread_Mux_Id;
static uint16_t s_Alcohol = 0;				//酒精浓度
static uint16_t s_Alcohol_Threshold = 1000;	//酒精浓度阈值
/******************************************************************************\
                             Functions definitions
\******************************************************************************/ 							   	 
/*
* 函数名称 : Get_MIX2111D_Alcohol
* 功能描述 : 获取酒精浓度
* 参    数 : 无
* 返回值   : 酒精浓度
* 示    例 : Alcohol = Get_MIX2111D_Alcohol();
*/
/******************************************************************************/ 
uint16_t Get_MIX2111D_Alcohol(void)
/******************************************************************************/
{
    return s_Alcohol;
}

/*
* 函数名称 : Get_MIX2111D_Alcohol_Alarm_Threshold
* 功能描述 : 获取酒精浓度阈值
* 参    数 : 无
* 返回值   : 酒精浓度
* 示    例 : Alcohol = Get_MIX2111D_Alcohol_Alarm_Threshold();
*/
/******************************************************************************/ 
uint16_t Get_MIX2111D_Alcohol_Alarm_Threshold(void)
/******************************************************************************/
{
    return s_Alcohol_Threshold;
}

/*
* 函数名称 : Set_MIX2111D_Alcohol_Alarm_Threshold
* 功能描述 : 设置酒精浓度阈值
* 参    数 : 无
* 返回值   : 酒精浓度
* 示    例 : Set_MIX2111D_Alcohol_Alarm_Threshold(Alcohol_Threshold);
*/
/******************************************************************************/ 
void Set_MIX2111D_Alcohol_Alarm_Threshold(uint16_t Alcohol_Threshold)
/******************************************************************************/
{
    s_Alcohol_Threshold = Alcohol_Threshold;
}

/*
* 函数名称 : MIX2111D_Init
* 功能描述 : MIX2111D模组初始化
* 参    数 : 无
* 返回值   : 0 - 初始化成功
            -1 - 初始化失败
* 示    例 : result = MIX2111D_Init();
*/
/******************************************************************************/ 
int MIX2111D_Init(void)
/******************************************************************************/ 
{
    int ret = 0;

    hi_uart_attribute uart_attr = {
        .baud_rate = MIX2111D_UART_BAUDRATE,    /* baud_rate: 9600 */
        .data_bits = 8,                         /* data_bits: 8bits */
        .stop_bits = 1,
        .parity = 0,
    };

    /* Initialize uart driver */
    ret = hi_uart_init(MIX2111D_UART_NUM, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("\r\nFailed to init uart%d! Err code = %d\n", MIX2111D_UART_NUM, ret);
        return ret;
    }
	else
	{
		printf("\r\nSuccess to init uart2!!!!\n");
	}
}

/*
* 函数名称 : MIX2111D_Task
* 功能描述 : MIX2111D任务
* 参    数 : arg - 任务参数
* 返回值   : 空
* 示    例 : MIX2111D_Task(&argument);
*/
/******************************************************************************/ 
void MIX2111D_Task(void *arg)
/******************************************************************************/ 
{
    int ret = 0;
	uint16_t i;
    uint8_t recv_data_buf[20] = {0};

    MIX2111D_Init();  //initialize the mix2111d
	
    while (1)
    {
		sleep(1);
		hi_uart_write(MIX2111D_UART_NUM, &s_Action_Cmd[0][0], 9);			//发送读取传感器浓度值命令

		uint32_t len = hi_uart_read_timeout(MIX2111D_UART_NUM, recv_data_buf, sizeof(recv_data_buf),100);	//接收传感器数据
		if (len > 0) 
		{
			if(recv_data_buf[0] == 0xFF && recv_data_buf[1] == 0x86)		//判断包头和命令字是否正确
			{
				s_Alcohol = (recv_data_buf[2]<<8) | recv_data_buf[3];		//传感器数值转换
			}
            printf("\r\n============================\r\n");
			printf("Alcohol = %d",s_Alcohol);
            printf("\r\n============================\r\n");
			
			memset(recv_data_buf,0,sizeof(recv_data_buf));
		} 
		if(s_Alcohol >= s_Alcohol_Threshold)
		{
			BUZZER_Set(BUZZER_KEEP_ALARM, 500, 1000, 0);    //酒精浓度超标，蜂鸣器报警
		}
		else
		{
			BUZZER_Set(BUZZER_OFF, 0, 0, 0);    			//酒精浓度正常，蜂鸣器停止报警
		}
        usleep(50);

    }
}
/*
* 函数名称 : MIX2111D_Demo
* 功能描述 : MIX2111D示例配置
* 参    数 : 空
* 返回值   : 空
* 示    例 : MIX2111D_Demo();
*/
/******************************************************************************/ 
void MIX2111D_Demo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "MIX2111D-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = MIX2111D_TASK_STACK_SIZE;
    attr.priority   = MIX2111D_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)MIX2111D_Task, NULL, &attr) == NULL) {
        printf("\r\n[MIX2111D_Demo] Falied to create MIX2111D Demo!\n");
    }else{
        printf("\r\n[MIX2111D_Demo] Succ to create MIX2111D Demo!\n");
    }
}

APP_FEATURE_INIT(MIX2111D_Demo);
/******************************* End of File (C) ******************************/
