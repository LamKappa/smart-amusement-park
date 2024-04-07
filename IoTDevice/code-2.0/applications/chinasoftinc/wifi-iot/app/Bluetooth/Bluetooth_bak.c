/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  Bluetooth.c
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

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "ohos_types.h"
#include "local_log.h"
#include "lwip/sockets.h"
#include "serial_protocol.h"
#include "adapter_for_serial_protol.h"
#include "ly_evk_config.h"


/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define BLE_TASK_STACK_SIZE      1024*8               //任务栈大小
#define BLE_TASK_PRIO            10                   //任务优先等级





#if MODULE_FIRE
extern int fire_distance;
extern int fire_alarm_distance;

TAGID_S fire_distance_cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0001,
    .data.int_value = 0x0
};

TAGID_S fire_alarm_distance_cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0002,
    .data.int_value = 0x0
};
#endif // DEBUG
/* wangbinquan 2021.09.23 add */
#if MODULE_SOMG
//烟感温度
TAGID_S Smog_Temp_cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0003,
    .data.int_value = 0x0
};
//烟感湿度
TAGID_S Smog_Humidity_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0004,
    .data.int_value = 0x0
};
//烟雾浓度
TAGID_S Smog_Scope_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0005,
    .data.int_value = 0x0
};
//烟雾报警阈值
TAGID_S Smog_Alarm_Threshold_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0006,
    .data.int_value = 0x0
};
#endif

#if MODULE_SOIL
extern char Set_Water_Pump_Onoff_Threshold(uint8_t HumiThreshold);
extern char Get_Water_Pump_Onoff_Threshold(uint8_t *HumiThreshold);
extern char Get_Soil_Humi(uint8_t *Humi);
//土壤湿度
TAGID_S Soil_Humidity_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0007,
    .data.int_value = 0x0
};
//水泵开关阈值
TAGID_S Water_Pump_OnOff_Threshold_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0008,
    .data.int_value = 0x0
};
#endif

#if MODULE_TEMP_HUMI
extern char Set_Sht40_Environment_Temp_Alarm_Threshold(uint8_t TempThreshold);
extern char Set_Sht40_Environment_Humidity_Alarm_Threshold(uint8_t HumiThreshold);
extern char Get_Sht40_Environment_Temp_Alarm_Threshold(uint8_t *TempThreshold);
extern char Get_Sht40_Environment_Humidity_Alarm_Threshold(uint8_t *HumiThreshold);
extern char Get_Sht40_Environment_Temp(uint8_t *Temp);
extern char Get_Sht40_Environment_Humidity(uint8_t *Humi);
//环境温度
TAGID_S Environment_Temp_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0009,
    .data.int_value = 0x0
};
//环境湿度
TAGID_S Environment_Humidity_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x000A,
    .data.int_value = 0x0
};
//环境温度报警阈值
TAGID_S Environment_Temp_Alarm_Threshold_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x000B,
    .data.int_value = 0x0
};
//环境湿度报警阈值
TAGID_S Environment_Humidity_Alarm_Threshold_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x000C,
    .data.int_value = 0x0
};
#endif

#if MODULE_ALCOHOL
extern uint16_t Get_MIX2111D_Alcoho(void);
extern uint16_t Get_MIX2111D_Alcoho_Alarm_Threshold(void);
extern void Set_MIX2111D_Alcoho_Alarm_Threshold(uint16_t Alcohol_Threshold);
//酒精浓度
TAGID_S Alcohol_Concentration_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x000D,
    .data.int_value = 0x0
};
//酒精浓度报警阈值
TAGID_S Alcohol_Alarm_Threshold_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x000E,
    .data.int_value = 0x0
};
#endif

#if MODULE_COLOUR

