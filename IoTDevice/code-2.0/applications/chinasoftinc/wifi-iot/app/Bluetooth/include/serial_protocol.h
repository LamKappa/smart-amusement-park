/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  serial_protocol.h
**  功能描述 :  开发板功能配置文件
**  作    者 :  王滨泉
**  日    期 :  2021.09.27
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.09.27
                1 首次创建                 
\******************************************************************************/
#ifndef __SERIAL_PROTOCOL_H__
#define __SERIAL_PROTOCOL_H__
/******************************************************************************\
                             Includes
\******************************************************************************/	
#include <stdbool.h>
/******************************************************************************\
                          Macro definitions
\******************************************************************************/
#define DTYPE_BOOL_ENUM                             (1)
#define DTYPE_ENUM_ENUM                             (2)
#define DTYPE_INT_ENUM                              (3)
#define DTYPE_STR_ENUM                              (4)

#define PROTOL_RECV_MAX_TAGID_COUNT                 (24)

// 消息类型定义
#define PROTOCOL_APP_TO_MCU_PUT_MSG_T               0x0
#define PROTOCOL_APP_TO_MCU_GET_MSG_T               0x1
#define PROTOCOL_MCU_TO_APP_REPORT_MSG_T            0x02
#define PROTOCOL_MCU_TO_APP_GET_MSG_T               0x03
#define PROTOCOL_WIFI_MOD_TO_MCU_PUT_MSG_T          0x10
#define PROTOCOL_WIFI_MOD_TO_MCU_GET_MSG_T          0x11
#define PROTOCOL_MCU_TO_WIFI_MOD_REPORT_MSG_T       0x12
#define PROTOCOL_MCU_TO_WIFI_MOD_GET_MSG_T          0x13
#define PROTOCOL_WIFI_TO_MCU_FACTORY_TEST           0xFE
#define PROTOCOL_MCU_TO_WIFI_FACTORY_TEST           0xFF
//最长位512字节 再加上 4字节的偏移量
#define PROTOCOL_FILE_TRANS_MAX_FRAME_LEN           (512 + 4)
//华为默认给的32字节，这里适当的预留一些余量
#define PROTOCOL_COMMAND_TRANS_DATA_BUF_LEN         (56)

#ifndef HILINK_M2M_CLOUD_OFFLINE
/* HiLink SDK通知开发者设备状态 */
#define HILINK_M2M_CLOUD_OFFLINE        (0)         /* 设备与云端连接断开(版本向前兼容) */
#define HILINK_M2M_CLOUD_ONLINE         (1)         /* 设备连接云端成功, 处于正常工作态(版本向前兼容) */
#define HILINK_M2M_LONG_OFFLINE         (2)         /* 设备与云端连接长时间断开(版本向前兼容) */
#define HILINK_M2M_LONG_OFFLINE_REBOOT  (3)         /* 设备与云端连接长时间断开后进行重启(版本向前兼容) */
#define HILINK_UNINITIALIZED            (4)         /* HiLink线程未启动 */
#define HILINK_LINK_UNDER_AUTO_CONFIG   (5)         /* 设备处于配网模式 */
#define HILINK_LINK_CONFIG_TIMEOUT      (6)         /* 设备处于10分钟超时状态 */
#define HILINK_LINK_CONNECTTING_WIFI    (7)         /* 设备正在连接路由器 */
#define HILINK_LINK_CONNECTED_WIFI      (8)         /* 设备已经连上路由器 */
#define HILINK_M2M_CONNECTTING_CLOUD    (9)         /* 设备正在连接云端 */
#define HILINK_M2M_CLOUD_DISCONNECT     (10)        /* 设备与路由器的连接断开 */
#endif
/******************************************************************************\
                         Typedef definitions
\******************************************************************************/
#pragma pack(1)
typedef union 
{
    unsigned char bool_value;
    unsigned short enum_value;
    int int_value;
    char str_value[PROTOCOL_COMMAND_TRANS_DATA_BUF_LEN];
}DATA_UNION;

typedef struct{
    unsigned char data_type;
    unsigned short tagid;
    unsigned short data_len;
    DATA_UNION data;
}TAGID_S;

typedef union 
{
    unsigned char bool_value;
    unsigned short enum_value;
    int int_value;
    char str_value[PROTOCOL_FILE_TRANS_MAX_FRAME_LEN];
}DATA_FILE_UNION;

typedef struct{
    unsigned char data_type;
    unsigned short tagid;
    unsigned short data_len;
    DATA_FILE_UNION data;
}TAGID_FILE_S;

//系统级相关命令
typedef struct 
{
    unsigned char send_msg_type;
    unsigned char recv_msg_type;
    TAGID_S tag_s;
}SYS_INFO_TAGID_S;

#pragma pack()

typedef void (SET_GLOBAL_PROP_STATUS_FUNC)(TAGID_S *);
typedef void (REPORT_CURRENT_SVC_ID_FUNC)(void);
typedef struct{
    //属性对应的CMD结构体
    TAGID_S *cmd_s;
    //CMD对应的设置本地静态属性的函数
    SET_GLOBAL_PROP_STATUS_FUNC *set_prop_func;
    //CMD对应的SVCID上报函数
    REPORT_CURRENT_SVC_ID_FUNC *report_svcid_func;
}GLOBAL_CONTROL_S;



int serial_protocol_init(void); //初始化串口，初始化全局变量等
int serial_protocol_deinit(void);
int serial_protocol_reset(void);
int serial_protocol_control_write_multi(TAGID_S *in_tag_s_list[], const int tag_s_count, const unsigned char msg_type);
int serial_protocol_read_multi(TAGID_S in_tag_s_list[], int *tag_s_count, unsigned char *msg_type);
int serial_protocol_write_command_and_wait_response(TAGID_S *in_tag_s, const unsigned char msg_type, \
TAGID_S *out_tag_s, unsigned char *return_msg_type);
int serial_protocol_write_file_and_wait_response(TAGID_FILE_S *in_file_tag_s, const int msg_type, TAGID_S *out_tag_s, \
unsigned char *return_msg_type);
// int serial_protocol_read();
// int serial_protocol_system_write();
// int serial_protocol_file_write();
#endif
/******************************* End of File (H) ******************************/