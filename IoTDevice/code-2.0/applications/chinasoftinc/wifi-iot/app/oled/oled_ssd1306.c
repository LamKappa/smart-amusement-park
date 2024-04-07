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

#include "ohos_types.h"
#include "iot_errno.h"
#include "iot_i2c.h"

#include "oled_fonts.h"
#include "oled_ssd1306.h"


#define OLED_I2C_IDX        0
#define OLED_I2C_BAUDRATE   (400*1000)  // 400k
#define OLED_I2C_ADDR       0x78        // 默认地址为 0x78
#define OLED_I2C_CMD        0x00        // 0000 0000       写命令
#define OLED_I2C_DATA       0x40        // 0100 0000(0x40) 写数据
#define OLED_WIDTH          (128)



static uint32 OledWriteByte(uint8 regAddr, uint8 byte)
{
	// TODO: Check SSD1306 DataSheet if must write by byte.

    uint8 buffer[] = {regAddr, byte};

	return IoTI2cWrite(OLED_I2C_IDX, OLED_I2C_ADDR, buffer, ARRAY_SIZE(buffer));
}

/**
 * @brief Write a command byte to OLED device.
 *
 * @param cmd the commnad byte to be writen.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 */
static uint32 OledWriteCmd(uint8 cmd)
{
    return OledWriteByte(OLED_I2C_CMD, cmd);
}

/**
 * @brief Write a data byte to OLED device.
 *
 * @param cmd the data byte to be writen.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 */
static uint32 OledWriteData(uint8 data)
{
	return OledWriteByte(OLED_I2C_DATA, data);
}

/**
 * @brief ssd1306 OLED Initialize.
 */
uint32 OledInit(void)
{
    static const uint8 initCmds[] = {
        0xAE, // --display off
        0x00, // ---set low column address
        0x10, // ---set high column address
        0x40, // --set start line address
        0xB0, // --set page address
        0x81, // contract control
        0xFF, // --128
        0xA1, // set segment remap
        0xA6, // --normal / reverse
        0xA8, // --set multiplex ratio(1 to 64)
        0x3F, // --1/32 duty
        0xC8, // Com scan direction
        0xD3, // -set display offset
        0x00, //
        0xD5, // set osc division
        0x80, //
        0xD8, // set area color mode off
        0x05, //
        0xD9, // Set Pre-Charge Period
        0xF1, //
        0xDA, // set com pin configuartion
        0x12, //
        0xDB, // set Vcomh
        0x30, //
        0x8D, // set charge pump enable
        0x14, //
        0xAF, // --turn on oled panel
    };

    IoTI2cInit(OLED_I2C_IDX, OLED_I2C_BAUDRATE);

	for (int32 i = 0; i < ARRAY_SIZE(initCmds); i++) {
        uint32 status = OledWriteCmd(initCmds[i]);
        if (status != IOT_SUCCESS) {
            return status;
        }
    }

    return IOT_SUCCESS;
}


void OledSetPosition(uint8 x, uint8 y)
{
    OledWriteCmd(0xb0 + y);
    OledWriteCmd(((x & 0xf0) >> 4) | 0x10);
    OledWriteCmd(x & 0x0f);
}


void OledFillScreen(uint8 fillData)
{
    uint8 m = 0;
    uint8 n = 0;

    for (m=0; m < 8; m++) {
        OledWriteCmd(0xb0 + m);
        OledWriteCmd(0x00);
        OledWriteCmd(0x10);

        for (n=0; n < 128; n++) {
            OledWriteData(fillData);
        }
    }
}

/**
 * @brief 8*16 typeface
 * @param x: write positon start from x axis 
 * @param y: write positon start from y axis
 * @param ch: write data
 * @param font: selected font
 */
void OledShowChar(uint8 x, uint8 y, uint8 ch, Font font)
{      	
	uint8 c = 0;
    uint8 i = 0;

    c = ch - ' '; //得到偏移后的值	
    if (x > OLED_WIDTH - 1) {
        x = 0;
        y = y + 2;
    }

    if (font == FONT8x16) {
        OledSetPosition(x, y);	
        for (i = 0; i < 8; i++){
            OledWriteData(F8X16[c*16 + i]);
        }

        OledSetPosition(x, y+1);
        for (i = 0; i < 8; i++) {
            OledWriteData(F8X16[c*16 + i + 8]);
        }
    } else {
        OledSetPosition(x, y);
        for (i = 0; i < 6; i++) {
            OledWriteData(F6x8[c][i]);
        }
    }
}


void OledShowString(uint8 x, uint8 y, const char* str, Font font)
{
	uint8 j = 0;
    if (str == NULL) {
        printf("param is NULL,Please check!!!\r\n");
        return;
    }

	while (str[j]) {
        OledShowChar(x, y, str[j], font);
		x += 8;
		if (x > 120) {
            x = 0;
            y += 2;
        }
		j++;
	}
}

//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void OLED_ShowNum(uint8 x, uint8 y, uint32 num, uint8 len, uint8 size)
{
    uint8 t, temp;
    uint8 enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                OLED_ShowChar(x + (size / 2)*t, y, ' ');
                continue;
            }
            else enshow = 1;
        }
        OLED_ShowChar(x + (size / 2)*t, y, temp + '0');
    }
}

