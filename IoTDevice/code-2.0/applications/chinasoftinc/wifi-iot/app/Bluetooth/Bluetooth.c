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
#define BLE_TASK_STACK_SIZE         (1024*8)              //任务栈大小
#define BLE_TASK_PRIO               (10)                  //任务优先等级

#define CMPDATA_CYCLE               (2)                   //数据比较周期50ms*2             
//------------------------------------------------------------------------------
//数据更新模式
typedef enum{
    UPDATA_ALL,                     //全量更新
    UPDATA_CHANGE,                  //数据变化后更新
}TE_UPDATA_MODE;


TAGID_S Updata_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0000,
    .data.int_value = 0x0
};
//-------------------------------------------------------------------------------
//火焰检测对应tagid
#if MODULE_FIRE
extern uint8_t Get_Fire_State(void);        //获取火焰状态函数
TAGID_S Fire_State_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0001,
    .data.int_value = 0x0
};
#endif 
//-------------------------------------------------------------------------------
//可燃气体检测对应tagid
#if MODUIL_GAS
extern float Get_Aht20_Environment_Temp(void);          //获取环境温度函数
extern float Get_Aht20_Environment_Humidity(void);      //获取环境湿度函数
extern float Get_Gas_Sensor_Resistance(void);           //获取可燃气体检测sensor电阻值
//烟感温度
TAGID_S Aht20_Temp_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0003,
    .data.int_value = 0x0
};
//烟感湿度
TAGID_S Aht20_Humi_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0004,
    .data.int_value = 0x0
};
//烟雾浓度
TAGID_S Gas_Resistance_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0005,
    .data.int_value = 0x0
};
//烟雾报警阈值
// TAGID_S Smog_Alarm_Threshold_Cmd = {
//     .data_type = DTYPE_INT_ENUM,
//     .tagid = 0x0006,
//     .data.int_value = 0x0
// };
#endif
//-------------------------------------------------------------------------------
//土壤湿度检测对应tagid
#if MODULE_SOIL
extern char Set_Water_Pump_Onoff_Threshold(uint8_t HumiThreshold);      //设置湿度阈值
extern uint8_t Get_Water_Pump_Onoff_Threshold(void);                    //获取湿度阈值
extern uint8_t Get_Soil_Humi(void);                                     //获取湿度
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
//-------------------------------------------------------------------------------
//环境温湿度检测对应tagid
#if MODULE_TEMP_HUMI
extern char Set_Sht40_Environment_Temp_Alarm_Threshold(uint8_t TempThreshold);          //设置环境温度阈值
extern char Set_Sht40_Environment_Humidity_Alarm_Threshold(uint8_t HumiThreshold);      //设置环境湿度阈值
extern uint8_t Get_Sht40_Environment_Temp_Alarm_Threshold(void);                        //获取环境温度阈值
extern uint8_t Get_Sht40_Environment_Humidity_Alarm_Threshold(void);                    //获取环境湿度阈值                   
extern uint8_t Get_Sht40_Environment_Temp(void);                                        //获取环境温度
extern uint8_t Get_Sht40_Environment_Humidity(void);                                    //获取环境湿度
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
//-------------------------------------------------------------------------------
//酒精浓度检测对应tagid
#if MODULE_ALCOHOL
extern uint16_t Get_MIX2111D_Alcohol(void);                                         //获取酒精浓度值
extern uint16_t Get_MIX2111D_Alcohol_Alarm_Threshold(void);                         //获取酒精浓度报警阈值
extern void Set_MIX2111D_Alcohol_Alarm_Threshold(uint16_t Alcohol_Threshold);       //设置酒精报警阈值
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
//-------------------------------------------------------------------------------
//颜色检测对应tagid
#if MODULE_COLOUR
extern int Get_Red_Data(void);                  //获取红色色值
extern int Get_Green_Data(void);                //获取绿色色值
extern int Get_Blue_Data(void);                 //获取蓝色色值
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
//-------------------------------------------------------------------------------
//心率血氧检测对应tagid
#if MODULE_HR_SPO2
extern uint8_t Get_Max30102_HeartRate(void);            //获取心率值
extern uint8_t Get_Max30102_SpO2(void);                 //获取血氧值
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
//-------------------------------------------------------------------------------
//空气质量检测对应tagid                         
#if MODULE_AIR
extern uint16_t Get_CCS811_CO2(void);               //获取CO2值
extern uint16_t Get_CCS811_TVOC(void);              //获取TVOC值
extern uint8_t Get_Air_Quality(void);               //获取空气等级值
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
//-------------------------------------------------------------------------------
//红外测温对应tagid 
#if MODULE_MEASUREMENT_TEMP
extern uint8_t Get_IR_Measure_Temp(void);               //获取温度值
//红外测温温度
TAGID_S Measurement_Temp_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x0015,
    .data.int_value = 0x0
};
#endif
//-------------------------------------------------------------------------------
//位置信息tagid 
#if MODULE_ORIENTATION
extern int Get_Gps_Longitude(void);                     //获取经度
extern int Get_Gps_Latitude(void);                      //获取纬度
extern long Get_Bmp220_Pressure(void);                  //获取大气压强值
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
//-------------------------------------------------------------------------------
//交通灯tagid 
#if MODUIL_TRAFFIC_LIGHT
extern uint8_t Get_Traffic_Light_Mode(void);                    //获取交通灯模式
extern void Set_Traffic_Light_Mode(uint8_t Mode);               //设置交通灯模式
//交通灯模式
TAGID_S TrafficMode_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x001C,
    .data.int_value = 0x0
};
#endif
//-------------------------------------------------------------------------------
//人体感应、亮度感应tagid 
#if MODUIL_PIR
extern uint8_t Get_Light_State(void);               //获取光感状态
extern uint8_t Get_Human_Status(void);              //获取人体感应状态
//光照状态
TAGID_S Human_Status_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x001D,
    .data.int_value = 0x0
};

