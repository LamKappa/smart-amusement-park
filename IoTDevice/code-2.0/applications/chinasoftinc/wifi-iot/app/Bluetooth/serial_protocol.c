/*
 * @Author: your name
 * @Date: 2021-05-06 01:03:14
 * @LastEditTime  2021-09-17 19:44:18
 * @LastEditors  yuchen
 * @Description: In User Settings Edit
 * @FilePath: /debug/serial_protocol/serial_protocol.c
 */
#include <stdio.h>
#include <string.h>
#include "lwip/sockets.h"
#include "local_log.h"
#include "serial_protocol.h"
#include "adapter_for_serial_protol.h"


/* PUT命令控制接口返回值 */
#ifndef HILINK_OK
#define HILINK_OK 0
#endif
#ifndef HILINK_ERR
#define HILINK_ERR (-1)
#endif
#ifndef HILINK_PROCESSING
#define HILINK_PROCESSING (-111)
#endif

#define PROTOCOL_HEAD_FIRST_BYTE_IDX    0
#define PROTOCOL_HEAD_SECOND_BYTE_IDX   1
#define PROTOCOL_PRO_VER_INDEX          2
#define PROTOCOL_MSG_TYPE_BYTE_INDEX    3
#define PROTOCOL_DATA_LENGTH_INDEX      4
#define PROTOCOL_DATA_LENGTH_FIRST_INDEX    4
#define PROTOCOL_DATA_LENGTH_SECOND_INDEX   5
#define PROTOCOL_DATA_START_INDEX       6
#define PROTOCOL_HEAD_CONST_LEN         6
#define PROTOCOL_DATA_LENGTH_BYTE_COUNT 2
//协议中的固定值定义
#define PROTOCOL_HEAD_FIRT_BYTE_VALUE   0xA5
#define PROTOCOL_HEAD_SECOND_BYTE_VALUE 0x5A
#define PROTOCOL_PRO_VER_VALUE          0x1

//等待接收帧的次数，超出以下限定值则认为本次收取返回值失败,约等于等待400ms
#define PROTOCOL_WAIT_REPONSE_COUNT     100
#define PROTOCOL_WAIT_FILE_REPONSE_COUNT    10

//串口发送的最大长度
#define PROTOCOL_SEND_BUF_MAX_LIMIT    512
#define PROTOCOL_RECV_BUF_MAX_LIMIT     PROTOCOL_SEND_BUF_MAX_LIMIT
//文件传输的串口buf大小,512为每次传输文件的最大长度，30自己是为协议预留的字节
#define PROTOCOL_FILE_TANS_BUF_MAX_LIMIT    (512 + 30)
//串口hex日志数组的最大长度,增加的36字节位函数名长度
#define PROTOCOL_DEBUG_HEX_MESG_MAX_LEN     (36)
#define PROTOCOL_SEND_AND_RECV_MAX_BUF_LEN  (1024 + 64)
#define PROTOCOL_RECV_HEAD_TIMEOUT      20
#define PROTOCOL_RECV_DATA_TIMEOUT      100
//各种数据类型的实际长度
#define P_BOOL_ACTION_LENGTH        (sizeof(unsigned short) * 2 + sizeof(unsigned char))
#define P_ENUM_ACTION_LENGTH        (sizeof(unsigned short) * 2 + sizeof(unsigned short))
#define P_INT_ACTION_LENGTH         (sizeof(unsigned short) * 2 + sizeof(unsigned int))
#define P_STR_ACTION_DEFAULT_LENGTH (sizeof(unsigned short) * 2)

#define G_SERIAL_NAME   "/dev/ttyUSB0"


typedef enum{
    DEVICE_UN_INIT = -1,
    DEVICE_INIT = 0,
    DEVICE_INIT_FAILED = 1
}DEVICE_STATUS_E;

static int g_serial_status = DEVICE_UN_INIT;

