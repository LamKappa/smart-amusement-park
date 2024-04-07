#include <stdio.h>
#include <unistd.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"
#include "MAX30102.h"
#include "algorithm.h"

uint16_t fifo_red;
uint16_t fifo_ir;







/**
 * @brief max30102写寄存器
 * @param {uint8} uch_addr 寄存器地址
 * @param {uint8} uch_data 写入数据
 * @return {*} 执行是否成功
 * @author yuchen
 */
static unsigned int maxim_max30102_write_reg(uint8 uch_addr, uint8 uch_data)
{
    uint8 buffer[2]={uch_addr,uch_data};
    if(IoTI2cWrite(MAX30102_I2C_IDX,(MAX30102_ADDR<<1)|0,buffer,ARRAY_SIZE(buffer))){
        printf("max30102 write falled.\n");
    }
    return IOT_SUCCESS;
}

/**
 * @brief max30102读寄存器
 * @param {uint8} uch_addr 寄存器地址
 * @param {uint8} *data 读取数据
 * @param {uint16} len 读取数据长度
 * @return {*} 执行是否成功
 * @author yuchen
 */
static unsigned int maxim_max30102_read_reg(uint8 uch_addr, uint8 *data, uint16 len)
{
    unsigned int ret=0;
    ret+=IoTI2cWrite(MAX30102_I2C_IDX,(MAX30102_ADDR<<1)|0,&uch_addr,1);
    ret+=IoTI2cRead(MAX30102_I2C_IDX,(MAX30102_ADDR<<1)|1,data,len);
    if(ret)
        printf("max30102 read falled.\n");
    return ret;
}
void MAX30102_GPIO(void)
{	
	IoTI2cInit(MAX30102_I2C_IDX, MAX30102_I2C_BAUDRATE);    //I2C初始化
	
}

uint8_t Max30102_reset(void)
{
	
	if(maxim_max30102_write_reg(REG_MODE_CONFIG,0x40))
        return 1;
    else
        return 0;    
}

void MAX30102_Config(void)
{
	maxim_max30102_write_reg(REG_INTR_ENABLE_1,0xc0);//// INTR setting
	maxim_max30102_write_reg(REG_INTR_ENABLE_2,0x00);//
	maxim_max30102_write_reg(REG_FIFO_WR_PTR,0x00);//FIFO_WR_PTR[4:0]
	maxim_max30102_write_reg(REG_OVF_COUNTER,0x00);//OVF_COUNTER[4:0]
	maxim_max30102_write_reg(REG_FIFO_RD_PTR,0x00);//FIFO_RD_PTR[4:0]
	
	maxim_max30102_write_reg(REG_FIFO_CONFIG,0x0f);//sample avg = 1, fifo rollover=false, fifo almost full = 17
	maxim_max30102_write_reg(REG_MODE_CONFIG,0x03);//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	maxim_max30102_write_reg(REG_SPO2_CONFIG,0x27);	// SPO2_ADC range = 4096nA, SPO2 sample rate (50 Hz), LED pulseWidth (400uS)  
	maxim_max30102_write_reg(REG_LED1_PA,0x32);//Choose value for ~ 10mA for LED1
	maxim_max30102_write_reg(REG_LED2_PA,0x32);// Choose value for ~ 10mA for LED2
	maxim_max30102_write_reg(REG_PILOT_PA,0x7f);// Choose value for ~ 25mA for Pilot LED
}

void max30102_read_fifo(void)
{
	uint16_t un_temp;
	fifo_red=0;
	fifo_ir=0;
	uint8_t ach_i2c_data[6];
	uint8_t uch_temp;
	unsigned int ret=0;
  
	//read and clear status register
	// IIC_Read_Byte(MAX30102_Device_address,REG_INTR_STATUS_1);
	// IIC_Read_Byte(MAX30102_Device_address,REG_INTR_STATUS_2);

	// ach_i2c_data[0]=REG_FIFO_DATA;

	// IIC_Read_Array(MAX30102_Device_address,REG_FIFO_DATA,ach_i2c_data,6);

	ret+=maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp, sizeof(uch_temp));
    ret+=maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp, sizeof(uch_temp));
    ret+=maxim_max30102_read_reg(REG_FIFO_DATA, ach_i2c_data, 6);
    if (ret)
    {
         printf("max30102 read fifo failed.\n");
    }
	
	un_temp=ach_i2c_data[0];
	un_temp<<=14;
	fifo_red+=un_temp;
	un_temp=ach_i2c_data[1];
	un_temp<<=6;
	fifo_red+=un_temp;
	un_temp=ach_i2c_data[2];
	un_temp>>=2;
	fifo_red+=un_temp;

	un_temp=ach_i2c_data[3];
	un_temp<<=14;
	fifo_ir+=un_temp;
	un_temp=ach_i2c_data[4];
	un_temp<<=6;
	fifo_ir+=un_temp;
	un_temp=ach_i2c_data[5];
	un_temp>>=2;
	fifo_ir+=un_temp;
	
	if(fifo_ir<=10000)
	{
		fifo_ir=0;
	}
	if(fifo_red<=10000)
	{
		fifo_red=0;
	}
}




void MAX30102Task(void *arg)
{
    (void)arg;
    IotGpioValue value;
	int i;
	uint8_t id;


    IoTGpioInit(MAX30102_INT_GPIO);
    IoTGpioSetDir(MAX30102_INT_GPIO, IOT_GPIO_DIR_IN);

	MAX30102_GPIO();
    
    maxim_max30102_read_reg(REG_PART_ID,&id,1);
    printf("\r\n id = %d \r\n",id);

    
	
	Max30102_reset();
	
	MAX30102_Config();

	for(i = 0; i < 128; i++) 
	{
		do{
            IoTGpioGetInputVal(MAX30102_INT_GPIO,&value);
        }while(value == IOT_GPIO_VALUE1);

		//读取FIFO
		max30102_read_fifo();

	}
	while(1)
	{
		blood_Loop();
        usleep(100); 
    }

}



/**
 * @brief MAX30102例程
 * @param {*}
 * @return {*}
 * @author yuchen
 */
void MAX30102Demo(void)
{
    osThreadAttr_t attr;

    attr.name       = "MAX30102Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = MAX30102_TASK_STACK_SIZE;
    attr.priority   = MAX30102_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)MAX30102Task, NULL, &attr) == NULL) {
        printf("\r\n[MAX30102Demo] Falied to create MAX30102Task!\n");
    }else{
        printf("\r\n[MAX30102Demo] Succ to create MAX30102Task!\n");
    }
}

APP_FEATURE_INIT(MAX30102Demo);