//人体感应状态
TAGID_S Light_State_Cmd = {
    .data_type = DTYPE_INT_ENUM,
    .tagid = 0x001E,
    .data.int_value = 0x0
};
#endif

/******************************************************************************\
                             Functions definitions
\******************************************************************************/

#if MODULE_SOIL
/*
* 函数名称 : Get_Current_Soil_Humidity
* 功能描述 : 上报土壤湿度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Current_Soil_Humidity();
*/
/******************************************************************************/ 
static void Get_Current_Soil_Humidity()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Soil_Humidity_Cmd};
    Soil_Humidity_Cmd.data.int_value=(int)Get_Soil_Humi();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Current_Water_Pump_Onoff_Threshold
* 功能描述 : 上报土壤湿度阈值(水泵开关控制对应湿度阈值)
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Current_Water_Pump_Onoff_Threshold();
*/
/******************************************************************************/ 
static void Get_Current_Water_Pump_Onoff_Threshold()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Water_Pump_OnOff_Threshold_Cmd};
    Water_Pump_OnOff_Threshold_Cmd.data.int_value=(int)Get_Water_Pump_Onoff_Threshold();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Put_Water_Pump_Onoff_Threshold
* 功能描述 : 设置土壤湿度阈值(水泵开关控制对应湿度阈值)
* 参    数 : 无
* 返回值   : 无
* 示    例 : Put_Water_Pump_Onoff_Threshold();
*/
/******************************************************************************/ 
static void Put_Water_Pump_Onoff_Threshold(TAGID_S * temp_tag_id_s)
/******************************************************************************/ 
{
    Set_Water_Pump_Onoff_Threshold((uint8_t)ntohl(temp_tag_id_s->data.int_value)); 
}
#endif


#if MODULE_FIRE
/*
* 函数名称 : Get_Acc_Fire_State
* 功能描述 : 上报火焰状态
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Acc_Fire_State();
*/
/******************************************************************************/ 
static void Get_Acc_Fire_State()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Fire_State_Cmd};
    Fire_State_Cmd.data.int_value=Get_Fire_State();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_ORIENTATION
