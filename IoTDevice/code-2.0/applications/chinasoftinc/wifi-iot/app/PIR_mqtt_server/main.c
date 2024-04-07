#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"
#include "lwip/netifapi.h"
#include "lwip/api_shell.h"
#include <at.h>
#include <hi_at.h>
#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"

void connect_wifi(const char * wifi_ssid, const char * wifi_password);
int mqtt_connect(void);

#define SSID "xc"
#define PASSWORD "xuchang408"

int facilityId;
int facilityType;
int deviceId;

static void Task(void *arg){
    (void)arg;

    facilityId = 2;
    facilityType = 0;
    deviceId = 2;
    printf("[RIP_mqtt_server] facility: %d\r\n", facilityId);
    printf("[RIP_mqtt_server] tag: %d\r\n", facilityType);
    printf("[RIP_mqtt_server] device: %d\r\n", deviceId);

    printf("[RIP_mqtt_server] CONNECTTING WIFI!!!\r\n");
    connect_wifi(SSID, PASSWORD); // 连接WIFI热点

    printf("[RIP_mqtt_server] CONNECTTING MQTT!!!\r\n");
    mqtt_connect(); // 运行mqtt测试程序
}

void RIP_mqtt_server(void)
{   
    osThreadAttr_t attr;

    attr.name = "RIP_mqtt_server";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = 36;

    // 在新线程中执行函数mqtt_test_thread
    if (osThreadNew((osThreadFunc_t)Task, NULL, &attr) == NULL)
    {
        printf("[RIP_mqtt_server] Falied to create RIP_mqtt_server!\n");
    }

    AT_RESPONSE_OK;
    // return HI_ERR_SUCCESS;
}

APP_FEATURE_INIT(RIP_mqtt_server); // Register the function HelloWorld to the system