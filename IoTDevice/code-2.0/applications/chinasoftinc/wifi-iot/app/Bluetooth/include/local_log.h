/*
 * @Author: your name
 * @Date: 2021-05-06 03:38:06
 * @LastEditTime: 2021-06-04 21:05:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /debug/log/local_unity_tool.h
 */

#ifndef __LOCAL_LOG_H__
#define __LOCAL_LOG_H__
#include "serial_protocol.h"
#ifndef HILINK_OK
#define HILINK_OK 0
#endif
#ifndef HILINK_ERR
#define HILINK_ERR (-1)
#endif
#ifndef HILINK_PROCESSING
#define HILINK_PROCESSING (-111)
#endif

typedef enum{
    NORMAL_LOG_LEVEL = 1,
    DEBUG_LOG_LEVEL = 2,
    ERROR_LOG_LEVEL = 3,
    LOG_LEVEL_MAX = ERROR_LOG_LEVEL
}LOG_LEVEL_ENUM;

void printf_tag_s(const TAGID_S *temp_action);

void local_printf(const int current_log_level, const char *fmt, ...);
#define NORMAL_PRINTF(fmt, ...)\
{\
local_printf(NORMAL_LOG_LEVEL, "NORMAL_INFO: %s-%d\t:", __FUNCTION__, __LINE__);\
local_printf(NORMAL_LOG_LEVEL, fmt, ##__VA_ARGS__);\
}

#define DEBUG_PRINTF(fmt, ...)\
{\
local_printf(DEBUG_LOG_LEVEL, "DEBUG_INFO: %s-%d\t:", __FUNCTION__, __LINE__);\
local_printf(DEBUG_LOG_LEVEL, fmt, ##__VA_ARGS__);\
}

#define ERROR_PRINTF(fmt, ...)\
{\
local_printf(ERROR_LOG_LEVEL, "ERROR_INFO: %s-%d\t:", __FUNCTION__, __LINE__);\
local_printf(ERROR_LOG_LEVEL, fmt, ##__VA_ARGS__);\
}

void local_log_set_log_level(int log_level);
int local_log_get_log_level(void);
#endif