//需要根据具体平台不一样 去具体实现 interface
int serial_protocol_init(void) //初始化串口，初始化全局变量等
{
    int ret = HILINK_OK;
    //初始化全局的互斥锁
    adpater_init_mutex();
    if (adapter_serial_init() == HILINK_OK)
    {
        g_serial_status = DEVICE_INIT;
    }else
    {
        g_serial_status = DEVICE_INIT_FAILED;
        ret = HILINK_ERR;
    }
    return ret;
}

//需要根据具体平台不一样 去具体实现 interface
int serial_protocol_deinit(void)
{
    return adapter_close_serial();
}

int serial_protocol_reset(void)
{
    serial_protocol_deinit();
    serial_protocol_init();
    return HILINK_OK;
}

//这里包含标准的printf后续根据实际情况进行调整 TODO
static void dump_original_hex(const unsigned char *temp_buf, const int send_len)
{
    int i = 0;
    // TAGID_S temp_action;
    // memset(&temp_action, 0, sizeof(TAGID_S));
    printf("temp_buf is :");
    for ( i = 0; i < send_len; i++)
    {
        printf("%02X ", temp_buf[i]);
    }
    printf("\r\n");
}

static char global_hex_log_buf[PROTOCOL_SEND_AND_RECV_MAX_BUF_LEN] = {0};

static void debug_dump_original_hex(const char *msg, const unsigned char *temp_buf, const int send_len)
{
    if (local_log_get_log_level() > DEBUG_LOG_LEVEL)
    {
        return;
    }
    memset(global_hex_log_buf, 0, PROTOCOL_SEND_AND_RECV_MAX_BUF_LEN);
    int msg_len = strnlen(msg, PROTOCOL_DEBUG_HEX_MESG_MAX_LEN);
    int i = 0;
    snprintf(global_hex_log_buf, PROTOCOL_DEBUG_HEX_MESG_MAX_LEN, "%s", msg);
    for ( i = 0; i < send_len; i++)
    {
        if ((msg_len + i * 3) > PROTOCOL_SEND_AND_RECV_MAX_BUF_LEN - 2)
        {
            break;
        }
        snprintf(&(global_hex_log_buf[msg_len + i * 3]), 4, "%02X ", temp_buf[i]);
    }
    global_hex_log_buf[msg_len + i * 3 + 1] = 0;
    DEBUG_PRINTF("%s\r\n", global_hex_log_buf);
    DEBUG_PRINTF("\r\n");
}


static void error_dump_original_hex(const char *msg, const unsigned char *temp_buf, const int send_len)
{
    int msg_len = strnlen(msg, PROTOCOL_DEBUG_HEX_MESG_MAX_LEN);
    int i = 0;
    snprintf(global_hex_log_buf, PROTOCOL_DEBUG_HEX_MESG_MAX_LEN, "%s", msg);
    for ( i = 0; i < send_len; i++)
    {
        if ((msg_len + i * 3) > PROTOCOL_SEND_AND_RECV_MAX_BUF_LEN - 2)
        {
            break;
        }
        snprintf(&(global_hex_log_buf[msg_len + i * 3]), 4, "%02X ", temp_buf[i]);
    }
    global_hex_log_buf[msg_len + i * 3] = 0;
    ERROR_PRINTF("%s\r\n", global_hex_log_buf);
}



//计算收到帧的CRC是否正确，如果正确返回HILINK_OK， 如果不正确返回HILINK_ERR
int check_recv_data_crc(unsigned char *buf, const int data_len)
{
    unsigned char tail_crc = 0;
    int i = 0;
    for ( i = 0; i < data_len; i++)
    {
        tail_crc += buf[i];
    }
    if (buf[data_len] == tail_crc)
    {
        return HILINK_OK;
    }else
    {
        return HILINK_ERR;
    }
    
    
}

int fill_crc_tail(unsigned char *buf, const int data_len)
{
    unsigned char tail_crc = 0;
    int i = 0;
    for ( i = 0; i < data_len; i++)
    {
        tail_crc += buf[i];
    }
    buf[data_len] = tail_crc;
    return data_len + 1;
}

