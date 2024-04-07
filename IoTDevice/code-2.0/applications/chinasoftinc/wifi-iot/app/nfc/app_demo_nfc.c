/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  app_demo_nfc.c
**  功能描述 :  蓝牙数据通讯
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
#include <unistd.h>
#include <securec.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_i2c.h"

#include "c081_nfc.h"

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define NFC_TASK_STACK_SIZE     (4096)
#define NFC_TASK_PRIO           (25)

#define NFC_I2C_IDX             (0)
#define NFC_I2C_BAUDRATE        (400*1000)
#define PIN_GPIO_NFC_CSN        (9)
#define PIN_GPIO_NFC_IRQ        (10)

//-----------------------------------------------------------------------------
//CC文件内容(功能容器)
uint8 capability_container[15] =
{   0x00, 0x0F,        //CCLEN
    0x20,              //Mapping Version
    0x00, 0xF6,        //R-APDU size MLe 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x00, 0xF6,        //C-APDU size MLc 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x04,              //NDEF消息格式 05的话就是私有
    0x06,              //NDEF消息长度
    0xE1, 0x04,        //NDEF FILE ID       NDEF的文件标识符
    // 0x03, 0x84,        //NDEF最大长度
    0x00, 0x32,        //NDEF最大长度
    0x00,              //Read Access           可读
    0x00               //Write Access          可写
};
//-----------------------------------------------------------------------------
//ndef文件内容
uint8 ndef_file[1024] = {
#ifdef HUAWEI_COM
    /*http://wwww.huawei.com*/
    0x00,0x0F,
    0xD1,0x01,0x0B,0x55,
    0x01,0x68,0x75,0x61,
    0x77,0x65,0x69,0x2E,
    0x63,0x6F,0x6D,
#endif
/*wechat*/
#ifdef  NFC_TAG_WECHAT
    0x00,0x20,
    0xd4, 0x0f,0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    0x69, 0x64,0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    0x6b, 0x67,0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x65,
    0x6e, 0x63,0x65, 0x6e, 0x74, 0x2e, 0x6d, 0x6d,
#endif


#ifdef NFC_TAG_CHINASOFTINC
    // 微信
    // 0x00,0x20,
    // 0xd4, 0x0f, 0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    // 0x69, 0x64, 0x2e, 0x63, 0x6F, 0x6D, 0x2E, 0x63, 
    // 0x68, 0x69, 0x6E, 0x61, 0x73, 0x6F, 0x66, 0x74, 
    // 0x69, 0x6E, 0x63, 0x2E, 0x6E, 0x66, 0x63, 0x64, 
    // 0x65, 0x6D, 0x6F,


    // 微博
    // 0x00,0x20,
    // 0xd4, 0x0f,0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    // 0x69, 0x64,0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    // 0x6b, 0x67,0x63, 0x6F, 0x6D, 0x2E, 0x73, 0x69, 
    // 0x6E, 0x61,0x2E, 0x77, 0x65, 0x69, 0x62, 0x6F,

    // 中软 app com.skh.dboard
    0x00, 0x20,
    0xd4, 0x0f, 0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f, 
    0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    0x6b, 0x67, 0x63, 0x6F, 0x6D, 0x2E, 0x73, 0x6B,
    0x68, 0x2E, 0x64, 0x62, 0x6F, 0x61, 0x72, 0x64,
    // 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 

    // 0x68, 0x64, 0x64, 0x62, 0x6F, 0x61, 0x72, 0x64,

    0x00, 0x1f,
    // 0x00, 0x32,
    // // app com.skh.dboard
    // 0x94, 0x0f, 0x0e, 
    // 0x61, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70, 0x6b, 0x67, 
    // 0x63, 0x6F, 0x6D, 0x2E, 0x73, 0x6B, 0x68, 0x2E, 0x64, 0x62, 0x6F, 0x61, 0x72, 0x64,
    //txt hello world
    0x91, 0x01, 0x0E, 0x54, 
    0x02, 0x65, 0x6E, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 
    //uri www.sina.com
    0x51, 0x01, 0x09, 0x55, 
    0x01, 0x73, 0x69, 0x6E, 0x61, 0x2E, 0x63, 0x6F, 0x6D,
    // 0x51, 0x01, 0x0B, 0x55,
    // 0x01, 0x68, 0x75, 0x61, 0x77, 0x65, 0x69, 0x2E, 0x63, 0x6F, 0x6D,
    

#endif
};
T4T_FILE current_file;
uint8 fm327_fifo[1024];
uint8 irq_data_in = 0;//非接数据接收终端标识
uint8 irq_txdone = 0;
uint8 rfLen;
uint8 irq_rxdone = 0;
uint8 irq_data_wl =0;
uint8 FlagFirstFrame = 0; //卡片首帧标识
uint8 read_reg =0;
uint8_t debug_str[256] = {0};
static hi_u32 s_Nfc_Mux_Id;
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : write_read
* 功能描述 : IIC数据读写
* 参    数 : reg_high_8bit_cmd - 寄存器高字节
             reg_low_8bit_cmd - 寄存器低字节
             recv_data - 读写数据指针
             send_len - 写入字节长度
             read_len - 读取数据长度
