/*
 * @Author: your name
 * @Date: 2021-05-25 01:01:31
 * @LastEditTime: 2021-06-03 20:10:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /serial_protocol/adapter_for_serial_protocol/adapter_for_serial_protol.h
 */

//串口初始化函数
int adapter_serial_init(void);
int adapter_close_serial(void);
int adapter_serial_write(const unsigned char *data, const int length);
int adapter_serial_timeout_read(unsigned char *resualt_buf, int read_len, int time);
void adapter_reset_wifi_func(void);
void adpater_thread_lock(void);
void adpater_thread_unlock(void);
void adpater_init_mutex(void);