int serial_protocol_control_write_multi(TAGID_S *in_tag_s_list[], const int tag_s_count, const unsigned char msg_type)
{
    adpater_thread_lock();
    TAGID_S local_tag_s;
    TAGID_S *tag_s = &local_tag_s;
    //const TAGID_S *in_tag_s = NULL;
    int send_data_len = 0;
    //action 结构体中data 的长度
    int temp_action_data_len = 0;
    //action 总长度
    int temp_action_len = 0;
    int ret = HILINK_OK;
    unsigned short temp_data_len = 0;
    unsigned char temp_send_buf[PROTOCOL_SEND_BUF_MAX_LIMIT] = {0};
    int i = 0;
    memset(temp_send_buf, 0, PROTOCOL_SEND_BUF_MAX_LIMIT);
    //设置固定的协议头
    temp_send_buf[PROTOCOL_HEAD_FIRST_BYTE_IDX] = PROTOCOL_HEAD_FIRT_BYTE_VALUE;
    temp_send_buf[PROTOCOL_HEAD_SECOND_BYTE_IDX] = PROTOCOL_HEAD_SECOND_BYTE_VALUE;
    temp_send_buf[PROTOCOL_PRO_VER_INDEX] = PROTOCOL_PRO_VER_VALUE;
    //此接口函数实现的都是 APP 下发指令
    temp_send_buf[PROTOCOL_MSG_TYPE_BYTE_INDEX] = msg_type;
    send_data_len += PROTOCOL_HEAD_CONST_LEN;
    for (i = 0; i < tag_s_count; i++)
    {        
        //清空本地的tag_s
        memset(tag_s, 0, sizeof(TAGID_S));     
        //将传入的tag_s copy 过来
        memcpy((void *)tag_s, (void *)in_tag_s_list[i], sizeof(TAGID_S));
        //将tagid 切换为大端序
        tag_s->tagid = htons(tag_s->tagid);
        switch (tag_s->data_type)
        {
        case DTYPE_BOOL_ENUM:
            temp_action_data_len = htons(sizeof(unsigned char));
            tag_s->data_len = temp_action_data_len;
            temp_action_len = P_BOOL_ACTION_LENGTH;
            break;
        case DTYPE_ENUM_ENUM:
            temp_action_data_len = htons(sizeof(unsigned short));
            tag_s->data_len = temp_action_data_len;
            tag_s->data.enum_value = htons(tag_s->data.enum_value);
            temp_action_len = P_ENUM_ACTION_LENGTH;
            break;
        case DTYPE_INT_ENUM:
            temp_action_data_len = htons(sizeof(unsigned int));
            tag_s->data_len = temp_action_data_len;
            tag_s->data.int_value = htonl(tag_s->data.int_value);
            temp_action_len = P_INT_ACTION_LENGTH;
            break;
        case DTYPE_STR_ENUM:
            temp_action_data_len = tag_s->data_len;
            tag_s->data_len = htons(tag_s->data_len);
            temp_action_len = temp_action_data_len + P_STR_ACTION_DEFAULT_LENGTH;
            break;
        default:
            ERROR_PRINTF("Get Action struct data type error, data_type is %d\r\n", tag_s->data_type);
            ret = HILINK_ERR;
            return ret;
        }
        if ((send_data_len + temp_action_len) > (PROTOCOL_SEND_BUF_MAX_LIMIT - 1))
        {
            ERROR_PRINTF("send data too long length is %d\r\n", (send_data_len + temp_action_len));
            return HILINK_ERR;
        }
        memcpy(&(temp_send_buf[send_data_len]), (void *)tag_s + 1, temp_action_len);
        send_data_len += temp_action_len;
    }
    temp_data_len = send_data_len - PROTOCOL_HEAD_CONST_LEN;
    temp_data_len = htons(temp_data_len);
    memcpy(&(temp_send_buf[PROTOCOL_DATA_LENGTH_INDEX]), (void *)&temp_data_len, sizeof(unsigned short));
    send_data_len = fill_crc_tail(temp_send_buf, send_data_len);
    adapter_serial_write(temp_send_buf, send_data_len);
    debug_dump_original_hex(__FUNCTION__, temp_send_buf, send_data_len);
    adpater_thread_unlock();
    return ret;
}