extern int Get_Red_Data(void);
extern int Get_Green_Data(void);
extern int Get_Blue_Data(void);
//红色
TAGID_S Colour_Red_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x000F,
    .data.int_value = 0x0
};
//绿色
TAGID_S Colour_Green_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0010,
    .data.int_value = 0x0
};
//蓝色
TAGID_S Colour_Blue_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0011,
    .data.int_value = 0x0
};
#endif

#if MODULE_HR_SPO2
extern char Get_Max30102_Data(uint8_t *HeartRate, uint8_t *SpO2);
//心率
TAGID_S Heart_Rate_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0012,
    .data.int_value = 0x0
};
//血氧
TAGID_S SpO2_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0019,
    .data.int_value = 0x0
};
#endif

#if MODULE_AIR
extern uint16_t Get_CCS811_CO2(void);
extern uint16_t Get_CCS811_TVOC(void);
extern uint8_t Get_Air_Quality(void);
//空气质量
TAGID_S Air_Quality_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0013,
    .data.int_value = 0x0
};
//空气质量报警阈值
TAGID_S Air_Quality_Alarm_Threshold_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0014,
    .data.int_value = 0x0
};

//CO2浓度
TAGID_S CO2_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x001A,
    .data.int_value = 0x0
};
//TVOC
TAGID_S TVOC_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x001B,
    .data.int_value = 0x0
};

#endif

#if MODULE_MEASUREMENT_TEMP
extern char Get_IR_Measure_Temp(uint8_t *Temp);
//红外测温温度
TAGID_S Measurement_Temp_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0015,
    .data.int_value = 0x0
};
#endif

#if MODULE_ORIENTATION
extern int Get_Gps_Longitude(void);
extern int Get_Gps_Latitude(void);
extern long Get_Bmp220_Pressure(void) ;
//经度
TAGID_S Longitude_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0016,
    .data.int_value = 0x0
};
//纬度
TAGID_S Latitude_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0017,
    .data.int_value = 0x0
};
//压力
TAGID_S Pressure_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0018,
    .data.int_value = 0x0
};
#endif

/******************************************************************************\
                             Functions definitions
\******************************************************************************/
#if MODULE_SOIL
static void Put_Water_Pump_Onoff_Threshold(TAGID_S * temp_tag_id_s)
{
    if(Set_Water_Pump_Onoff_Threshold((uint8_t)ntohl(temp_tag_id_s->data.int_value)) == 0)
    {
        ;
    }
}
#endif


#if MODULE_FIRE
static void put_fire_alarm_distance(TAGID_S * temp_tag_id_s)
{
    fire_alarm_distance=ntohl(temp_tag_id_s->data.int_value);
}
#endif

#if MODULE_TEMP_HUMI
static void Put_Environment_Temp_Alarm_Threshold(TAGID_S * temp_tag_id_s)
{
    uint8_t tmp;

    tmp = (uint8_t)ntohl(temp_tag_id_s->data.int_value);
    Set_Sht40_Environment_Temp_Alarm_Threshold(tmp);
    // if(Set_Sht40_Environment_Temp_Alarm_Threshold((uint8_t)ntohl(temp_tag_id_s->data.int_value)) == 0)
    // {
    //     ;
    // }
}

static void Put_Environment_Humidity_Alarm_Threshold(TAGID_S * temp_tag_id_s)
{
    uint8_t tmp;
    tmp = (uint8_t)ntohl(temp_tag_id_s->data.int_value);
    Set_Sht40_Environment_Humidity_Alarm_Threshold(tmp);
    // if(Set_Sht40_Environment_Humidity_Alarm_Threshold((uint8_t)ntohl(temp_tag_id_s->data.int_value)) == 0)
    // {
    //     ;
    // }
}
#endif

#if MODULE_ALCOHOL
static void Put_Alcohol_Alarm_Threshold_Cmd(TAGID_S * temp_tag_id_s)
{
    // uint16_t alcohol;
    // alcohol = (uint16_t)ntohl(temp_tag_id_s->data.int_value);
    Set_MIX2111D_Alcoho_Alarm_Threshold((uint16_t)ntohl(temp_tag_id_s->data.int_value));
}

