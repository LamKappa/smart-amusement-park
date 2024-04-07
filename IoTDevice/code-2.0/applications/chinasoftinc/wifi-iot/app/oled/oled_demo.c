/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  oled_demo.c
**  功能描述 :  oled显示示例
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
#include <stdbool.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"

// #include "oled_ssd1306.h"
#include "../Bluetooth/include/ly_evk_config.h"
#include "oled.h"

/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define OLED_TASK_STACK_SIZE    4096            //任务栈大小
#define OLED_TASK_PRIO          25              //任务优先等级

#if MODULE_FIRE
extern uint8_t Get_Fire_State(void);
#endif

#if MODULE_ORIENTATION
extern int Get_Gps_Longitude(void);
extern int Get_Gps_Latitude(void);
extern long Get_Bmp220_Pressure(void) ;
#endif
#if MODULE_SOIL
extern uint8_t Get_Water_Pump_Onoff_Threshold(void);
extern uint8_t Get_Soil_Humi(void);
#endif
#if MODULE_TEMP_HUMI
extern uint8_t Get_Sht40_Environment_Temp_Alarm_Threshold(void);
extern uint8_t Get_Sht40_Environment_Humidity_Alarm_Threshold(void);
extern uint8_t Get_Sht40_Environment_Temp(void);
extern uint8_t Get_Sht40_Environment_Humidity(void);
#endif
#if MODULE_COLOUR
extern int Get_Red_Data(void);
extern int Get_Green_Data(void);
extern int Get_Blue_Data(void);
#endif

#if MODULE_HR_SPO2
extern uint8_t Get_Max30102_HeartRate(void);
extern uint8_t Get_Max30102_SpO2(void);
#endif

#if MODULE_MEASUREMENT_TEMP
extern uint8_t Get_IR_Measure_Temp(void);
#endif

#if MODULE_AIR
extern uint16_t Get_CCS811_CO2(void);
extern uint16_t Get_CCS811_TVOC(void);
extern uint8_t Get_Air_Quality(void);
#endif

#if MODULE_ALCOHOL
extern uint16_t Get_MIX2111D_Alcohol(void);
extern uint16_t Get_MIX2111D_Alcohol_Alarm_Threshold(void);
#endif

#if MODUIL_TRAFFIC_LIGHT
extern uint8_t Get_Traffic_Light_Mode(void);
#endif

#if MODUIL_PIR
extern uint8_t Get_Human_Status(void);
extern uint8_t Get_Light_State(void);
#endif

#if MODUIL_GAS
extern float Get_Aht20_Environment_Temp(void);
extern float Get_Aht20_Environment_Humidity(void);
extern float Get_Gas_Sensor_Resistance(void);
#endif