//传入的数组为tagid 的列表，不包含协议头，和CRC校验尾
static int get_recv_action_list(unsigned char *data_buf, const int data_len, TAGID_S in_tag_s_list[], int *tag_s_count)
{
    int remain_data_to_analyse_len = 0;
    int current_analyse_data_index = 0;
    int temp_tag_s_count = 0;
    TAGID_S temp_action;
    unsigned short *temp_data_len_p = NULL;
    unsigned short *temp_tagid_p = NULL;
    remain_data_to_analyse_len = data_len;

    while (remain_data_to_analyse_len > 0)
    {
        memset(&temp_action, 0, sizeof(temp_action));
        temp_tagid_p = (unsigned short *)&(data_buf[current_analyse_data_index]);
        temp_action.tagid = ntohs(*temp_tagid_p);
        current_analyse_data_index += sizeof(unsigned short);
        temp_data_len_p = (unsigned short *)&(data_buf[current_analyse_data_index]);
        temp_action.data_len = ntohs(*temp_data_len_p);
        current_analyse_data_index += sizeof(unsigned short);
        memcpy(&(temp_action.data.str_value), &(data_buf[current_analyse_data_index]), temp_action.data_len);
        current_analyse_data_index += temp_action.data_len;
        remain_data_to_analyse_len = data_len - current_analyse_data_index;
        temp_tag_s_count ++;
        DEBUG_PRINTF("temp_tag_s_count is %d\r\n", temp_tag_s_count);
        if (temp_tag_s_count > PROTOL_RECV_MAX_TAGID_COUNT)
        {
            ERROR_PRINTF("recv tag id too many \r\n");
            return HILINK_ERR;
        }
        //printf_tag_s(&temp_action);
        //将当前获取的tag_s保存至返回的列表
        memcpy(&(in_tag_s_list[temp_tag_s_count - 1]), &temp_action, sizeof(temp_action));
    }
    *tag_s_count = temp_tag_s_count;
    return HILINK_OK;
}