/*
* 函数名称 : Get_Longitude
* 功能描述 : 上报经度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Longitude();
*/
/******************************************************************************/ 
static void Get_Longitude()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Longitude_Cmd};
    Longitude_Cmd.data.int_value = Get_Gps_Longitude();               //经度
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Latitude
* 功能描述 : 上报纬度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Latitude();
*/
/******************************************************************************/ 
static void Get_Latitude()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Latitude_Cmd};
    Latitude_Cmd.data.int_value = Get_Gps_Latitude();               //纬度
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Pressure
* 功能描述 : 上报压强
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Pressure();
*/
/******************************************************************************/ 
static void Get_Pressure()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Pressure_Cmd};
    Pressure_Cmd.data.int_value = (int)Get_Bmp220_Pressure();               //压强，单位：pa
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_TEMP_HUMI
/*
* 函数名称 : Get_Environment_Temp
* 功能描述 : 上报环境温度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Environment_Temp();
*/
/******************************************************************************/ 
static void Get_Environment_Temp()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Environment_Temp_Cmd};
    Environment_Temp_Cmd.data.int_value = Get_Sht40_Environment_Temp();                 //获取温度
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Environment_Humidity
* 功能描述 : 上报环境湿度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Environment_Humidity();
*/
/******************************************************************************/ 
static void Get_Environment_Humidity()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Environment_Humidity_Cmd};
    Environment_Humidity_Cmd.data.int_value = Get_Sht40_Environment_Humidity();                 //获取湿度
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Environment_Temp_Alarm_Threshold
* 功能描述 : 上报温度报警阈值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Environment_Temp_Alarm_Threshold();
*/
/******************************************************************************/ 
static void Get_Environment_Temp_Alarm_Threshold()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Environment_Temp_Alarm_Threshold_Cmd};
    Environment_Temp_Alarm_Threshold_Cmd.data.int_value = Get_Sht40_Environment_Temp_Alarm_Threshold();       //获取温度报警阈值
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Environment_Humidity_Alarm_Threshold
* 功能描述 : 上报湿度报警阈值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Environment_Humidity_Alarm_Threshold();
*/
/******************************************************************************/ 
static void Get_Environment_Humidity_Alarm_Threshold()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Environment_Humidity_Alarm_Threshold_Cmd};
    Environment_Humidity_Alarm_Threshold_Cmd.data.int_value = Get_Sht40_Environment_Humidity_Alarm_Threshold();       //获取湿度报警阈值
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Put_Environment_Temp_Alarm_Threshold
* 功能描述 : 设置温度报警阈值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Put_Environment_Temp_Alarm_Threshold(&temp_tag_id_s);
*/
/******************************************************************************/ 
static void Put_Environment_Temp_Alarm_Threshold(TAGID_S *temp_tag_id_s)
/******************************************************************************/ 
{
    uint8_t tmp;

    tmp = (uint8_t)ntohl(temp_tag_id_s->data.int_value);
    Set_Sht40_Environment_Temp_Alarm_Threshold(tmp);
}
/*
* 函数名称 : Put_Environment_Humidity_Alarm_Threshold
* 功能描述 : 设置湿度报警阈值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Put_Environment_Humidity_Alarm_Threshold(&temp_tag_id_s);
*/
/******************************************************************************/ 
static void Put_Environment_Humidity_Alarm_Threshold(TAGID_S *temp_tag_id_s)
/******************************************************************************/ 
{
    uint8_t tmp;
    tmp = (uint8_t)ntohl(temp_tag_id_s->data.int_value);
    Set_Sht40_Environment_Humidity_Alarm_Threshold(tmp);
}
#endif

#if MODULE_HR_SPO2
/*
* 函数名称 : Get_Heart_Rate
* 功能描述 : 上报心率值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Heart_Rate();
*/
/******************************************************************************/ 
static void Get_Heart_Rate()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Heart_Rate_Cmd};
    Heart_Rate_Cmd.data.int_value=Get_Max30102_HeartRate();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_SpO2
* 功能描述 : 上报血氧值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_SpO2();
*/
/******************************************************************************/ 
static void Get_SpO2()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&SpO2_Cmd};
    SpO2_Cmd.data.int_value=Get_Max30102_SpO2();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_COLOUR
/*
* 函数名称 : Get_Colour_Red
* 功能描述 : 上报红色色值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Colour_Red();
*/
/******************************************************************************/ 
static void Get_Colour_Red()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Colour_Red_Cmd};
    Colour_Red_Cmd.data.int_value=Get_Red_Data();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Colour_Green
* 功能描述 : 上报绿色色值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Colour_Green();
*/
/******************************************************************************/ 
static void Get_Colour_Green()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Colour_Green_Cmd};
    Colour_Green_Cmd.data.int_value=Get_Green_Data();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Colour_Blue
* 功能描述 : 上报蓝色色值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Colour_Blue();
*/
/******************************************************************************/ 
static void Get_Colour_Blue()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Colour_Blue_Cmd};
    Colour_Blue_Cmd.data.int_value=Get_Green_Data();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif

#if MODULE_MEASUREMENT_TEMP
/*
* 函数名称 : Get_Body_Temp
* 功能描述 : 上报人体温度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Body_Temp();
*/
/******************************************************************************/ 
static void Get_Body_Temp()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Measurement_Temp_Cmd};
    Measurement_Temp_Cmd.data.int_value=Get_IR_Measure_Temp();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif
#if MODULE_AIR
/*
* 函数名称 : Get_CO2
* 功能描述 : 上报CO2
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_CO2();
*/
/******************************************************************************/ 
static void Get_CO2()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&CO2_Cmd};
    CO2_Cmd.data.int_value=Get_CCS811_CO2();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_TVOC