#endif

//put功能实现
static GLOBAL_CONTROL_S put_array[] = {
    #if MODULE_FIRE 
    //火焰距离报警阈值
    {&fire_alarm_distance_cmd,                      &put_fire_alarm_distance, NULL},
    #endif
    #if MODULE_SOMG
    //烟雾报警阈值
    {&Smog_Alarm_Threshold_Cmd,                     &put_fire_alarm_distance, NULL},
    #endif
    #if MODULE_SOIL
    //水泵开关阈值
    {&Water_Pump_OnOff_Threshold_Cmd,               &Put_Water_Pump_Onoff_Threshold, NULL},
    #endif
    #if MODULE_TEMP_HUMI
    //环境温度报警阈值
    {&Environment_Temp_Alarm_Threshold_Cmd,         &Put_Environment_Temp_Alarm_Threshold, NULL},
    //环境湿度报警阈值
    {&Environment_Humidity_Alarm_Threshold_Cmd,     &Put_Environment_Humidity_Alarm_Threshold, NULL},
    #endif
    #if MODULE_ALCOHOL
    //酒精浓度报警阈值
    {&Alcohol_Alarm_Threshold_Cmd,                  &Put_Alcohol_Alarm_Threshold_Cmd, NULL},
    #endif
    #if MODULE_AIR
    // //空气质量报警阈值
    // {&Air_Quality_Alarm_Threshold_Cmd,              &put_fire_alarm_distance, NULL},
    #endif
};

#define PUT_ARRAY_LENGTH    (sizeof(put_array)/sizeof(put_array[0]))

static void handle_recv_app_to_mcu_put(TAGID_S recv_tag_id_list[], const int recv_tag_count)
{
    DEBUG_PRINTF("recv_tag_count is %d\r\n", recv_tag_count);
    for (size_t i = 0; i < recv_tag_count; i++)
    {
        //printf_tag_s(&(recv_tag_id_list[i]));
        //找到对应的cmd结构体，并设置缓存值
        for (size_t j = 0; j < PUT_ARRAY_LENGTH; j++)
        {
            if (recv_tag_id_list[i].tagid == put_array[j].cmd_s->tagid)
            {
                put_array[j].set_prop_func(&(recv_tag_id_list[i]));
                break;
            }
        }
    }
}