int serial_protocol_read_multi(TAGID_S in_tag_s_list[], int *tag_s_count, unsigned char *msg_type)
{    
    unsigned char recv_buf[PROTOCOL_RECV_BUF_MAX_LIMIT] = {0};
    int temp_read_len = 0;
    int ret = 0;
    //当前帧的数据长度包含CRC字节
    unsigned short temp_data_len = 0;
    unsigned short *temp_data_len_p = NULL;
    //真实的数据长度
    int remain_data_len = 0;
    //int temp_action_data_len = 0;
    *tag_s_count = 0;
    if (g_serial_status != DEVICE_INIT)
    {
        ERROR_PRINTF("serial have not init\r\n");
        return HILINK_ERR;
    }
    memset(recv_buf, 0, PROTOCOL_RECV_BUF_MAX_LIMIT);
    temp_read_len = adapter_serial_timeout_read(recv_buf, PROTOCOL_HEAD_CONST_LEN, PROTOCOL_RECV_HEAD_TIMEOUT);
    if (temp_read_len == 0)
    {
        return HILINK_OK;
    }else if (temp_read_len == PROTOCOL_HEAD_CONST_LEN)
    {
        //收到了足够的长度，开始分析数据包
        debug_dump_original_hex("recv data from serial :", (unsigned char *)recv_buf, PROTOCOL_HEAD_CONST_LEN);
        temp_data_len_p = (unsigned short *)&(recv_buf[PROTOCOL_DATA_LENGTH_FIRST_INDEX]);
        // temp_data_len = recv_buf[PROTOCOL_DATA_LENGTH_FIRST_INDEX];
        // temp_data_len = (temp_data_len << 8) | recv_buf[PROTOCOL_DATA_LENGTH_SECOND_INDEX];
        temp_data_len = ntohs(*temp_data_len_p);
        remain_data_len = temp_data_len;
        //设置msg_type
        *msg_type = recv_buf[PROTOCOL_MSG_TYPE_BYTE_INDEX];
        //数据长度不含尾部crc字节
        temp_data_len = temp_data_len + 1;
        temp_read_len = adapter_serial_timeout_read(&(recv_buf[PROTOCOL_HEAD_CONST_LEN]), temp_data_len, PROTOCOL_RECV_DATA_TIMEOUT);
        if (temp_read_len != temp_data_len)
        {
            ERROR_PRINTF("recv data length error data to recv is %d, act recv is %d\r\n",temp_data_len, temp_read_len);
            error_dump_original_hex("recv data length error: ", (unsigned char *)recv_buf, PROTOCOL_HEAD_CONST_LEN + temp_read_len);
            return HILINK_ERR;
        }else
        {
            if (check_recv_data_crc((unsigned char *)recv_buf, temp_data_len+PROTOCOL_HEAD_CONST_LEN - 1) != HILINK_OK)
            {
                error_dump_original_hex("check crc failed :", (unsigned char *)recv_buf, PROTOCOL_HEAD_CONST_LEN + temp_read_len);
                return HILINK_ERR;
            }
            debug_dump_original_hex("recv full package : ", (unsigned char *)recv_buf, PROTOCOL_HEAD_CONST_LEN + temp_read_len);
        }
    }else
    {
        ERROR_PRINTF("protocol read error\r\n");
        return HILINK_ERR;
    }
    //收到了一帧完整的数据，开始解析单独的action
    ret = get_recv_action_list(&(recv_buf[PROTOCOL_DATA_START_INDEX]), remain_data_len, in_tag_s_list, tag_s_count);
    return ret;
}

