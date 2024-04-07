/*
 * @Author: your name
 * @Date: 2021-05-06 17:43:42
 * @LastEditTime  2021-09-17 19:42:09
 * @LastEditors  yuchen
 * @Description: In User Settings Edit
 * @FilePath: /debug/log/local_log.c
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "local_log.h"
#include "serial_protocol.h"

#define LOG_MAX_LENGTH  2048

#if HILINK_CHINASOFT_RELEASE
static int global_log_level = ERROR_LOG_LEVEL;
#else
static int global_log_level = NORMAL_LOG_LEVEL;
#endif

void local_printf(const int current_log_level, const char *fmt, ...)
{
    if (current_log_level < global_log_level)
    {
        return;
    }
    char temp_log_buffer[LOG_MAX_LENGTH] = {0};
    memset(temp_log_buffer, 0, LOG_MAX_LENGTH);
    va_list temp_var;
    va_start(temp_var, fmt);
    vsnprintf(temp_log_buffer, LOG_MAX_LENGTH, fmt, temp_var);
    va_end(temp_var);
    printf("%s", temp_log_buffer);
}

void printf_tag_s(const TAGID_S *temp_action)
{
    DEBUG_PRINTF("temp_action: tagid is %04X, data_len is %04X\r\n", temp_action->tagid, temp_action->data_len);
    DEBUG_PRINTF("temp_action: data is:\r\n");
    for (size_t i = 0; i < temp_action->data_len; i++)
    {
        DEBUG_PRINTF("%02X\r\n", temp_action->data.str_value[i]);
    }
}

void local_log_set_log_level(int log_level)
{
    global_log_level = log_level;
    return;
}

int local_log_get_log_level(void)
{
    return global_log_level;
}