* 功能描述 : 上报TVOC
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_TVOC();
*/
/******************************************************************************/ 
static void Get_TVOC()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&TVOC_Cmd};
    TVOC_Cmd.data.int_value=Get_CCS811_TVOC();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Air_Level
* 功能描述 : 上报控制质量等级
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Air_Level();
*/
/******************************************************************************/ 
static void Get_Air_Level()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Air_Quality_Cmd};
    Air_Quality_Cmd.data.int_value=Get_Air_Quality();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif
#if MODULE_ALCOHOL
/*
* 函数名称 : Get_Alcohol
* 功能描述 : 上报酒精浓度
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Alcohol();
*/
/******************************************************************************/ 
static void Get_Alcohol()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Alcohol_Concentration_Cmd};
    Alcohol_Concentration_Cmd.data.int_value=Get_MIX2111D_Alcohol();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Alcohol_Alarm_Threshold
* 功能描述 : 上报酒精报警浓度阈值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Alcohol_Alarm_Threshold();
*/
/******************************************************************************/ 
static void Get_Alcohol_Alarm_Threshold()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Alcohol_Alarm_Threshold_Cmd};
    Alcohol_Alarm_Threshold_Cmd.data.int_value=Get_MIX2111D_Alcohol_Alarm_Threshold();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Put_Alcohol_Alarm_Threshold_Cmd
* 功能描述 : 设置酒精报警浓度阈值
* 参    数 : 无
* 返回值   : 无
* 示    例 : Put_Alcohol_Alarm_Threshold_Cmd(&temp_tag_id_s);
*/
/******************************************************************************/ 
static void Put_Alcohol_Alarm_Threshold_Cmd(TAGID_S * temp_tag_id_s)
/******************************************************************************/ 
{
    Set_MIX2111D_Alcohol_Alarm_Threshold((uint16_t)ntohl(temp_tag_id_s->data.int_value));
}
#endif


#if MODUIL_TRAFFIC_LIGHT
/*
* 函数名称 : Put_Traffic_Light_Mode
* 功能描述 : 设置交通灯模式
* 参    数 : 无
* 返回值   : 无
* 示    例 : Put_Traffic_Light_Mode(&temp_tag_id_s);
*/
/******************************************************************************/ 
static void Put_Traffic_Light_Mode(TAGID_S * temp_tag_id_s)
/******************************************************************************/ 
{
    Set_Traffic_Light_Mode((uint8_t)ntohl(temp_tag_id_s->data.int_value));
}
/*
* 函数名称 : Get_TrafficLight_Mode
* 功能描述 : 获取交通灯模式
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_TrafficLight_Mode();
*/
/******************************************************************************/ 
static void Get_TrafficLight_Mode()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&TrafficMode_Cmd};
    TrafficMode_Cmd.data.int_value=Get_Traffic_Light_Mode();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}

#endif

#if MODUIL_PIR
/*
* 函数名称 : Get_PIR_Light_State
* 功能描述 : 上报光照状态
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_PIR_Light_State();
*/
/******************************************************************************/ 
static void Get_PIR_Light_State()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Light_State_Cmd};
    Light_State_Cmd.data.int_value=Get_Light_State();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_PIR_Human_Status
* 功能描述 : 上报人体感应状态
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_PIR_Human_Status();
*/
/******************************************************************************/ 
static void Get_PIR_Human_Status()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Human_Status_Cmd};
    Human_Status_Cmd.data.int_value=Get_Human_Status();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif


#if MODUIL_GAS
/*
* 函数名称 : Get_Aht20_Temp
* 功能描述 : 上报温度值(可燃气体板)
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Aht20_Temp();
*/
/******************************************************************************/ 
static void Get_Aht20_Temp()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Aht20_Temp_Cmd};
    Aht20_Temp_Cmd.data.int_value=Get_Aht20_Environment_Temp();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Aht20_Humi
* 功能描述 : 上报湿度值(可燃气体板)
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Aht20_Humi();
*/
/******************************************************************************/ 
static void Get_Aht20_Humi()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Aht20_Humi_Cmd};
    Aht20_Humi_Cmd.data.int_value=Get_Aht20_Environment_Humidity();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