int serial_protocol_write_command_and_wait_response(TAGID_S *in_tag_s, const unsigned char msg_type, 
TAGID_S *out_tag_s, unsigned char *return_msg_type)
{
    TAGID_S local_tag_s;
    TAGID_S *tag_s = &local_tag_s;
    //const TAGID_S *in_tag_s = NULL;
    int send_data_len = 0;
    //action 结构体中data 的长度
    int temp_action_data_len = 0;
    //action 总长度
    int temp_action_len = 0;
    //等待回应帧的次数，超过此次数则认为接受失败
    TAGID_S recv_tag_list[PROTOL_RECV_MAX_TAGID_COUNT] = {0};
    int recv_tag_count = 0;
    int recv_count = PROTOCOL_WAIT_REPONSE_COUNT;
    int ret = HILINK_OK;
    unsigned short temp_data_len = 0;
    unsigned char temp_send_buf[PROTOCOL_SEND_BUF_MAX_LIMIT] = {0};
    int i = 0;
    memset(recv_tag_list, 0, sizeof(recv_tag_list));
    memset(temp_send_buf, 0, PROTOCOL_SEND_BUF_MAX_LIMIT);
    //设置固定的协议头
    temp_send_buf[PROTOCOL_HEAD_FIRST_BYTE_IDX] = PROTOCOL_HEAD_FIRT_BYTE_VALUE;
    temp_send_buf[PROTOCOL_HEAD_SECOND_BYTE_IDX] = PROTOCOL_HEAD_SECOND_BYTE_VALUE;
    temp_send_buf[PROTOCOL_PRO_VER_INDEX] = PROTOCOL_PRO_VER_VALUE;
    //此接口函数实现的都是 MCU下发系统相关指令
    temp_send_buf[PROTOCOL_MSG_TYPE_BYTE_INDEX] = msg_type;
    send_data_len += PROTOCOL_HEAD_CONST_LEN;        
    //清空本地的tag_s
    memset(tag_s, 0, sizeof(TAGID_S));     
    //将传入的tag_s copy 过来
    memcpy((void *)tag_s, (void *)in_tag_s, sizeof(TAGID_S));
    //将tagid 切换为大端序
    tag_s->tagid = htons(tag_s->tagid);
    switch (tag_s->data_type)
    {
        case DTYPE_BOOL_ENUM:
            temp_action_data_len = htons(sizeof(unsigned char));
            tag_s->data_len = temp_action_data_len;
            temp_action_len = P_BOOL_ACTION_LENGTH;
            break;
        case DTYPE_ENUM_ENUM:
            temp_action_data_len = htons(sizeof(unsigned short));
            tag_s->data_len = temp_action_data_len;
            tag_s->data.enum_value = htons(tag_s->data.enum_value);
            temp_action_len = P_ENUM_ACTION_LENGTH;
            break;
        case DTYPE_INT_ENUM:
            temp_action_data_len = htons(sizeof(unsigned int));
            tag_s->data_len = temp_action_data_len;
            tag_s->data.int_value = htonl(tag_s->data.int_value);
            temp_action_len = P_INT_ACTION_LENGTH;
            break;
        case DTYPE_STR_ENUM:
            temp_action_data_len = tag_s->data_len;
            tag_s->data_len = htons(tag_s->data_len);
            temp_action_len = temp_action_data_len + P_STR_ACTION_DEFAULT_LENGTH;
            break;
        default:
            ERROR_PRINTF("Get Action struct data type error, data_type is %d\r\n", tag_s->data_type);
            ret = HILINK_ERR;
            return ret;
    }
    if ((send_data_len + temp_action_len) > (PROTOCOL_SEND_BUF_MAX_LIMIT - 1))
    {
        ERROR_PRINTF("send data too long length is %d\r\n", (send_data_len + temp_action_len));
        return HILINK_ERR;
    }
    memcpy(&(temp_send_buf[send_data_len]), (void *)tag_s + 1, temp_action_len);
    send_data_len += temp_action_len;
    temp_data_len = send_data_len - PROTOCOL_HEAD_CONST_LEN;
    temp_data_len = htons(temp_data_len);
    memcpy(&(temp_send_buf[PROTOCOL_DATA_LENGTH_INDEX]), (void *)&temp_data_len, sizeof(unsigned short));
    send_data_len = fill_crc_tail(temp_send_buf, send_data_len);
    debug_dump_original_hex(__FUNCTION__, temp_send_buf, send_data_len);
    adpater_thread_lock();
    adapter_serial_write(temp_send_buf, send_data_len);
    while (recv_count--)
    {
        ret = serial_protocol_read_multi(recv_tag_list, &recv_tag_count, return_msg_type);
        if (ret != HILINK_OK)
        {
            continue;
        }
        for (i = 0; i < recv_tag_count; i++)
        {
            //tag_id 匹配上了,就将当前tag_s返回，供发送命令的函数进行分析
            if (recv_tag_list[i].tagid == in_tag_s->tagid)
            {
                memcpy(out_tag_s, &(recv_tag_list[i]), sizeof(TAGID_S));
                DEBUG_PRINTF("recv_count is %d\r\n", recv_count);
                adpater_thread_unlock();
                return HILINK_OK;
            }
            
        }
        
        
    }
    adpater_thread_unlock();
    ERROR_PRINTF("can not recv mcu response tag_id is %04x\r\n", in_tag_s->tagid);
    ret = HILINK_ERR;
    return ret;
}


