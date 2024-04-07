/*
 * @Author: your name
 * @Date: 2021-05-25 01:01:38
 * @LastEditTime  2021-09-17 19:26:54
 * @LastEditors  yuchen
 * @Description: In User Settings Edit
 * @FilePath: /serial_protocol/adapter_for_serial_protocol/adapter_for_serial_protol.c
 */
#include <stdio.h>
#include "local_log.h"
#include <hi_types_base.h>
#include <hi_early_debug.h>
#include <hi_task.h>
#include <hi_uart.h>
#include <hi_mux.h>
static hi_u32 g_thread_mux_id;


#define WRITE_BY_INT
#define UART_DEMO_TASK_STAK_SIZE 2048
#define UART_DEMO_TASK_PRIORITY  25
#define DEMO_UART_NUM            HI_UART_IDX_1
#define UART_BUFF_SIZE           32

#ifndef HILINK_OK
#define HILINK_OK 0
#endif
#ifndef HILINK_ERR
#define HILINK_ERR (-1)
#endif
#ifndef HILINK_PROCESSING
#define HILINK_PROCESSING (-111)
#endif

int adapter_serial_init(void)
{
    int ret = 0;

    hi_uart_attribute uart_attr = {
        .baud_rate = 115200, /* baud_rate: 115200 */
        .data_bits = 8,      /* data_bits: 8bits */
        .stop_bits = 1,
        .parity = 0,
    };

    /* Initialize uart driver */
    ret = hi_uart_init(DEMO_UART_NUM, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        ERROR_PRINTF("Failed to init uart%d Err code = %d\n", DEMO_UART_NUM, ret);
        return;
    }

    // ret = hi_uart_init(HI_UART_IDX_2, &uart_attr, HI_NULL);
    // if (ret != HI_ERR_SUCCESS) {
    //     printf("\r\nFailed to init uart%d! Err code = %d\n", HI_UART_IDX_1, ret);
    //     return;
    // }
	// else
	// {
	// 	printf("\r\nSuccess to init uart2!!!!\n");
	// }
    return ret;
}

int adapter_close_serial(void)
{

    hi_uart_deinit(DEMO_UART_NUM);

    return HILINK_OK;
}

int adapter_serial_write(const unsigned char *data, const int length)
{
    int ret = 0;
    ret =  hi_uart_write(DEMO_UART_NUM, data, length);
    return ret;
}

int adapter_serial_timeout_read(unsigned char *resualt_buf, int read_len, int time)
{
    int ret = 0;
    ret = hi_uart_read_timeout(DEMO_UART_NUM, resualt_buf, read_len, time);
    return ret;
}

void adapter_reset_wifi_func(void)
{
    DEBUG_PRINTF("test_reset_wifi_func\r\n");
    hilink_restore_factory_settings();
    return;
}

void adpater_init_mutex(void)
{
    hi_mux_create(&g_thread_mux_id);
}

void adpater_thread_lock(void)
{
    hi_mux_pend(g_thread_mux_id, HI_SYS_WAIT_FOREVER);
    return;
}

void adpater_thread_unlock(void)
{
    hi_mux_post(g_thread_mux_id);
    return;
}