#if MODULE_SOIL
static void Get_Current_Soil_Humidity()
{
    TAGID_S *temp_tagid_list[] = {&Soil_Humidity_Cmd};
    uint8_t humidity;
    if(Get_Soil_Humi(&humidity) == 0)
    {
        ;
    }
    Soil_Humidity_Cmd.data.int_value=(int)humidity;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Current_Water_Pump_Onoff_Threshold()
{
    TAGID_S *temp_tagid_list[] = {&Water_Pump_OnOff_Threshold_Cmd};
    uint8_t humi_threshold;
    if(Get_Water_Pump_Onoff_Threshold(&humi_threshold) == 0)
    {
        ;
    }
    Water_Pump_OnOff_Threshold_Cmd.data.int_value=(int)humi_threshold;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif


#if MODULE_FIRE
static void get_fire_distance(){
    TAGID_S *temp_tagid_list[] = {&fire_distance_cmd};
    fire_distance_cmd.data.int_value=fire_distance;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void get_fire_alarm_distance(){
    TAGID_S *temp_tagid_list[] = {&fire_alarm_distance_cmd};
    fire_alarm_distance_cmd.data.bool_value=fire_alarm_distance;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_ORIENTATION
static void Get_Longitude(){
    TAGID_S *temp_tagid_list[] = {&Longitude_Cmd};
    int longitude;
    longitude = Get_Gps_Longitude(); 
    Longitude_Cmd.data.int_value = longitude;               //经度
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Latitude(){
    TAGID_S *temp_tagid_list[] = {&Latitude_Cmd};
    int latitude;
    latitude = Get_Gps_Latitude(); 
    Latitude_Cmd.data.int_value = latitude;               //纬度
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Pressure(){
    TAGID_S *temp_tagid_list[] = {&Pressure_Cmd};
    long Press;
    Press = Get_Bmp220_Pressure(); 
    Pressure_Cmd.data.int_value = (int)Press;               //压强，单位：pa
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_TEMP_HUMI
static void Get_Environment_Temp(){
    TAGID_S *temp_tagid_list[] = {&Environment_Temp_Cmd};
    uint8_t temp=0;
    Get_Sht40_Environment_Temp(&temp); 
    Environment_Temp_Cmd.data.int_value = temp;                 //获取温度

    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Environment_Humidity(){
    TAGID_S *temp_tagid_list[] = {&Environment_Humidity_Cmd};
    uint8_t humi=0;
    Get_Sht40_Environment_Humidity(&humi); 
    Environment_Humidity_Cmd.data.int_value = humi;                 //获取湿度
    printf("\r\n Get humi = %d \r\n", humi);
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}


static void Get_Environment_Temp_Alarm_Threshold(){
    TAGID_S *temp_tagid_list[] = {&Environment_Temp_Alarm_Threshold_Cmd};
    uint8_t temp_threshold=0;
    Get_Sht40_Environment_Temp_Alarm_Threshold(&temp_threshold); 
    printf("\r\n Get temp_threshold = %d \r\n", temp_threshold);
    Environment_Temp_Alarm_Threshold_Cmd.data.int_value = temp_threshold;       //获取温度报警阈值


    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Environment_Humidity_Alarm_Threshold(){
    TAGID_S *temp_tagid_list[] = {&Environment_Humidity_Alarm_Threshold_Cmd};
    uint8_t humi_threshold=0;
    Get_Sht40_Environment_Humidity_Alarm_Threshold(&humi_threshold); 
    printf("\r\n Get humi_threshold = %d \r\n", humi_threshold);
    Environment_Humidity_Alarm_Threshold_Cmd.data.int_value = humi_threshold;       //获取湿度报警阈值

    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_HR_SPO2
static void Get_Heart_Rate(){
    TAGID_S *temp_tagid_list[] = {&Heart_Rate_Cmd};
    uint8_t hr,spo2;
    Get_Max30102_Data(&hr, &spo2);
    Heart_Rate_Cmd.data.int_value=hr;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_SpO2(){
    TAGID_S *temp_tagid_list[] = {&SpO2_Cmd};
    uint8_t hr,spo2;
    Get_Max30102_Data(&hr, &spo2);
    SpO2_Cmd.data.int_value=spo2;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_COLOUR
static void Get_Colour_Red(){
    TAGID_S *temp_tagid_list[] = {&Colour_Red_Cmd};
    Colour_Red_Cmd.data.int_value=Get_Red_Data();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Colour_Green(){
    TAGID_S *temp_tagid_list[] = {&Colour_Green_Cmd};
    Colour_Green_Cmd.data.int_value=Get_Green_Data();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Colour_Blue(){
    TAGID_S *temp_tagid_list[] = {&Colour_Blue_Cmd};
    Colour_Blue_Cmd.data.int_value=Get_Green_Data();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_MEASUREMENT_TEMP
static void Get_Body_Temp(){
    TAGID_S *temp_tagid_list[] = {&Measurement_Temp_Cmd};
    uint8_t temp;
    Get_IR_Measure_Temp(&temp);
    Measurement_Temp_Cmd.data.int_value=temp;
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif
#if MODULE_AIR
static void Get_CO2(){
    TAGID_S *temp_tagid_list[] = {&CO2_Cmd};
    CO2_Cmd.data.int_value=Get_CCS811_CO2();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_TVOC(){
    TAGID_S *temp_tagid_list[] = {&TVOC_Cmd};
    TVOC_Cmd.data.int_value=Get_CCS811_TVOC();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

static void Get_Air_Level(){
    TAGID_S *temp_tagid_list[] = {&Air_Quality_Cmd};
    Air_Quality_Cmd.data.int_value=Get_Air_Quality();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif
#if MODULE_ALCOHOL
static void Get_Alcohol(){
    TAGID_S *temp_tagid_list[] = {&Alcohol_Concentration_Cmd};
    Alcohol_Concentration_Cmd.data.int_value=Get_MIX2111D_Alcoho();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
static void Get_Alcohol_Alarm_Threshold(){
    TAGID_S *temp_tagid_list[] = {&Alcohol_Alarm_Threshold_Cmd};
    Alcohol_Alarm_Threshold_Cmd.data.int_value=Get_MIX2111D_Alcoho_Alarm_Threshold();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif
//get功能实现
static GLOBAL_CONTROL_S get_array[] = {
    #if MODULE_FIRE
    //火焰距离
    {&fire_distance_cmd,                        NULL,&get_fire_distance},
    //火焰距离报警阈值
    {&fire_alarm_distance_cmd,                  NULL,&get_fire_alarm_distance},
    #endif

    #if MODULE_SOMG
    //烟感温度
    {&Smog_Temp_cmd,                            NULL,&get_fire_distance},
    //烟感湿度
    {&Smog_Humidity_Cmd,                        NULL,&get_fire_alarm_distance},
    //烟雾浓度
    {&Smog_Scope_Cmd,                           NULL,&get_fire_alarm_distance},
    //烟雾报警阈值
    {&Smog_Alarm_Threshold_Cmd,                 NULL,&get_fire_alarm_distance},
    #endif

    #if MODULE_SOIL
    //土壤湿度
    {&Soil_Humidity_Cmd,                        NULL,&Get_Current_Soil_Humidity},
    //水泵开关阈值
    {&Water_Pump_OnOff_Threshold_Cmd,           NULL,&Get_Current_Water_Pump_Onoff_Threshold},
    #endif

    #if MODULE_TEMP_HUMI
    //环境温度
    {&Environment_Temp_Cmd,                     NULL,&Get_Environment_Temp},
    //环境湿度
    {&Environment_Humidity_Cmd,                 NULL,&Get_Environment_Humidity},
    //环境温度报警阈值
    {&Environment_Temp_Alarm_Threshold_Cmd,     NULL,&Get_Environment_Temp_Alarm_Threshold},
    //环境湿度报警阈值
    {&Environment_Humidity_Alarm_Threshold_Cmd, NULL,&Get_Environment_Humidity_Alarm_Threshold},
    #endif

    #if MODULE_ALCOHOL
    //酒精浓度
    {&Alcohol_Concentration_Cmd,                NULL,&Get_Alcohol},
    //酒精浓度报警阈值
    {&Alcohol_Alarm_Threshold_Cmd,              NULL,&Get_Alcohol_Alarm_Threshold},
    #endif

    #if MODULE_COLOUR
    //红色
    {&Colour_Red_Cmd,                           NULL,&Get_Colour_Red},
    //绿色
    {&Colour_Green_Cmd,                         NULL,&Get_Colour_Green},
    //蓝色
    {&Colour_Blue_Cmd,                          NULL,&Get_Colour_Blue},
    #endif

    #if MODULE_HR_SPO2
    //心率
    {&Heart_Rate_Cmd,                           NULL,&Get_Heart_Rate},
    //血氧
    {&SpO2_Cmd,                                 NULL,&Get_SpO2},
    #endif

    #if MODULE_AIR
    //空气质量
    {&Air_Quality_Cmd,                          NULL,&Get_Air_Level},
    // //空气质量报警阈值
    // {&Air_Quality_Alarm_Threshold_Cmd,          NULL,&get_fire_distance},
    //CO2
    {&CO2_Cmd,                                  NULL,&Get_CO2},
    //TVOC
    {&TVOC_Cmd,                                 NULL,&Get_TVOC},
    #endif

    #if MODULE_MEASUREMENT_TEMP
    //红外测温温度
    {&Measurement_Temp_Cmd,                     NULL,&Get_Body_Temp},
    #endif

    #if MODULE_ORIENTATION
    //经度
    {&Longitude_Cmd,                            NULL,&Get_Longitude},
    //维度
    {&Latitude_Cmd,                             NULL,&Get_Latitude},
    //压力
    {&Pressure_Cmd,                             NULL,&Get_Pressure},
    #endif

};
#define GET_ARRAY_LENGTH    (sizeof(get_array)/sizeof(get_array[0]))
handle_recv_app_to_mcu_get(TAGID_S recv_tag_id_list[], const int recv_tag_count){
    DEBUG_PRINTF("recv_tag_count is %d\r\n", recv_tag_count);
    for (size_t i = 0; i < recv_tag_count; i++)
    {
        //printf_tag_s(&(recv_tag_id_list[i]));
        //找到对应的cmd结构体，并设置缓存值
        for (size_t j = 0; j < GET_ARRAY_LENGTH; j++)
        {
            if (recv_tag_id_list[i].tagid == get_array[j].cmd_s->tagid)
            {
                get_array[j].report_svcid_func();
                break;
            }
        }
    }
}

void *thread_always_read_from_serial(void * argv)
{
    /* Initialize uart driver */
    serial_protocol_init();
    int temp_tag_count = 0;
    int ret = 0;
    unsigned char msg_type = 0;
    TAGID_S recv_tag_id_list[PROTOL_RECV_MAX_TAGID_COUNT];
    TAGID_S recv_tag_id_list_bank[PROTOL_RECV_MAX_TAGID_COUNT];
    DEBUG_PRINTF("i am in child thread\r\n");
    while (1)
    {
        adpater_thread_lock();
        temp_tag_count = 0;
        memset(recv_tag_id_list, 0, sizeof(recv_tag_id_list));
        ret = serial_protocol_read_multi(recv_tag_id_list, &temp_tag_count, &msg_type);
        if ((ret == HILINK_OK) && (temp_tag_count > 0))
        {
            switch (msg_type){
                case PROTOCOL_APP_TO_MCU_PUT_MSG_T:
                    handle_recv_app_to_mcu_put(recv_tag_id_list,temp_tag_count);
                    break;
                case PROTOCOL_APP_TO_MCU_GET_MSG_T:
                    handle_recv_app_to_mcu_get(recv_tag_id_list,temp_tag_count);
                    break;
                case PROTOCOL_MCU_TO_APP_GET_MSG_T:
                    break;
                case PROTOCOL_MCU_TO_WIFI_MOD_REPORT_MSG_T:
                    break;
                case PROTOCOL_MCU_TO_WIFI_MOD_GET_MSG_T:
                    break;
                default:
                    ERROR_PRINTF("Get error msg type: %02X\r\n", msg_type);
                    break;
            }
        }
        
        adpater_thread_unlock();
        //DEBUG_PRINTF("serial_loop_count is %d\r\n", serial_loop_count);
        usleep(50);
    }
}

void BluetoothDemo(void)
{
    osThreadAttr_t attr;

    attr.name       = "thread_always_read_from_serial";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = BLE_TASK_STACK_SIZE;
    attr.priority   = BLE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)thread_always_read_from_serial, NULL, &attr) == NULL) {
        printf("\r\n[BluetoothDemo] Falied to create thread_always_read_from_serial!\r\n");
    }else{
        printf("\r\n[BluetoothDemo] Succ to create thread_always_read_from_serial!\r\n");
    }
}

APP_FEATURE_INIT(BluetoothDemo);