int serial_protocol_write_file_and_wait_response(TAGID_FILE_S *in_file_tag_s, const int msg_type, TAGID_S *out_tag_s, \
unsigned char *return_msg_type)
{
    TAGID_FILE_S local_tag_s;
    TAGID_FILE_S *tag_s = &local_tag_s;
    int send_data_len = 0;
    //action 结构体中data 的长度
    int temp_action_data_len = 0;
    //action 总长度
    int temp_action_len = 0;
    //等待回应帧的次数，超过此次数则认为接受失败
    TAGID_S recv_tag_list[PROTOL_RECV_MAX_TAGID_COUNT] = {0};
    int recv_tag_count = 0;
    int recv_count = PROTOCOL_WAIT_FILE_REPONSE_COUNT;
    int ret = HILINK_OK;
    unsigned short temp_data_len = 0;
    unsigned char temp_send_buf[PROTOCOL_FILE_TANS_BUF_MAX_LIMIT] = {0};
    int i = 0;
    memset(recv_tag_list, 0, sizeof(recv_tag_list));
    memset(temp_send_buf, 0, PROTOCOL_FILE_TANS_BUF_MAX_LIMIT);
    //设置固定的协议头
    temp_send_buf[PROTOCOL_HEAD_FIRST_BYTE_IDX] = PROTOCOL_HEAD_FIRT_BYTE_VALUE;
    temp_send_buf[PROTOCOL_HEAD_SECOND_BYTE_IDX] = PROTOCOL_HEAD_SECOND_BYTE_VALUE;
    temp_send_buf[PROTOCOL_PRO_VER_INDEX] = PROTOCOL_PRO_VER_VALUE;
    temp_send_buf[PROTOCOL_MSG_TYPE_BYTE_INDEX] = msg_type;
    send_data_len += PROTOCOL_HEAD_CONST_LEN;        
    //清空本地的tag_s
    memset(tag_s, 0, sizeof(TAGID_FILE_S));     
    //将传入的tag_s copy 过来
    memcpy((void *)tag_s, (void *)in_file_tag_s, sizeof(TAGID_FILE_S));
    //将tagid 切换为大端序
    tag_s->tagid = htons(tag_s->tagid);
    switch (tag_s->data_type)
    {
        case DTYPE_STR_ENUM:
            temp_action_data_len = tag_s->data_len;
            tag_s->data_len = htons(tag_s->data_len);
            temp_action_len = temp_action_data_len + P_STR_ACTION_DEFAULT_LENGTH;
            break;
        default:
            ERROR_PRINTF("Get Action struct data type error, data_type is %d\r\n", tag_s->data_type);
            ret = HILINK_ERR;
            return ret;
    }
    if ((send_data_len + temp_action_len) > (PROTOCOL_FILE_TANS_BUF_MAX_LIMIT - 1))
    {
        ERROR_PRINTF("send data too long length is %d\r\n", (send_data_len + temp_action_len));
        return HILINK_ERR;
    }
    memcpy(&(temp_send_buf[send_data_len]), (void *)tag_s + 1, temp_action_len);
    send_data_len += temp_action_len;
    temp_data_len = send_data_len - PROTOCOL_HEAD_CONST_LEN;
    temp_data_len = htons(temp_data_len);
    memcpy(&(temp_send_buf[PROTOCOL_DATA_LENGTH_INDEX]), (void *)&temp_data_len, sizeof(unsigned short));
    send_data_len = fill_crc_tail(temp_send_buf, send_data_len);
    debug_dump_original_hex("send_file_wait_respon", temp_send_buf, send_data_len);
    adpater_thread_lock();
    adapter_serial_write(temp_send_buf, send_data_len);
    while (recv_count--)
    {
        ret = serial_protocol_read_multi(recv_tag_list, &recv_tag_count, return_msg_type);
        if (ret != HILINK_OK)
        {
            continue;
        }
        for (i = 0; i < recv_tag_count; i++)
        {
            //tag_id 匹配上了,就将当前tag_s返回，供发送命令的函数进行分析
            if (recv_tag_list[i].tagid == in_file_tag_s->tagid)
            {
                memcpy(out_tag_s, &(recv_tag_list[i]), sizeof(TAGID_S));
                DEBUG_PRINTF("recv_count is %d\r\n", recv_count);
                adpater_thread_unlock();
                return HILINK_OK;
            }
            
        }
    }
    adpater_thread_unlock();
    ERROR_PRINTF("can not recv mcu response tag_id is %04x\r\n", in_file_tag_s->tagid);
    ret = HILINK_ERR;
    return ret;
}
