/*
 * Copyright (c) 2021 Chinasoft International Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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


#define NFC_TASK_STACK_SIZE     4096
#define NFC_TASK_PRIO           25

#define NFC_I2C_IDX             0
#define NFC_I2C_BAUDRATE        (400*1000)
#define PIN_GPIO_NFC_CSN        9
#define PIN_GPIO_NFC_IRQ        10



T4T_FILE current_file;

uint8 capability_container[15] =
{   0x00, 0x0F,        //CCLEN
    0x20,              //Mapping Version
    0x00, 0xF6,        //MLe 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x00, 0xF6,        //MLc 必须是F6  写成FF超过256字节就会分帧  但是写成F6就不会分帧
    0x04,              //NDEF消息格式 05的话就是私有
    0x06,              //NDEF消息长度
    0xE1, 0x04,        //NDEF FILE ID       NDEF的文件标识符
    0x03, 0x84,        //NDEF最大长度
    0x00,              //Read Access           可读
    0x00               //Write Access          可写
};

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
    // 0x00,0x20,
    // 0xd4, 0x0f, 0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    // 0x69, 0x64, 0x2e, 0x63, 0x6F, 0x6D, 0x2E, 0x63, 
    // 0x68, 0x69, 0x6E, 0x61, 0x73, 0x6F, 0x66, 0x74, 
    // 0x69, 0x6E, 0x63, 0x2E, 0x6E, 0x66, 0x63, 0x64, 
    // 0x65, 0x6D, 0x6F,


    // 
    // 0x00,0x20,
    // 0xd4, 0x0f,0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    // 0x69, 0x64,0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    // 0x6b, 0x67,0x63, 0x6F, 0x6D, 0x2E, 0x73, 0x69, 
    // 0x6E, 0x61,0x2E, 0x77, 0x65, 0x69, 0x62, 0x6F,

    0x00, 0x20,
    0xd4, 0x0f, 0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    0x6b, 0x67, 0x63, 0x6F, 0x6D, 0x2E, 0x63, 0x6E,
    0x73, 0x69, 0x6E, 0x63, 0x2E, 0x6E, 0x66, 0x63,
    // 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
    // 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 
#endif
};

uint8 fm327_fifo[1024];
uint8 irq_data_in = 0;//非接数据接收终端标识
uint8 irq_txdone = 0;
uint8 rfLen;
uint8 irq_rxdone = 0;
uint8 irq_data_wl =0;
uint8 FlagFirstFrame = 0; //卡片首帧标识

uint32 g_c08i_nfc_demo_task_id = 0;
uint32 g_nfc_display_task_id = 0;
//extern uint32 g_mux_id;
uint8 read_reg =0;

/*i2c read*/
uint32 write_read(uint8 reg_high_8bit_cmd, uint8 reg_low_8bit_cmd, uint8* recv_data, uint8 send_len, uint8 read_len)
{
    uint8 buffer[] = {reg_high_8bit_cmd, reg_low_8bit_cmd};
	
    memset_s(recv_data, read_len, 0x0, read_len);
    
    read_reg = NFC_CLEAN;   // 消除stop信号



    IoTI2cWrite(NFC_I2C_IDX, C081_NFC_ADDR & 0xFE, buffer, ARRAY_SIZE(buffer));    
    IoTI2cRead(NFC_I2C_IDX, C081_NFC_ADDR | I2C_RD, recv_data, read_len);
    return IOT_SUCCESS;
}