/*
* 函数名称 : Get_Aht20_Humi
* 功能描述 : 上报可燃气体传感器阻值(可燃气体板)
* 参    数 : 无
* 返回值   : 无
* 示    例 : Get_Aht20_Humi();
*/
/******************************************************************************/ 
static void Get_Gas_Resistance()
/******************************************************************************/ 
{
    TAGID_S *temp_tagid_list[] = {&Gas_Resistance_Cmd};
    Gas_Resistance_Cmd.data.int_value=Get_Gas_Sensor_Resistance();
    serial_protocol_control_write_multi(temp_tagid_list, (sizeof(temp_tagid_list) / sizeof(temp_tagid_list[0])), PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
}
#endif
//-------------------------------------------------------------------------------
//功能实现句柄
static GLOBAL_CONTROL_S tag_array[] = {
    //属性对应的CMD结构体                        APP下发控制回调函数指针                       本地上报数据函数指针                        
    #if MODULE_FIRE
    //火焰距离
    {&Fire_State_Cmd,                           NULL,                                       &Get_Acc_Fire_State},
    #endif

    #if MODULE_SOIL
    //土壤湿度
    {&Soil_Humidity_Cmd,                        NULL,                                       &Get_Current_Soil_Humidity},
    //水泵开关阈值
    {&Water_Pump_OnOff_Threshold_Cmd,           &Put_Water_Pump_Onoff_Threshold,            &Get_Current_Water_Pump_Onoff_Threshold},
    #endif

    #if MODULE_TEMP_HUMI
    //环境温度
    {&Environment_Temp_Cmd,                     NULL,                                       &Get_Environment_Temp},
    //环境湿度
    {&Environment_Humidity_Cmd,                 NULL,                                       &Get_Environment_Humidity},
    //环境温度报警阈值
    {&Environment_Temp_Alarm_Threshold_Cmd,     &Put_Environment_Temp_Alarm_Threshold,      &Get_Environment_Temp_Alarm_Threshold},
    //环境湿度报警阈值
    {&Environment_Humidity_Alarm_Threshold_Cmd, &Put_Environment_Humidity_Alarm_Threshold,  &Get_Environment_Humidity_Alarm_Threshold},
    #endif

    #if MODULE_ALCOHOL
    //酒精浓度
    {&Alcohol_Concentration_Cmd,                NULL,                                       &Get_Alcohol},
    //酒精浓度报警阈值
    {&Alcohol_Alarm_Threshold_Cmd,              &Put_Alcohol_Alarm_Threshold_Cmd,           &Get_Alcohol_Alarm_Threshold},
    #endif

    #if MODULE_COLOUR
    //红色
    {&Colour_Red_Cmd,                           NULL,                                       &Get_Colour_Red},
    //绿色
    {&Colour_Green_Cmd,                         NULL,                                       &Get_Colour_Green},
    //蓝色
    {&Colour_Blue_Cmd,                          NULL,                                       &Get_Colour_Blue},
    #endif

    #if MODULE_HR_SPO2
    //心率
    {&Heart_Rate_Cmd,                           NULL,                                       &Get_Heart_Rate},
    //血氧
    {&SpO2_Cmd,                                 NULL,                                       &Get_SpO2},
    #endif

    #if MODULE_AIR
    //空气质量
    {&Air_Quality_Cmd,                          NULL,                                       &Get_Air_Level},
    // //空气质量报警阈值
    // {&Air_Quality_Alarm_Threshold_Cmd,          NULL,&get_fire_distance},
    //CO2
    {&CO2_Cmd,                                  NULL,                                       &Get_CO2},
    //TVOC
    {&TVOC_Cmd,                                 NULL,                                       &Get_TVOC},
    #endif

    #if MODULE_MEASUREMENT_TEMP
    //红外测温温度
    {&Measurement_Temp_Cmd,                     NULL,                                       &Get_Body_Temp},
    #endif

    #if MODULE_ORIENTATION
    //经度
    {&Longitude_Cmd,                            NULL,                                       &Get_Longitude},
    //维度
    {&Latitude_Cmd,                             NULL,                                       &Get_Latitude},
    //压力
    {&Pressure_Cmd,                             NULL,                                       &Get_Pressure},
    #endif

    #if MODUIL_TRAFFIC_LIGHT
    //交通灯模式
    {&TrafficMode_Cmd,                          &Put_Traffic_Light_Mode,                    &Get_TrafficLight_Mode},
    #endif

    #if MODUIL_PIR
    //光照状态
    {&Light_State_Cmd,                          NULL,                                       &Get_PIR_Light_State},
    //人体感应状态
    {&Human_Status_Cmd,                         NULL,                                       &Get_PIR_Human_Status},
    #endif

    #if MODUIL_GAS
    //温度
    {&Aht20_Humi_Cmd,                           NULL,                                       &Get_Aht20_Temp},
    //湿度
    {&Aht20_Humi_Cmd,                           NULL,                                       &Get_Aht20_Humi},
    //阻值
    {&Gas_Resistance_Cmd,                       NULL,                                       &Get_Gas_Resistance},
    #endif
};

#define TAG_ARRAY_LENGTH    (sizeof(tag_array)/sizeof(tag_array[0]))
/*
* 函数名称 : handle_recv_data
* 功能描述 : 接收蓝牙透传数据处理
* 参    数 : recv_tag_id_list - tagid地址
             recv_tag_count - tagid总数
             msg_type - 消息类型
* 返回值   : 无
* 示    例 : handle_recv_data();
*/
/******************************************************************************/ 
static void handle_recv_data(TAGID_S recv_tag_id_list[], const int recv_tag_count,uint8_t msg_type)
/******************************************************************************/ 
{
    DEBUG_PRINTF("recv_tag_count is %d\r\n", recv_tag_count);
    for (size_t i = 0; i < recv_tag_count; i++)
    {
        //printf_tag_s(&(recv_tag_id_list[i]));
        //找到对应的cmd结构体，并设置缓存值
        for (size_t j = 0; j < TAG_ARRAY_LENGTH; j++)
        {
            if (recv_tag_id_list[i].tagid == tag_array[j].cmd_s->tagid)
            {
                if(msg_type == PROTOCOL_APP_TO_MCU_PUT_MSG_T)               //APP下发控制
                {
                    tag_array[j].set_prop_func(&(recv_tag_id_list[i]));
                }
                else if(msg_type == PROTOCOL_APP_TO_MCU_GET_MSG_T)          //上报本地状态
                {
                    tag_array[j].report_svcid_func();
                }
                break;
            }
        }
    }
}

/*
* 函数名称 : Sensor_Update
* 功能描述 : sensor数据更新
* 参    数 : Mode - 数据更新模式
* 返回值   : 无
* 示    例 : Sensor_Update();
*/
/******************************************************************************/ 
void Sensor_Update(TE_UPDATA_MODE Mode)
/******************************************************************************/ 
{
    uint8_t tag_cnt = 0;
    uint8_t tag_num = 0;
    bool updata_flag = true;
    TAGID_S *temp_tagid_list[30] = {NULL};

    updata_flag = false;
    tag_cnt = 0;
    tag_num = 0;
    #if MODULE_FIRE
    uint8_t fire_state;    

    temp_tagid_list[tag_num++] = &Fire_State_Cmd;
   
    fire_state = Get_Fire_State();
    if(Fire_State_Cmd.data.int_value != fire_state || Mode == UPDATA_ALL)
    {
        Fire_State_Cmd.data.int_value = fire_state;
        updata_flag = true;
        tag_cnt += 1;
    }
    #endif


    #if MODULE_SOIL
    uint8_t soil_humidity;
    uint8_t soil_humi_threshold;
    
    temp_tagid_list[tag_num++] = &Soil_Humidity_Cmd;
    temp_tagid_list[tag_num++] = &Water_Pump_OnOff_Threshold_Cmd;

    soil_humidity = Get_Soil_Humi();
    soil_humi_threshold = Get_Water_Pump_Onoff_Threshold();
    if(Soil_Humidity_Cmd.data.int_value != soil_humidity || Water_Pump_OnOff_Threshold_Cmd.data.int_value != soil_humi_threshold || Mode == UPDATA_ALL)
    {
        Soil_Humidity_Cmd.data.int_value = soil_humidity;
        Water_Pump_OnOff_Threshold_Cmd.data.int_value = soil_humi_threshold;
        updata_flag = true;
    }
    #endif

    #if MODULE_ALCOHOL
    uint16_t alcohol;
    uint16_t alcoholthreshold;

    temp_tagid_list[tag_num++] = &Alcohol_Concentration_Cmd;
    temp_tagid_list[tag_num++] = &Alcohol_Alarm_Threshold_Cmd;
    alcohol = Get_MIX2111D_Alcohol();
    alcoholthreshold = Get_MIX2111D_Alcohol_Alarm_Threshold();
    if(Alcohol_Concentration_Cmd.data.int_value != alcohol || Alcohol_Alarm_Threshold_Cmd.data.int_value != alcoholthreshold || Mode == UPDATA_ALL)
    {
        Alcohol_Concentration_Cmd.data.int_value = alcohol;
        Alcohol_Alarm_Threshold_Cmd.data.int_value = alcoholthreshold;
        updata_flag = true;
    }
    #endif

    #if MODULE_AIR
    uint16_t co2;
    uint16_t tvoc;
    uint8_t airquality;

    temp_tagid_list[tag_num++] = &CO2_Cmd;
    temp_tagid_list[tag_num++] = &TVOC_Cmd;
    temp_tagid_list[tag_num++] = &Air_Quality_Cmd;

    co2 = Get_CCS811_CO2();
    tvoc = Get_CCS811_TVOC();
    airquality = Get_Air_Quality();

    if(CO2_Cmd.data.int_value != co2 || TVOC_Cmd.data.int_value != tvoc || Air_Quality_Cmd.data.int_value != airquality || Mode == UPDATA_ALL)
    {
        CO2_Cmd.data.int_value = co2;
        TVOC_Cmd.data.int_value = tvoc;
        Air_Quality_Cmd.data.int_value = airquality;
        updata_flag = true;
    }
    #endif

    #if MODULE_ORIENTATION
    int longitude;
    int latitude;
    long Press;

    temp_tagid_list[tag_num++] = &Longitude_Cmd;
    temp_tagid_list[tag_num++] = &Latitude_Cmd;
    temp_tagid_list[tag_num++] = &Pressure_Cmd;

    longitude = Get_Gps_Longitude(); 
    latitude = Get_Gps_Latitude(); 
    Press = Get_Bmp220_Pressure(); 

    if(Longitude_Cmd.data.int_value != longitude || Latitude_Cmd.data.int_value != latitude || Pressure_Cmd.data.int_value != Press || Mode == UPDATA_ALL)
    {
        Longitude_Cmd.data.int_value = longitude;
        Latitude_Cmd.data.int_value = latitude;
        Pressure_Cmd.data.int_value = Press;
        updata_flag = true;
    }
    #endif

    #if MODULE_MEASUREMENT_TEMP
    uint8_t temp;

    temp_tagid_list[tag_num++] = &Measurement_Temp_Cmd;
    temp = Get_IR_Measure_Temp();

    if(Measurement_Temp_Cmd.data.int_value != temp || Mode == UPDATA_ALL)
    {
        Measurement_Temp_Cmd.data.int_value = temp;
        updata_flag = true;
    }
    #endif

    #if MODULE_HR_SPO2
    uint8_t hr,spo2;

    temp_tagid_list[tag_num++] = &Heart_Rate_Cmd;
    temp_tagid_list[tag_num++] = &SpO2_Cmd;

    hr = Get_Max30102_HeartRate();
    spo2 = Get_Max30102_SpO2();

    if(Heart_Rate_Cmd.data.int_value != hr || SpO2_Cmd.data.int_value != spo2 || Mode == UPDATA_ALL)
    {
        Heart_Rate_Cmd.data.int_value = hr;
        SpO2_Cmd.data.int_value = spo2;
        updata_flag = true;
    }
    #endif

    #if MODULE_COLOUR
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    temp_tagid_list[tag_num++] = &Colour_Red_Cmd;
    temp_tagid_list[tag_num++] = &Colour_Green_Cmd;
    temp_tagid_list[tag_num++] = &Colour_Blue_Cmd;

    red = Get_Red_Data();
    green = Get_Green_Data();
    blue = Get_Blue_Data();

    if(Colour_Red_Cmd.data.int_value != red || Colour_Green_Cmd.data.int_value != green || Colour_Blue_Cmd.data.int_value != blue || Mode == UPDATA_ALL)
    {
        Colour_Red_Cmd.data.int_value = red;
        Colour_Green_Cmd.data.int_value = green;
        Colour_Blue_Cmd.data.int_value = blue;
        updata_flag = true;
    }
    #endif

    #if MODULE_TEMP_HUMI

    uint8_t temp=0;
    uint8_t humi=0;
    uint8_t temp_threshold=0;
    uint8_t humi_threshold=0;

    temp_tagid_list[tag_num++] = &Environment_Temp_Cmd;
    temp_tagid_list[tag_num++] = &Environment_Humidity_Cmd;
    temp_tagid_list[tag_num++] = &Environment_Temp_Alarm_Threshold_Cmd;
    temp_tagid_list[tag_num++] = &Environment_Humidity_Alarm_Threshold_Cmd;

    temp = Get_Sht40_Environment_Temp(); 
    humi = Get_Sht40_Environment_Humidity();
    temp_threshold = Get_Sht40_Environment_Temp_Alarm_Threshold();
    humi_threshold = Get_Sht40_Environment_Humidity_Alarm_Threshold();

    if(Environment_Temp_Cmd.data.int_value != temp || Environment_Humidity_Cmd.data.int_value != humi || \
        Environment_Temp_Alarm_Threshold_Cmd.data.int_value != temp_threshold ||\
        Environment_Humidity_Alarm_Threshold_Cmd.data.int_value != humi_threshold || Mode == UPDATA_ALL)
    {
        Environment_Temp_Cmd.data.int_value = temp;
        Environment_Humidity_Cmd.data.int_value = humi;
        Environment_Temp_Alarm_Threshold_Cmd.data.int_value = temp_threshold;
        Environment_Humidity_Alarm_Threshold_Cmd.data.int_value = humi_threshold;
        updata_flag = true;
    }

    #endif

    #if MODUIL_TRAFFIC_LIGHT
    uint8_t trafficmode;

    temp_tagid_list[tag_num++] = &TrafficMode_Cmd;
    trafficmode = Get_Traffic_Light_Mode();

    if(TrafficMode_Cmd.data.int_value != trafficmode || Mode == UPDATA_ALL)
    {
        TrafficMode_Cmd.data.int_value = trafficmode;
        updata_flag = true;
    }
    #endif

    #if MODUIL_PIR
    uint8_t lightstate;
    uint8_t humanstatus;

    temp_tagid_list[tag_num++] = &Light_State_Cmd;
    temp_tagid_list[tag_num++] = &Human_Status_Cmd;
    lightstate = Get_Light_State();
    humanstatus = Get_Human_Status();


    if(Light_State_Cmd.data.int_value != lightstate || Human_Status_Cmd.data.int_value != humanstatus || Mode == UPDATA_ALL)
    {
        Light_State_Cmd.data.int_value = lightstate;
        Human_Status_Cmd.data.int_value = humanstatus;
        updata_flag = true;
    }
    #endif

    #if MODUIL_GAS
    int temp;
    int humi;
    int resistance;

    temp_tagid_list[tag_num++] = &Aht20_Temp_Cmd;
    temp_tagid_list[tag_num++] = &Aht20_Humi_Cmd;
    temp_tagid_list[tag_num++] = &Gas_Resistance_Cmd;
    temp = Get_Aht20_Environment_Temp();
    humi = Get_Aht20_Environment_Humidity();
    resistance = Get_Gas_Sensor_Resistance();

    if(Aht20_Temp_Cmd.data.int_value != temp || Aht20_Humi_Cmd.data.int_value != humi || Gas_Resistance_Cmd.data.int_value != resistance || Mode == UPDATA_ALL)
    {
        Aht20_Temp_Cmd.data.int_value = temp;
        Aht20_Humi_Cmd.data.int_value = humi;
        Gas_Resistance_Cmd.data.int_value = resistance;
        updata_flag = true;
    }
    #endif


    if(updata_flag == true)
    {
        serial_protocol_control_write_multi(temp_tagid_list, tag_num, PROTOCOL_MCU_TO_APP_REPORT_MSG_T);
    }
}
/*
* 函数名称 : thread_always_read_from_serial
* 功能描述 : 蓝牙串口数据接收任务
* 参    数 : argv - 任务参数
* 返回值   : 无
* 示    例 : thread_always_read_from_serial(&argv);
*/
/******************************************************************************/ 
void *thread_always_read_from_serial(void * argv)
/******************************************************************************/ 
{
    /* Initialize uart driver */
    serial_protocol_init();
    int temp_tag_count = 0;
    int ret = 0;
    static uint8_t cmptimecnt = 0;
    unsigned char msg_type = 0;
    TAGID_S recv_tag_id_list[PROTOL_RECV_MAX_TAGID_COUNT];
    TAGID_S local_tag_id_list[PROTOL_RECV_MAX_TAGID_COUNT] = {0};
    TAGID_S local_tag_id_list_bank[PROTOL_RECV_MAX_TAGID_COUNT] = {0};
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
                    printf("\r\n------------------PROTOCOL_APP_TO_MCU_PUT_MSG_T------------------\r\n");
                    handle_recv_data(recv_tag_id_list,temp_tag_count,PROTOCOL_APP_TO_MCU_PUT_MSG_T);
                    break;
                case PROTOCOL_APP_TO_MCU_GET_MSG_T:
                    printf("\r\n------------------PROTOCOL_APP_TO_MCU_GET_MSG_T------------------\r\n");
                    // handle_recv_data(recv_tag_id_list,temp_tag_count,PROTOCOL_APP_TO_MCU_GET_MSG_T);
                    Sensor_Update(UPDATA_ALL);
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
        else if(++cmptimecnt >= CMPDATA_CYCLE)
        {
            cmptimecnt = 0;
            Sensor_Update(UPDATA_CHANGE);
        }
        
        adpater_thread_unlock();
        
        //DEBUG_PRINTF("serial_loop_count is %d\r\n", serial_loop_count);
        usleep(50);
    }
}
/*
* 函数名称 : BluetoothDemo
* 功能描述 : 蓝牙应用初始化
* 参    数 : 无
* 返回值   : 无
* 示    例 : BluetoothDemo();
*/
/******************************************************************************/ 
void BluetoothDemo(void)
/******************************************************************************/ 
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
/******************************* End of File (C) ******************************/