#if MODULE_QR
extern int facilityId;
extern int facilityType;
extern int deviceId;
#endif
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : OledTask
* 功能描述 : oled显示任务
* 参    数 : arg - 任务参数
* 返回值   : 空
* 示    例 : OledTask(&argument);
*/
/******************************************************************************/ 
static void OledTask(void *arg)
/******************************************************************************/ 
{
    
    (void)arg;
    char str[128]= {0};
    int n;
    bool updata_flag = true;
    static bool startup_flag = true;
    uint8_t data_num = 0;
    uint8_t y_num = 0;

    int font_size = 12;
    
    OLED_Init();
    OLED_Clear(); 
	OLED_Refresh_Gram();	 			
    while(1)
    {
        usleep(1000*100);
        OLED_Clear();                                                   //清理显示缓存
        data_num = 0;
        y_num = 0;
        updata_flag = false;
        #if MODULE_ORIENTATION
        static int longitude = 0;
        static int latitude = 0;
        static long Pressure = 0;
        data_num += 3;
        if(longitude != Get_Gps_Longitude() || latitude != Get_Gps_Latitude() || Pressure != Get_Bmp220_Pressure()) //判读数据是否发生变化
        {
            updata_flag = true;
        }
        longitude = Get_Gps_Longitude();                                //获取经度
        latitude = Get_Gps_Latitude();                                  //获取纬度
        Pressure = Get_Bmp220_Pressure();                               //获取气压
        //刷新显示缓存
        sprintf(str, "Longitude:%.2f", (float)longitude/10000);
        OLED_ShowString(0,((y_num++)*16),str,16);  
        sprintf(str, "Latitude:%.2f", (float)latitude/10000);
        OLED_ShowString(0,((y_num++)*16),str,16);
        sprintf(str, "Press:%d", Pressure);
        OLED_ShowString(0,((y_num++)*16),str,16);
        #endif
        #if MODULE_SOIL
        static uint8_t soil_humi = 255;
        static uint8_t soil_humiThreshold = 255;
        data_num += 2;
        if(soil_humi != Get_Soil_Humi() || soil_humiThreshold != Get_Water_Pump_Onoff_Threshold())  //判读数据是否发生变化
        {
            updata_flag = true;
        } 
        soil_humi = Get_Soil_Humi();                                    //获取土壤湿度
        soil_humiThreshold = Get_Water_Pump_Onoff_Threshold();          //获取水泵开启阈值
        //刷新显示缓存
        sprintf(str, "Soilhumi:%d%s", soil_humi,"%");               
        OLED_ShowString(0,((y_num++)*16),str,16);
        sprintf(str, "Threshold:%d%s", soil_humiThreshold,"%");
        OLED_ShowString(0,((y_num++)*16),str,16);
        #endif
        #if MODULE_TEMP_HUMI
        static uint8_t humi = 255;
        static uint8_t temp = 255;
        data_num += 2;
        if(temp != Get_Sht40_Environment_Temp() || humi != Get_Sht40_Environment_Humidity())    //判读数据是否发生变化
        {
            updata_flag = true;
        }
        temp = Get_Sht40_Environment_Temp();                            //获取环境温度
        humi = Get_Sht40_Environment_Humidity();                        //获取环境湿度
        //刷新显示缓存
        sprintf(str, "temp:%d", temp);
        OLED_ShowString(0,((y_num++)*16),str,16);
        sprintf(str, "humi:%d%s", humi,"%");
        OLED_ShowString(0,((y_num++)*16),str,16);
        #endif
        #if MODULE_COLOUR
        static int r = 0;
        static int g = 0; 
        static int b = 0;
        data_num += 3;
        if(r != Get_Red_Data() || g != Get_Green_Data() || b != Get_Blue_Data())        //判读数据是否发生变化
        {
            updata_flag = true;
        }
        r = Get_Red_Data();                             //获取红色值
        g = Get_Green_Data();                           //获取绿色值
        b = Get_Blue_Data();                            //获取蓝色值
        //刷新显示缓存
        sprintf(str, "Red:%d", r);
        OLED_ShowString(0,((y_num++)*16),str,16);
        sprintf(str, "Green:%d", g);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        sprintf(str, "Blue:%d", b);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        #endif
        #if MODULE_HR_SPO2
        static uint8_t heartrate=0xff;
        static uint8_t spo2=0xff;
        data_num += 2;
        if(heartrate != Get_Max30102_HeartRate() || spo2 != Get_Max30102_SpO2())
        {
            updata_flag = true;
        }
        heartrate = Get_Max30102_HeartRate();               //获取心率
        spo2 = Get_Max30102_SpO2();                         //获取血氧
        //刷新显示缓存
        sprintf(str, "HeartRate:%d", heartrate);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        sprintf(str, "SpO2:%d", spo2); 
        OLED_ShowString(0,((y_num++)*16),str,16); 
        #endif
        #if MODULE_MEASUREMENT_TEMP
        static uint8_t temp = 0;
        data_num += 1;
        if(temp == Get_IR_Measure_Temp())
        {
            updata_flag = true;
        }
        temp = Get_IR_Measure_Temp();                       //获取检测温度
        //刷新显示缓存
        sprintf(str, "MeasureTemp:%d", temp);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        #endif
        #if MODULE_AIR
        static uint16_t CO2 = 0xffff;
        static uint16_t TVOC = 0xffff;
        static uint8_t AirQuality = 0;
        data_num += 3;
        if(CO2 != Get_CCS811_CO2() || TVOC != Get_CCS811_TVOC() || AirQuality != Get_Air_Quality())
        {
            updata_flag = true;
        }
        CO2 = Get_CCS811_CO2();                             //获取CO2浓度
        TVOC = Get_CCS811_TVOC();                           //获取TVOC
        AirQuality = Get_Air_Quality();                     //获取空气质量
        //刷新显示缓存
        sprintf(str, "CO2:%d", CO2);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        sprintf(str, "TVOC:%d", TVOC);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        sprintf(str, "AirQuality:%d", AirQuality);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        #endif
        #if MODULE_ALCOHOL
        static uint16_t alcohol = 0;
        static uint16_t alcoholthreshold = 0;
        data_num += 2;
        if(alcohol != Get_MIX2111D_Alcohol() || alcoholthreshold != Get_MIX2111D_Alcohol_Alarm_Threshold())
        {
            updata_flag = true;
        }
        alcohol = Get_MIX2111D_Alcohol();                                   //获取酒精浓度
        alcoholthreshold = Get_MIX2111D_Alcohol_Alarm_Threshold();          //获取酒精浓度报警阈值
        //刷新显示缓存
        sprintf(str, "Alcohol:%.1f", (float)alcohol/10);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        sprintf(str, "Threshold:%.1f", (float)alcoholthreshold/10);
        OLED_ShowString(0,((y_num++)*16),str,16); 
        #endif
        #if MODUIL_TRAFFIC_LIGHT
        static uint8_t mode = 4;
        const uint8_t ON[] = {"ON"};
        const uint8_t OFF[] = {"OFF"};
        data_num += 3;
        if(mode != Get_Traffic_Light_Mode())
        {
            updata_flag = true;
        }
        mode = Get_Traffic_Light_Mode();                                    //获取模式
        //刷新显示缓存
        switch (mode)
        {
            case 0:
                sprintf(str, "RED_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16); 
                sprintf(str, "GREEN_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16);
                sprintf(str, "YELLOW_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16);
            break;

            case 1:
                sprintf(str, "RED_%s",ON);
                OLED_ShowString(0,((y_num++)*16),str,16); 
                sprintf(str, "GREEN_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16);
                sprintf(str, "YELLOW_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16);
            break;

            case 2:
                sprintf(str, "RED_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16); 
                sprintf(str, "GREEN_%s",ON);
                OLED_ShowString(0,((y_num++)*16),str,16);
                sprintf(str, "YELLOW_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16);
            break;

            case 3:
                sprintf(str, "RED_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16); 
                sprintf(str, "GREEN_%s",OFF);
                OLED_ShowString(0,((y_num++)*16),str,16);
                sprintf(str, "YELLOW_%s",ON);
                OLED_ShowString(0,((y_num++)*16),str,16);
            break;
        }
        #endif
        #if MODUIL_PIR
        static uint8_t lightstate = 3;
        static uint8_t humanstatus = 3;
        data_num += 3;
        // sprintf(str, "welcome to here");
        // OLED_ShowString(0,((y_num++)*16),str,16);
        if(humanstatus != Get_Human_Status() || lightstate != Get_Light_State())       //判断数据是否发生改变
        {
            updata_flag = true;
        }
        humanstatus = Get_Human_Status();                   //获取人体感应状态
        // lightstate = Get_Light_State();                     //获取感光状态
        //刷新human显示
        if(humanstatus)
        {
            
            sprintf(str, "human detected");
        }
        else
        {
            sprintf(str, "human lost target");
        }
        OLED_ShowString(0,((y_num++)*16),str,16); 
        //刷新light显示
        // if(lightstate)
        // {
            
        //     sprintf(str, "night!!!");
        // }
        // else
        // {
        //     sprintf(str, "day!!!");
        // }
        // OLED_ShowString(0,((y_num++)*16),str,16);
        #endif 
        #if MODUIL_GAS
        static int humi = 0;
        static int temp = 0;
        static int Resistance = 0;
        data_num += 3;
        if(temp != Get_Aht20_Environment_Temp() || humi != Get_Aht20_Environment_Humidity() || Resistance != Get_Gas_Sensor_Resistance())       //判断数据是否发生改变
        {
            updata_flag = true;
        }
        temp = Get_Aht20_Environment_Temp();                    //获取环境温度
        humi = Get_Aht20_Environment_Humidity();                //获取环境湿度  
        Resistance = Get_Gas_Sensor_Resistance();               //获取Sensor电阻值
        //刷新显示缓存
        sprintf(str, "temp: %d", temp);
        OLED_ShowString(0,((y_num++)*16),str,16);
        sprintf(str, "humi: %d%s", humi,"%");
        OLED_ShowString(0,((y_num++)*16),str,16);
        sprintf(str, "gas: %dkom", Resistance);
        OLED_ShowString(0,((y_num++)*16),str,16);
        #endif 
        #if MODULE_FIRE
        static uint8_t firestate = 0;
        data_num += 2;
        if(firestate != Get_Fire_State())
        {
            updata_flag = true;
        }
        firestate = Get_Fire_State();                           //获取火焰状态
        //刷新显示缓存
        OLED_ShowString(0, ((y_num++)*16), "Hello, Harmony", 16);
        if(firestate)
        {
            sprintf(str, "firestate:fire");
        }
        else
        {
            sprintf(str, "firestate:normal");
        }
        OLED_ShowString(0,((y_num++)*16),str,16);
        #endif
        #ifdef MODULE_QR
            const char QRMap[] = 
"#######___#_#_#######\
#_____#___#_#_#_____#\
#_###_#_#_#_#_#_###_#\
#_###_#_##_##_#_###_#\
#_###_#_#___#_#_###_#\
#_____#_#_##__#_____#\
#######_#_#_#_#######\
________##___________\
#_#####____#__#####__\
#__###__#__########_#\
_####_##_#__###__###_\
#_#_#___##_###__###__\
_##___#_____###_____#\
________#_#_#____#___\
#######____#__#___##_\
#_____#_#____#_#_####\
#_###_#_#__#__##____#\
#_###_#_###_######___\
#_###_#_##__#__#__#__\
#_____#___#_##__###__\
#######_#_###_#_#__#_";
            for(int i=0; i<21; i++){
                for(int j=0; j<21; j++){
                    if(QRMap[i*21+j] == '#'){
                        OLED_DrawPoint(3*j, 3*i, 1);
                        OLED_DrawPoint(3*j+1, 3*i, 1);
                        OLED_DrawPoint(3*j+2, 3*i, 1);
                        OLED_DrawPoint(3*j, 3*i+1, 1);
                        OLED_DrawPoint(3*j+1, 3*i+1, 1);
                        OLED_DrawPoint(3*j+2, 3*i+1, 1);
                        OLED_DrawPoint(3*j, 3*i+2, 1);
                        OLED_DrawPoint(3*j+1, 3*i+2, 1);
                        OLED_DrawPoint(3*j+2, 3*i+2, 1);
                    }
                }
            }
            char payload[64];
            snprintf(payload, sizeof(payload), "id: %d", deviceId);
            OLED_ShowString(3*21+3, ((y_num++)*font_size), payload, font_size);
            snprintf(payload, sizeof(payload), "fid: %d", facilityId);
            OLED_ShowString(3*21+3, ((y_num++)*font_size), payload, font_size);
            snprintf(payload, sizeof(payload), "type: %d", facilityType);
            OLED_ShowString(3*21+3, ((y_num++)*font_size), payload, font_size);
            snprintf(payload, sizeof(payload), "det: %d", (int)Get_Human_Status());
            OLED_ShowString(3*21+3, ((y_num++)*font_size), payload, font_size);
            
            updata_flag = true;

        #endif
        
        
        if(updata_flag == true || startup_flag == true)
        {
            startup_flag = false;
            OLED_Refresh_Gram();                                            //更新显示
        }
        
    }
    
}

/*
* 函数名称 : OledDemo
* 功能描述 : oled显示示例配置
* 参    数 : 空
* 返回值   : 空
* 示    例 : OledDemo();
*/
/******************************************************************************/ 
static void OledDemo(void)
/******************************************************************************/ 
{
    osThreadAttr_t attr;

    attr.name       = "OledTask";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = OLED_TASK_STACK_SIZE;
    attr.priority   = OLED_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)OledTask, NULL, &attr) == NULL) {
        printf("[OledTask] Falied to create OledTask!\n");
    }
    printf("\r\n[OledTask] Succ to create OledTask!\n");
}

APP_FEATURE_INIT(OledDemo);
/******************************* End of File (C) ******************************/