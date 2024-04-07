#ifndef __C081_NFC_H__
#define __C081_NFC_H__

#include <hi_i2c.h>

// #define INTERRUPT
#define CHECK

// #define HUAWEI_COM
// #define NFC_TAG_WECHAT
#define NFC_TAG_CHINASOFTINC

#define     NFC_I2C_REG_ARRAY_LEN           (32)  
#define     NFC_SEND_BUFF                   (3)
#define     C08I_NFC_DEMO_TASK_STAK_SIZE    (1024*10)
#define     NFC_DISPLAY_TASK_STAK_SIZE      (1024)
#define     C08I_NFC_DEMO_TASK_PRIORITY     (25)
#define     CLEAN_STOP_SIGNAL               (hi_u8(0x00))

#define CLA                                 (1)         //指令类别
#define INS                                 (2)         //指令代码
#define P1                                  (3)         //指令参数1
#define P2                                  (4)         //指令参数2 
#define LC                                  (5)         //指令
#define DATA                                (6)

#define C081_NFC_ADDR           0xAE // 7 bit slave device address  1010 111 0/1
#define I2C_WR                  0x00
#define I2C_RD                  0x01
#define C081_NFC_READ_ADDR      0xAF
#define C081NFC_WRITE_ADDR      (C081_NFC_ADDR|I2C_WR)
#define C081NFC_READ_ADDR       (C081_NFC_ADDR|I2C_RD)
#define FM11_E2_USER_ADDR       0x0010
#define FM11_E2_MANUF_ADDR      0x03FF
#define FM11_E2_BLOCK_SIZE      16

#define FM327_FIFO				0xFFF0              //读写 FIFO 的入口地址
#define FIFO_FLUSH_REG			0xFFF1              //MCU 清空 FIFO 内容寄存器  
#define	FIFO_WORDCNT_REG		0xFFF2              //MCU 查询 FIFO 剩余字节寄存器
#define RF_STATUS_REG			0xFFF3              //非接触口工作状态
#define RF_TXEN_REG				0xFFF4              //非接触口回发使能
#define RF_BAUD_REG				0xFFF5              //非接触口通信波特率选择
#define RF_RATS_REG				0xFFF6              //非接触口通信收到的 RATS 数据
#define MAIN_IRQ_REG			0xFFF7              //主中断标志寄存器
#define FIFO_IRQ_REG			0xFFF8              //FIFO 中断标志寄存器
#define AUX_IRQ_REG				0xFFF9              //辅助中断标志寄存器
#define MAIN_IRQ_MASK_REG		0xFFFA              //主中断屏蔽寄存器
#define FIFO_IRQ_MASK_REG		0xFFFB              //FIFO 中断屏蔽寄存器
#define AUX_IRQ_MASK_REG		0xFFFC              //辅助中断屏蔽寄存器
#define NFC_CFG_REG				0xFFFD              //NFC 配置寄存器
#define VOUT_CFG_REG			0xFFFE              //LDO_CFG 配置寄存器
#define EE_WR_CTRL_REG			0xFFFF              //

#define MAIN_IRQ				0xFFF7              //主中断标志寄存器
#define FIFO_IRQ				0xFFF8              //FIFO 中断标志寄存器
#define AUX_IRQ		    	    0xFFF9              //辅助中断标志寄存器
#define MAIN_IRQ_MASK		    0xFFFA              //主中断屏蔽寄存器
#define FIFO_IRQ_MASK		    0xFFFB              //FIFO 中断屏蔽寄存器
#define AUX_IRQ_MASK	        0xFFFC              //辅助中断屏蔽寄存器
#define FIFO_FLUSH			    0xFFF1              //MCU 清空 FIFO 内容寄存器
#define	FIFO_WORDCNT		    0xFFF2              //MCU 查询 FIFO 剩余字节寄存器

#define MAIN_IRQ_RF_PWON        0x80                //射频上电中断标志，芯片进场后置位 
#define MAIN_IRQ_ACTIVE         0x40                //ISO14443-3 选卡（Select）完成中断标志
#define MAIN_IRQ_RX_START       0x20                //ISO14443-3 选卡（Select）完成后开始接收数据，FIFO 从空到不空
#define MAIN_IRQ_RX_DONE        0x10                //当前数据帧接收完成
#define MAIN_IRQ_TX_DONE        0x08                //当前数据帧回发完成
#define MAIN_IRQ_ARBIT          0x04                //仲裁冲突中断，当非接触口访问 EEPROM 时，如果接触口试图访问 EEPROM 则触发中断
#define MAIN_IRQ_FIFO           0x02                //FIFO 中断标志，此位置1表示有 FIFO 中断产生，MCU 应去查询 FIFO 中断标志寄存器
#define MAIN_IRQ_AUX            0x01                //辅助中断标志，此位置1表示有辅助中断产生，MCU 应去查询辅助中断标志寄存器,此位表示 AUX_IRQ 各 bit 的或逻辑结果。
#define FIFO_IRQ_WL             0x08                //FIFO 水平线中断：接收时当 FIFO 内数据增加到 24字节时触发中断（渐满）；发送时当 FIFO 数据减少到 8 字节时触发中断（渐空）

typedef enum {
    NONE, 
    CC_FILE,
    NDEF_FILE 
} T4T_FILE;

typedef enum{
    NFC_RECOVERY =0,
    NFC_CLEAN
}nfc_clean_stop_signal;

extern hi_u8 irq_data_in;
extern hi_u8 irq_rxdone;
extern hi_u8 irq_txdone;
extern hi_u8 FlagFirstFrame;
extern hi_u8 isr_flag;
extern hi_u8 g_menu_mode;
extern hi_u8  g_current_mode;
extern hi_u8  g_current_type;
extern hi_u8 g_menu_select;

hi_void *app_nfc_display(hi_void* param);
hi_void app_nfc_display_task(hi_void);
hi_void oled_nfc_display(hi_void);
hi_u32 c08i_nfc_i2c_write( hi_u8 reg_high_8bit_cmd, hi_u8 reg_low_8bit_cmd, hi_u8* data_buff, hi_u8 len);

void gpio_isr_handle(void *arg);
hi_void fm11_t4t(hi_void);
hi_u32 fm11_data_recv(hi_u8 *rbuf);
hi_u8  fm11_write_reg(hi_u16 addr, hi_u8 data);
hi_u32 fm11_read_eep(hi_u8 *dataBuff, hi_u16 ReadAddr, hi_u16 len);
hi_void nfc_init(hi_void);
hi_void fm11_write_eep(hi_u16 addr,hi_u32 len,hi_u8 *wbuf);
hi_void sEE_WritePage(hi_u8 *pBuffer, hi_u16 WriteAddr, hi_u8 datalen);
hi_void fm11_data_send(hi_u32 ilen,hi_u8 *ibuf);
hi_u8 fm11_write_fifo(hi_u8 *pbuf, hi_u8 len);
hi_u32 write_read(hi_u8 reg_high_8bit_cmd, hi_u8 reg_low_8bit_cmd, hi_u8* recv_data, hi_u8 send_len, hi_u8 read_len);
hi_u8 fm11_read_reg(hi_u16 addr);
hi_u32 write_fifo_reg( hi_u8 reg_high_8bit_cmd, hi_u8 reg_low_8bit_cmd, hi_u8 data_buff);
hi_u32 write_fifo_data( hi_u8* data_buff, hi_u8 len);
#endif