/*co8i 写命令: 该接口写eeprom 更改芯片配置*/
uint32 c08i_nfc_i2c_write( uint8 reg_high_8bit_cmd, uint8 reg_low_8bit_cmd, uint8* data_buff, uint8 len)
{
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

/* 写寄存器*/
uint32 write_fifo_reg( uint8 reg_high_8bit_cmd, uint8 reg_low_8bit_cmd, uint8 data_buff)
{
    uint8 buffer[] = {reg_high_8bit_cmd, reg_low_8bit_cmd, data_buff};

    IoTI2cWrite(NFC_I2C_IDX, C081_NFC_ADDR & 0xFE, buffer, ARRAY_SIZE(buffer));

    return IOT_SUCCESS;
}

/*写fifo data*/
uint32 write_fifo_data( uint8* data_buff, uint8 len)
{
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

/*EEPROM page write*/
void eep_write_page(uint8 *pBuffer, uint16 WriteAddr, uint8 datalen)
{
    uint32 status;
    status = c08i_nfc_i2c_write((uint8)((WriteAddr & 0xFF00) >> 8), (uint8)(WriteAddr & 0x00FF), pBuffer, datalen);
    usleep(10000);//必须延时10ms
    IoTGpioSetOutputVal(PIN_GPIO_NFC_CSN, 1);
    printf("----- eep_write_page %d : %s! -----\r\n\n", __LINE__,status == IOT_SUCCESS ? "SUCCESS!" : "FAILURE!");
}

/*写EEPROM*/
void fm11_write_eep(uint16 addr,uint32 len,uint8 *wbuf)
{
    uint8 offset;

    if (addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MANUF_ADDR) {
        return;
    }

    if (addr % FM11_E2_BLOCK_SIZE) {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        if (len > offset) {
            eep_write_page(wbuf,addr,offset);

            addr += offset;
            wbuf += offset;
            len -= offset;
        } else {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
     }
    while (len) {
        if (len >= FM11_E2_BLOCK_SIZE) {
            eep_write_page(wbuf,addr,FM11_E2_BLOCK_SIZE);
            addr += FM11_E2_BLOCK_SIZE;
            wbuf += FM11_E2_BLOCK_SIZE;
            len -= FM11_E2_BLOCK_SIZE;
        } else {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
    }
}

/* NFC 芯片配置 ,平时不要调用 NFC init*/
void nfc_init(void)
{
    uint8 wbuf[5]={0x05,0x78,0xF7,0x90,0x02};   //芯片默认配置

    /*读取字节的时候屏蔽csn引脚,写eep的时候打开*/
    IoTGpioInit(PIN_GPIO_NFC_CSN);
    IoTGpioSetDir(PIN_GPIO_NFC_CSN, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(PIN_GPIO_NFC_CSN, 0);

    fm11_write_eep(0x3B1,1,&wbuf[1]);
    fm11_write_eep(0x3B5,1,&wbuf[3]);
}

/*读EEPROM*/
uint32 fm11_read_eep(uint8 *dataBuff, uint16 ReadAddr, uint16 len)
{
    write_read((uint8)((ReadAddr & 0xFF00)>>8), (uint8)(ReadAddr & 0x00FF), dataBuff, 2, len);

    return IOT_SUCCESS;
}

/*读NFC寄存器*/
uint8 fm11_read_reg(uint16 addr)
{
    uint8 pdata[10] ={0};
    uint8 a =0;

    if (fm11_read_eep(pdata, addr, 1) == IOT_SUCCESS) {
        a=pdata[0];
        //printf("\r\n========a=[%s]=======\r\n",a);
        return a;
    } else {
        // TODO: 无效的返回值
        return IOT_FAILURE;
    }
}

/*写NFC寄存器*/
uint8 fm11_write_reg(uint16 addr, uint8 data)
{
    uint32 status =0;

    status = write_fifo_reg((uint8)((addr & 0xFF00) >> 8), (uint8)(addr & 0x00FF), data);
    if (status != IOT_SUCCESS) {
        return IOT_FAILURE;
    }
    return IOT_SUCCESS;
}
/*读取FIFO*/
uint8  fm11_read_fifo(uint8 NumByteToRead, uint8 *pbuf)
{
    uint8 read_fifo_len = NumByteToRead;

    if (fm11_read_eep(pbuf, FM327_FIFO, read_fifo_len)) {
        return IOT_SUCCESS;
    } else {
        return IOT_FAILURE;
    }
}
/*写FIFO*/
uint8 fm11_write_fifo(uint8 *pbuf, uint8 len)
{
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

/*数据回发*/
void fm11_data_send(uint32 ilen,uint8 *ibuf)
{
    uint32 slen =0;
    uint8 *sbuf = NULL;

    if (ibuf == NULL) 
    {
        return;
    }
    slen = ilen;
    sbuf = &ibuf[0];

    if (slen <= 32) {
        fm11_write_fifo(sbuf,slen);//write fifo 有多少发多少
        slen = 0;
        fm11_write_reg(RF_TXEN_REG,0x55);   //写0x55时触发非接触口回发数据
    } else {
        fm11_write_fifo(sbuf,32);//write fifo    先发32字节进fifo
        fm11_write_reg(RF_TXEN_REG,0x55);   //写0x55时触发非接触口回发数据

        slen = slen - 32;//待发长度－32
        sbuf = sbuf + 32;//待发数据指针+32

        while (slen>0) {
            if ((fm11_read_reg(FIFO_WORDCNT_REG) & 0x3F )<=8) {
                if (slen<=24) {
                    fm11_write_fifo(sbuf,slen);//write fifo 先发32字节进fifo
                    slen = 0;
                } else {
                    fm11_write_fifo(sbuf,24);           //write fifo    先发32字节进fifo
                    slen = slen - 24;   //待发长度－24
                    sbuf = sbuf + 24;   //待发数据指针+24
                }
            }
        }
        irq_txdone = 0;
    }
    printf("\r\n send data = ");
    for(uint16_t i = 0; i<ilen ; i++)
    {
        printf("%02x ",ibuf[i]);
    }
    printf("\r\n");
}

/*读取RF数据*/
uint32 fm11_data_recv(uint8 *rbuf)
{
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
        // printf("\r\n MAIN_IRQ = %X\r\n",irq);
        if (irq & MAIN_IRQ_FIFO)                    //判断是否有FIFO中断
        {      
            // printf("\r\n======1==========\r\n");
            ret=fm11_read_reg(FIFO_IRQ);            //读取FIFO中断寄存器
            if(ret & FIFO_IRQ_WL)                   //判断FIFO数据是否发生变化
            irq_data_wl = 1;
            // printf("\r\n 1:FIFO_IRQ = %X\r\n",ret);
        }
        if (irq & MAIN_IRQ_AUX) {                   
            // printf("\r\n======2==========\r\n");
            ret = fm11_read_reg(AUX_IRQ);
            fm11_write_reg(FIFO_FLUSH,0xFF);        //复位FIFO寄存器？？？
            // printf("\r\n 2:AUX_IRQ = %X\r\n",ret);
        }

        if (irq& MAIN_IRQ_RX_START)                 //判断是否有数据开始接收     
        {               
            // printf("\r\n======2==========\r\n");
            irq_data_in = 1;
            // printf("\r\n 2:MAIN_IRQ_RX_START \r\n");
        }

        if (irq_data_in && irq_data_wl) 
        {
            // printf("\r\n======3==========\r\n");
            irq_data_wl =0;
            fm11_read_fifo(24,&rbuf[rlen]);         //渐满之后读取24字节
            rlen += 24;
            // printf("\r\n 3:read fifo \r\n");
        }

        if (irq & MAIN_IRQ_RX_DONE) 
        {
            // printf("\r\n======4==========\r\n");
            // printf("\r\n 4、 read fifo r\n");

            temp =(uint32)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);    //接收完全之后，查fifo有多少字节，FIFO_WORDCNT寄存器取值范围0~32，所以&0x3F
            fm11_read_fifo(temp,&rbuf[rlen]);       //读最后的数据
            rlen += temp;
            irq_data_in = 0;
            // printf("\r\n 4:recv len = %d",rfLen);
            // printf("\r\n recv data = ");
            // for(uint16_t i = 0; i<rfLen ; i++)
            // {
            // 	printf("%02x ",fm327_fifo[i]);
            // }
            if(rlen!=0)
            break;
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
    if(rfLen > 0) {
        printf("\r\n recv data = ");
        for(uint16_t i = 0; i<rfLen ; i++)
        {
            printf("%02x ",fm327_fifo[i]);
        }
        printf("\r\n");
    }
    
    if (rlen <= 2) {
        return 0;
    }
    rlen -= 2;//2字节crc校验
    return rlen;
}

/* 写fifo 和 写寄存器*/
void fm11_t4t(void)
{
    uint8 ret =0;
    uint8 nak_crc_err = 0x05;
    uint8 status_ok[3] = { 0x02, 0x90, 0x00 };
    uint8 status_word[3] = { 0x02, 0x6A, 0x82 };
    uint8 status_word2[3] = { 0x02, 0x6A, 0x00 };
    uint8 crc_err = 0;
    const uint8 ndef_capability_container[2] = { 0xE1, 0x03 };
    const uint8 ndef_id[2] = { 0xE1, 0x04 };
    uint8 i =0;
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
            if (fm327_fifo[P1] == 0x00)
            {
                if ((fm327_fifo[LC] == sizeof(ndef_capability_container)) && (0 == memcmp(ndef_capability_container, fm327_fifo + DATA, fm327_fifo[LC]))) 
                {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = CC_FILE;
                } 
                else if ((fm327_fifo[LC] == sizeof(ndef_id)) && (0 == memcmp(ndef_id, fm327_fifo + DATA, fm327_fifo[LC]))) 
                {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NDEF_FILE;
                } 
                else 
                {
                    fm11_write_fifo(status_word2, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NONE;
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
            }
        } 
        else if (fm327_fifo[INS] == 0xB0)               //"B0"读取二进制数据，read binary
        {
            if (current_file == CC_FILE) 
            {
                fm11_write_fifo(status_ok, 1);
                fm11_write_fifo(capability_container + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                fm11_write_fifo(&status_ok[1], 2);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            } 
            else if (current_file == NDEF_FILE) 
            {
                memcpy(&xbuf[0], &status_ok[0], 1);
                memcpy(&xbuf[1], &ndef_file[0] + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                memcpy(&xbuf[0]+fm327_fifo[LC]+1, status_ok+1, 2);
                xlen=fm327_fifo[LC]+3;
                // printf("xlen %d\r\n", xlen);
                
                fm11_data_send(xlen, xbuf);
            } 
            else 
            {
                fm11_write_fifo(status_word, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            }
        } 
        else if (fm327_fifo[INS] ==  0xD6)          // UPDATE_BINARY
        { 
            for (i=0;i<rfLen;i++) 
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


void NfcTask(void *arg)
{
    (void)arg;

    IoTI2cInit(NFC_I2C_IDX, NFC_I2C_BAUDRATE);

    // IoTGpioInit(PIN_GPIO_NFC_CSN);
    // IoTGpioSetDir(PIN_GPIO_NFC_CSN, IOT_GPIO_DIR_OUT);
    // IoTGpioSetOutputVal(PIN_GPIO_NFC_CSN, 1);
    // usleep(1000);

#ifdef  CHECK
    while (1)
    {
        rfLen = fm11_data_recv(fm327_fifo); //读取rf数据(一帧)
        if (rfLen > 0)
        {
            fm11_t4t();
            irq_data_in = 0;
        }
        usleep(1000*10);
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

void NfcDemo(void)
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