* 返回值   : 读写结果
* 示    例 : write_read(reg_high_8bit_cmd,reg_low_8bit_cmd,&recv_data,send_len,read_len);
*/
/******************************************************************************/ 
uint32 write_read(uint8 reg_high_8bit_cmd, uint8 reg_low_8bit_cmd, uint8* recv_data, uint8 send_len, uint8 read_len)
/******************************************************************************/ 
{
    /*i2c read*/
    uint8 buffer[] = {reg_high_8bit_cmd, reg_low_8bit_cmd};
	
    memset_s(recv_data, read_len, 0x0, read_len);
    
    read_reg = NFC_CLEAN;   // 消除stop信号
    IoTI2cWrite(NFC_I2C_IDX, C081_NFC_ADDR & 0xFE, buffer, ARRAY_SIZE(buffer));    
    IoTI2cRead(NFC_I2C_IDX, C081_NFC_ADDR | I2C_RD, recv_data, read_len);
    return IOT_SUCCESS;
}
/*
* 函数名称 : c08i_nfc_i2c_write
* 功能描述 : nfc rrprom数据写入
* 参    数 : reg_high_8bit_cmd  - 寄存器高字节
             reg_low_8bit_cmd   - 寄存器低字节
             data_buff          - 读写数据指针
             len                - 写入字节长度
* 返回值   : 写结果
* 示    例 : c08i_nfc_i2c_write(reg_high_8bit_cmd,reg_low_8bit_cmd,&recv_data,send_len,read_len);
*/
/******************************************************************************/ 
uint32 c08i_nfc_i2c_write( uint8 reg_high_8bit_cmd, uint8 reg_low_8bit_cmd, uint8* data_buff, uint8 len)
/******************************************************************************/ 
{
    /*co8i 写命令: 该接口写eeprom 更改芯片配置*/
    uint8 buffer[64] = {0};
    buffer[0] = reg_high_8bit_cmd;
    buffer[1] = reg_low_8bit_cmd;

    for (int32 i = 0; i < len; i++)
    {
        buffer[2+i] = *(data_buff + i);
    }

    IoTI2cWrite(NFC_I2C_IDX, C081_NFC_ADDR & 0xFE, buffer, ARRAY_SIZE(buffer));

    return IOT_SUCCESS;
}
/*
* 函数名称 : write_fifo_reg
* 功能描述 : 写fifo寄存器
* 参    数 : reg_high_8bit_cmd  - 寄存器高字节
             reg_low_8bit_cmd   - 寄存器低字节
             data_buff          - 写入字节长度
* 返回值   : 写结果
* 示    例 : write_fifo_reg(reg_high_8bit_cmd,reg_low_8bit_cmd,data_buff);
*/
/******************************************************************************/ 
uint32 write_fifo_reg( uint8 reg_high_8bit_cmd, uint8 reg_low_8bit_cmd, uint8 data_buff)
/******************************************************************************/ 
{
    /* 写寄存器*/
    uint8 buffer[] = {reg_high_8bit_cmd, reg_low_8bit_cmd, data_buff};

    IoTI2cWrite(NFC_I2C_IDX, C081_NFC_ADDR & 0xFE, buffer, ARRAY_SIZE(buffer));

    return IOT_SUCCESS;
}
/*
* 函数名称 : write_fifo_data
* 功能描述 : 数据写入fifo
* 参    数 : data_buff  - 读写数据指针
             len        - 写入字节长度
* 返回值   : 写结果
* 示    例 : write_fifo_data(&data_buff,len);
*/
/******************************************************************************/ 
uint32 write_fifo_data( uint8* data_buff, uint8 len)
/******************************************************************************/ 
{
    /*写fifo data*/
    uint8 buffer[128] = {0};
    buffer[0] = 0xFF;
    buffer[1] = 0xF0;

    for (int32 i = 0; i < len; i++)
    {
        buffer[2+i] = *(data_buff+i);
    }

    IoTI2cWrite(NFC_I2C_IDX, C081_NFC_ADDR & 0xFE, buffer, 2 + len);

    return IOT_SUCCESS;
}
/*
* 函数名称 : eep_write_page
* 功能描述 : eeprom页写操作
* 参    数 : pBuffer     - 数据指针
            WriteAddr   - 写入地址
            len         - 写入字节长度
* 返回值   : 写结果
* 示    例 : eep_write_page(&data_buff,len);
*/
/******************************************************************************/ 
void eep_write_page(uint8 *pBuffer, uint16 WriteAddr, uint8 datalen)
/******************************************************************************/ 
{
    /*EEPROM page write*/
    uint32 status;
    status = c08i_nfc_i2c_write((uint8)((WriteAddr & 0xFF00) >> 8), (uint8)(WriteAddr & 0x00FF), pBuffer, datalen);
    usleep(10000);//必须延时10ms
    IoTGpioSetOutputVal(PIN_GPIO_NFC_CSN, 1);
    printf("----- eep_write_page %d : %s! -----\r\n\n", __LINE__,status == IOT_SUCCESS ? "SUCCESS!" : "FAILURE!");
}
/*
* 函数名称 : fm11_write_eep
* 功能描述 : 写eeprom
* 参    数 : wbuf   - 数据指针
            addr    - 写入地址
            len     - 写入字节长度
* 返回值   : 写结果
* 示    例 : fm11_write_eep(&data_buff,len);
*/
/******************************************************************************/ 
void fm11_write_eep(uint16 addr,uint32 len,uint8 *wbuf)
/******************************************************************************/ 
{
    /*写EEPROM*/
    uint8 offset;

    if (addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MANUF_ADDR) 
    {
        return;
    }

    if (addr % FM11_E2_BLOCK_SIZE) 
    {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        if (len > offset) 
        {
            eep_write_page(wbuf,addr,offset);

            addr += offset;
            wbuf += offset;
            len -= offset;
        } 
        else 
        {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
     }
    while (len) 
    {
        if (len >= FM11_E2_BLOCK_SIZE) 
        {
            eep_write_page(wbuf,addr,FM11_E2_BLOCK_SIZE);
            addr += FM11_E2_BLOCK_SIZE;
            wbuf += FM11_E2_BLOCK_SIZE;
            len -= FM11_E2_BLOCK_SIZE;
        } 
        else 
        {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
    }
}
/*
* 函数名称 : nfc_init
* 功能描述 : NFC 芯片配置 ,平时不要调用 
* 参    数 : 无
* 返回值   : 无
* 示    例 : nfc_init(&data_buff,len);
*/
/******************************************************************************/ 
void nfc_init(void)
/******************************************************************************/ 
{
    /* NFC 芯片配置 ,平时不要调用 NFC init*/
    uint8 wbuf[5]={0x05,0x78,0xF7,0x90,0x02};   //芯片默认配置

    /*读取字节的时候屏蔽csn引脚,写eep的时候打开*/
    IoTGpioInit(PIN_GPIO_NFC_CSN);
    IoTGpioSetDir(PIN_GPIO_NFC_CSN, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(PIN_GPIO_NFC_CSN, 0);

    fm11_write_eep(0x3B1,1,&wbuf[1]);
    fm11_write_eep(0x3B5,1,&wbuf[3]);
}
/*
* 函数名称 : fm11_read_eep
* 功能描述 : 读取eeprom
* 参    数 : dataBuff   - 数据指针
             ReadAddr   - 读取地址
             len        - 数据长度
* 返回值   : 读取结果
* 示    例 : fm11_read_eep(&dataBuff,ReadAddr,len);
*/
/******************************************************************************/ 
uint32 fm11_read_eep(uint8 *dataBuff, uint16 ReadAddr, uint16 len)
/******************************************************************************/ 
{
    /*读EEPROM*/
    write_read((uint8)((ReadAddr & 0xFF00)>>8), (uint8)(ReadAddr & 0x00FF), dataBuff, 2, len);

    return IOT_SUCCESS;
}
/*
* 函数名称 : fm11_read_reg
* 功能描述 : 读寄存器
* 参    数 : addr   - 读取地址
* 返回值   : 读取结果
* 示    例 : fm11_read_reg(addr);
*/
/******************************************************************************/ 
uint8 fm11_read_reg(uint16 addr)
/******************************************************************************/ 
{
    /*读NFC寄存器*/
    uint8 pdata[10] ={0};
    uint8 a =0;

    if (fm11_read_eep(pdata, addr, 1) == IOT_SUCCESS) 
    {
        a=pdata[0];
        //printf("\r\n========a=[%s]=======\r\n",a);
        return a;
    } 
    else 
    {
        // TODO: 无效的返回值
        return IOT_FAILURE;
    }
}
/*
* 函数名称 : fm11_write_reg
* 功能描述 : 写寄存器
* 参    数 : addr   - 写地址
             data   - 数据内容
* 返回值   : 写结果
* 示    例 : fm11_write_reg(addr,data);
*/
/******************************************************************************/ 
uint8 fm11_write_reg(uint16 addr, uint8 data)
/******************************************************************************/
{
    /*写NFC寄存器*/
    uint32 status =0;

    status = write_fifo_reg((uint8)((addr & 0xFF00) >> 8), (uint8)(addr & 0x00FF), data);
    if (status != IOT_SUCCESS) 
    {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}
/*
* 函数名称 : fm11_read_fifo
* 功能描述 : 读fifo内容
* 参    数 : NumByteToRead  - 读取字节长度
             pbuf           - 数据指针
* 返回值   : 读结果
* 示    例 : fm11_read_fifo(NumByteToRead,&pbuf);
*/
/******************************************************************************/ 
uint8  fm11_read_fifo(uint8 NumByteToRead, uint8 *pbuf)
/******************************************************************************/ 
{
    /*读取FIFO*/
    uint8 read_fifo_len = NumByteToRead;

    if (fm11_read_eep(pbuf, FM327_FIFO, read_fifo_len)) 
    {
        return IOT_SUCCESS;
    } 
    else 
    {
        return IOT_FAILURE;
    }
}
/*
* 函数名称 : fm11_write_fifo
* 功能描述 : 写fifo
* 参    数 : len    - 写入字节长度
             pbuf   - 数据指针
* 返回值   : 读结果
* 示    例 : fm11_write_fifo(NumByteToRead,&pbuf);
*/
/******************************************************************************/ 
uint8 fm11_write_fifo(uint8 *pbuf, uint8 len)
/******************************************************************************/ 
{
    /*写FIFO*/
    uint8 status =0;

    if (pbuf == NULL) {
        return -1;
    }
    status = write_fifo_data(pbuf, len);
    if (status != IOT_SUCCESS) 
    {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}
/*
* 函数名称 : fm11_data_send
* 功能描述 : 数据回发
* 参    数 : ilen   - 发送字节长度
             ibuf   - 数据指针
* 返回值   : 无
* 示    例 : fm11_data_send(ilen,&ibuf);
*/
/******************************************************************************/ 
void fm11_data_send(uint32 ilen,uint8 *ibuf)
/******************************************************************************/ 
{
    /*数据回发*/
    uint32 slen =0;
    uint8 *sbuf = NULL;

    if (ibuf == NULL) 
    {
        return;
    }
    slen = ilen;
    sbuf = &ibuf[0];

    if (slen <= 32) 
    {
        fm11_write_fifo(sbuf,slen);//write fifo 有多少发多少
        slen = 0;
        fm11_write_reg(RF_TXEN_REG,0x55);   //写0x55时触发非接触口回发数据
        // printf("\r\n slen<=32 \r\n");
    } 
    else 
    {
        // printf("\r\n slen>32 \r\n");
        fm11_write_fifo(sbuf,32);//write fifo    先发32字节进fifo
        fm11_write_reg(RF_TXEN_REG,0x55);   //写0x55时触发非接触口回发数据

        slen = slen - 32;//待发长度－32
        sbuf = sbuf + 32;//待发数据指针+32

        while (slen>0) 
        {
            if ((fm11_read_reg(FIFO_WORDCNT_REG) & 0x3F )<=8) 
            {
                if (slen<=24) 
                {
                    fm11_write_fifo(sbuf,slen);//write fifo 先发32字节进fifo
                    slen = 0;
                }
                else 
                {
                    fm11_write_fifo(sbuf,24);           //write fifo    先发32字节进fifo
                    slen = slen - 24;   //待发长度－24
                    sbuf = sbuf + 24;   //待发数据指针+24
                }
            }
            //fm11_write_reg(RF_TXEN_REG,0x55);   //写0x55时触发非接触口回发数据

        }
        irq_txdone = 0;
    }
    // printf("\r\n send data = ");
    // for(uint16_t i = 0; i<ilen ; i++)
    // {
    //     printf("%02x ",ibuf[i]);
    // }
    // printf("\r\n");
}
/*
* 函数名称 : fm11_data_recv
* 功能描述 : 数据接收
* 参    数 : rbuf   - 接收数据指针
* 返回值   : 无
* 示    例 : fm11_data_recv(&rbuf);
*/
/******************************************************************************/ 
uint32 fm11_data_recv(uint8 *rbuf)
/******************************************************************************/ 
{
    /*读取RF数据*/
    uint8 irq =0;
    uint8 ret =0;
    uint8 irq_data_wl = 0;
    uint8 irq_data_in = 0;
    uint32 rlen =0;
    uint32 temp =0;
#ifdef CHECK
    /*查询方式*/
    while (1) {
        irq_data_wl = 0;
        irq = fm11_read_reg(MAIN_IRQ);              //查询中断标志
        if(irq)
        {
            
            // printf("\r\n MAIN_IRQ = %X\r\n",irq);
            if (irq & MAIN_IRQ_FIFO)                    //判断是否有FIFO中断
            {      
                ret=fm11_read_reg(FIFO_IRQ);            //读取FIFO中断寄存器
                if(ret & FIFO_IRQ_WL)                   //判断FIFO数据是否发生变化
                irq_data_wl = 1;
                // printf("\r\n 1:FIFO_IRQ = %2X\r\n",ret);
            }
            if (irq & MAIN_IRQ_AUX) {                   
                ret = fm11_read_reg(AUX_IRQ);
                fm11_write_reg(FIFO_FLUSH,0xFF);        //复位FIFO寄存器？？？
                // printf("\r\n 2:AUX_IRQ = %2X\r\n",ret);
            }

            if (irq& MAIN_IRQ_RX_START)                 //判断是否有数据开始接收     
            {               
                irq_data_in = 1;
                // printf("\r\n 2:MAIN_IRQ_RX_START \r\n");
            }

            if (irq_data_in && irq_data_wl) 
            {
                irq_data_wl =0;
                fm11_read_fifo(24,&rbuf[rlen]);         //渐满之后读取24字节
                rlen += 24;
                // printf("\r\n 3:reading fifo \r\n");
            }

            if (irq & MAIN_IRQ_RX_DONE) 
            {
                temp =(uint32)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);    //接收完全之后，查fifo有多少字节，FIFO_WORDCNT寄存器取值范围0~32，所以&0x3F
                fm11_read_fifo(temp,&rbuf[rlen]);       //读最后的数据
                rlen += temp;
                irq_data_in = 0;
                // printf("\r\n 4:recv len = %d",rfLen);

                if(rlen!=0)
                break;
            }
            // printf("\r\n return \r\n");
        }
        usleep(1000);
    }
#endif

#ifdef INTERRUPT
    while (1) {
        irq_data_wl = 0;
        irq = fm11_read_reg(MAIN_IRQ);//查询中断标志

        if (irq & MAIN_IRQ_FIFO) {
            ret=fm11_read_reg(FIFO_IRQ);
            if(ret & FIFO_IRQ_WL)
            irq_data_wl = 1;
        }
        if (irq & MAIN_IRQ_AUX) {
            fm11_read_reg(AUX_IRQ);
            fm11_write_reg(FIFO_FLUSH,0xFF);
        }

        if (irq& MAIN_IRQ_RX_START) {

          irq_data_in = 1;
        }

         if (irq_data_in && irq_data_wl) {
            irq_data_wl =0;
            fm11_read_fifo(24,&rbuf[rlen]);//渐满之后读取24字节
            rlen += 24;
        }

        if (irq & MAIN_IRQ_RX_DONE) {
            temp =(uint32)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);    //接收完全之后，查fifo有多少字节
            fm11_read_fifo(temp,&rbuf[rlen]);       //读最后的数据
            rlen += temp;
            irq_data_in = 0;
            break;
        }
        usleep(1000);
    }
#endif
    // if(rfLen > 0) {
    //     printf("\r\n recv data = ");
    //     for(uint16_t i = 0; i<rfLen ; i++)
    //     {
    //         printf("%02x ",rbuf[i]);
    //     }
    //     printf("\r\n");
    // }
    
    if (rlen <= 2) {
        return 0;
    }
    rlen -= 2;//2字节crc校验
    return rlen;
}
/*
* 函数名称 : fm11_t4t
* 功能描述 : 数据回发
* 参    数 : 无
* 返回值   : 无
* 示    例 : fm11_t4t();
*/
/******************************************************************************/ 
void fm11_t4t(void)
/******************************************************************************/ 
{
    /* 写fifo 和 写寄存器*/
    uint8 ret =0;
    uint8 nak_crc_err = 0x05;
    uint8 status_ok[3] = { 0x02, 0x90, 0x00 };
    uint8 status_word[3] = { 0x02, 0x6A, 0x82 };
    uint8 status_word2[3] = { 0x02, 0x6A, 0x00 };
    uint8 crc_err = 0;
    const uint8 ndef_capability_container[2] = { 0xE1, 0x03 };
    const uint8 ndef_id[2] = { 0xE1, 0x04 };
    uint8 xlen =0;
    uint8 xbuf[256] = {0};

    if (crc_err) 
    {
        fm11_write_fifo(&nak_crc_err, 1);
        fm11_write_reg(RF_TXEN_REG, 0x55);
        crc_err = 0;
        printf("\r\n write RF_TXEN_REG \r\n");
    } 
    else 
    {
        status_ok[0] = fm327_fifo[0];
        status_word[0] = fm327_fifo[0];
        status_word2[0] = fm327_fifo[0];
        // select apdu
        if (fm327_fifo[INS] == 0xA4) 
        {
            printf("\r\n select apdu \r\n");
            // printf("\r\n fm327_fifo = ");
            for(uint16_t i = 0; i<((DATA)+ fm327_fifo[LC]); i++)
            {
                printf("%02x ",fm327_fifo[i]);
            }
            printf("\r\n");
            if (fm327_fifo[P1] == 0x00)
            {

                if ((fm327_fifo[LC] == sizeof(ndef_capability_container)) && (0 == memcmp(ndef_capability_container, fm327_fifo + DATA, fm327_fifo[LC]))) 
                {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = CC_FILE;
                    // printf("\r\n CC_FILE \r\n");
                } 
                else if ((fm327_fifo[LC] == sizeof(ndef_id)) && (0 == memcmp(ndef_id, fm327_fifo + DATA, fm327_fifo[LC]))) 
                {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NDEF_FILE;
                    // printf("\r\n NDEF_FILE \r\n");
                } 
                else 
                {
                    fm11_write_fifo(status_word2, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NONE;
                    // printf("\r\n NONE_FILE \r\n");
                }
            } 
            else if (fm327_fifo[P1] == 0x04) 
            {
                ret = fm11_write_fifo(status_ok, 3);
                if (ret != HI_ERR_SUCCESS) 
                {
                    printf("fm11_write_reg failed\r\n");
                }
                ret = fm11_write_reg(RF_TXEN_REG, 0x55);
                if (ret != HI_ERR_SUCCESS) 
                {
                    printf("fm11_write_reg failed\r\n");
                }
            } 
            else 
            {
                fm11_write_fifo(status_ok, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
                // printf("\r\n status_ok \r\n");
            }
        } 
        else if (fm327_fifo[INS] == 0xB0)               //"B0"读取二进制数据，read binary
        {
            printf("\r\n READ_BINARY \r\n");
            if (current_file == CC_FILE) 
            {
                fm11_write_fifo(status_ok, 1);
                fm11_write_fifo(capability_container + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                fm11_write_fifo(&status_ok[1], 2);
                fm11_write_reg(RF_TXEN_REG, 0x55);
                // printf("\r\n write CC_FILE\r\n", xlen);
            } 
            else if (current_file == NDEF_FILE) 
            {
                memcpy(&xbuf[0], &status_ok[0], 1);
                memcpy(&xbuf[1], &ndef_file[0] + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                memcpy(&xbuf[0]+fm327_fifo[LC]+1, status_ok+1, 2);
                xlen=fm327_fifo[LC]+3;
                
                fm11_data_send(xlen, xbuf);
                // printf("\r\n write NDEF_FILE\r\n", xlen);
            } 
            else 
            {
                fm11_write_fifo(status_word, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            }
        } 
        else if (fm327_fifo[INS] ==  0xD6)          // UPDATE_BINARY
        { 
            printf("\r\n UPDATE_BINARY \r\n");
            for (uint8_t i=0;i<rfLen;i++) 
            {
                printf("0x%02x ",fm327_fifo[i]);
            }
            printf("\r\n");
            memcpy(ndef_file + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo + DATA, fm327_fifo[LC]);
            fm11_write_fifo(status_ok, 3);
            fm11_write_reg(RF_TXEN_REG, 0x55);
            
        } 
        else 
        {
            fm11_data_send(rfLen, fm327_fifo);
        }
    }
}
/*
* 函数名称 : nfc_init_mutex
* 功能描述 : 创建互斥锁
* 参    数 : 无
* 返回值   : 无
* 示    例 : nfc_init_mutex();
*/
/******************************************************************************/ 
static void nfc_init_mutex(void)
/******************************************************************************/ 
{
    hi_mux_create(&s_Nfc_Mux_Id);
}
/*
* 函数名称 : nfc_thread_lock
* 功能描述 : 上锁
* 参    数 : 无
* 返回值   : 无
* 示    例 : nfc_thread_lock();
*/
/******************************************************************************/ 
static void nfc_thread_lock(void)
/******************************************************************************/ 
{
    hi_mux_pend(s_Nfc_Mux_Id, HI_SYS_WAIT_FOREVER);
}
/*
* 函数名称 : nfc_thread_unlock
* 功能描述 : 解锁
* 参    数 : 无
* 返回值   : 无
* 示    例 : nfc_thread_unlock();
*/
/******************************************************************************/ 
static void nfc_thread_unlock(void)
/******************************************************************************/ 
{
    hi_mux_post(s_Nfc_Mux_Id);
}
/*
* 函数名称 : NfcTask
* 功能描述 : nfc任务
* 参    数 : arg - 任务参数
* 返回值   : 无
* 示    例 : NfcTask(&arg);
*/
/******************************************************************************/ 
void NfcTask(void *arg)
/******************************************************************************/ 
{
    (void)arg;

    IoTI2cInit(NFC_I2C_IDX, NFC_I2C_BAUDRATE);
    nfc_init_mutex();

#ifdef  CHECK
    while (1)
    {
        nfc_thread_lock();
        rfLen = fm11_data_recv(fm327_fifo); //读取rf数据(一帧)
        if (rfLen > 0)
        {
            fm11_t4t();
            irq_data_in = 0;
        }
        nfc_thread_unlock();
        // usleep(1000*10);
        usleep(1);
    }
#endif

#ifdef INTERRUPT
    while (1)
    {
        if (FlagFirstFrame)
        {
            rfLen = fm11_data_recv(fm327_fifo); //读取rf数据(一帧)
            if (rfLen > 0)
            {
                fm11_t4t();
            }
            irq_data_in = 0;
        }
        usleep(1000);
    }
#endif
}
/*
* 函数名称 : NfcDemo
* 功能描述 : nfc应用demo
* 参    数 : 无
* 返回值   : 无
* 示    例 : NfcDemo();
*/
/******************************************************************************/ 
void NfcDemo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "NfcTask";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = NFC_TASK_STACK_SIZE;
    attr.priority   = NFC_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)NfcTask, NULL, &attr) == NULL) {
        printf("\r\n[NfcDemo] Falied to create NfcTask!\n");
    }
    printf("\r\n[NfcDemo] Succ to create NfcTask!\n");
}

APP_FEATURE_INIT(NfcDemo);
/******************************* End of File (C) ******************************/