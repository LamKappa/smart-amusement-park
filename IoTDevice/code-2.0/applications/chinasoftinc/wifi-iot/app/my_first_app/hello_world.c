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
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
// #include "wifi_sta.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"

// #include "lwip/sockets.h"
#include "ohos_init.h"
#include "ohos_types.h"
#include "MQTTPacket.h"
#include "transport.h"

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"
#include "lwip/netifapi.h"
#include "lwip/api_shell.h"

#define HOST_ADDR "120.46.211.213"

int mqtt_connect(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int mysock = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	char payload[200] = "{'msg': 'test'}"; // 发送数据
	int payloadlen = strlen(payload);
	int len = 0;
	char *host = HOST_ADDR; // MQTT服务器的IP地址
	int port = 1883;

	// 打开一个接口，并且和服务器 建立连接，创建socket连接mqtt服务器函数
	mysock = transport_open(host, port);
	printf("mysock:%d\r\n", mysock);
	if (mysock < 0)
	{
		return mysock;
	}

	printf("Sending to hostname %s port %d\n", host, port);

	data.clientID.cstring = "test_hi3861"; // 修改成自己名称，
	data.keepAliveInterval = 20;	// 心跳时间
	data.cleansession = 1;
	data.username.cstring = "test_hi3861";
	data.password.cstring = "114514";
	// 数据封装成数据报文
	len = MQTTSerialize_connect(buf, buflen, &data);
	// 以TCP方式发送数据, <0发送数据失败
	transport_sendPacketBuffer(mysock, buf, len);

	/* wait for connack读取服务器返回的报文是否是connack */
	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
		{
			printf("Unable to connect, return code %d\n", connack_rc);
			goto exit;
		}
	}
	else
		goto exit;

	/* subscribe 订阅主题 */
	topicString.cstring = "subscribe";
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos); // 打包橙订阅主题包保存在buf中
	transport_sendPacketBuffer(mysock, buf, len);									 // 客户端发送订阅主题至服务器
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK)					 /* 等待服务器返回订阅主题ACK响应包 */
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0)
		{
			printf("granted qos != 0, %d\n", granted_qos);
			goto exit;
		}
	}
	else
		goto exit;

	/* loop getting msgs on subscribed topic */

	while (1)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		{
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			int payloadlen_in;
			unsigned char *payload_in;
			MQTTString receivedTopic;

			// MQTTSerialize_publish  构造PUBLISH报文的函数，需要发送消息的依靠此函数构造报文
			// MQTTDeserialize_publish  函数是解析PUBLISH报文的函数，接收消息就靠它
			MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, buf, buflen);
			printf("message arrived %d,%s\n", payloadlen_in, payload_in);
		}
		topicString.cstring = "publish"; // 发布设置主题

		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);
		transport_sendPacketBuffer(mysock, buf, len);

		usleep(100000);
	}
exit:
	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	transport_sendPacketBuffer(mysock, buf, len); // 客户端发送发布主题包至服务器
	transport_close(mysock);

	return 0;
}

void connect_wifi(const char * wifi_ssid, const char * wifi_password)
{

    printf("in connect_wifi 1\r\n");
    WifiErrorCode errCode;
    WifiDeviceConfig apConfig = {};
    int netId = -1;
    printf("in connect_wifi 2\r\n");

    // setup your AP params
    strcpy(apConfig.ssid, wifi_ssid);     //
    strcpy(apConfig.preSharedKey, wifi_password); //密码
    apConfig.securityType = WIFI_SEC_TYPE_PSK;   //加密方式
    printf("in connect_wifi 3\r\n");

    errCode = EnableWifi();
    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("in connect_wifi 4\r\n");

    errCode = ConnectTo(netId);
    printf("ConnectTo(%d): %d\r\n", netId, errCode);
    usleep(1000 * 1000);
    // 联网业务开始
    // 这里是网络业务代码...
    struct netif *iface = netifapi_netif_find("wlan0");
    if (iface)
    {
        err_t ret = netifapi_dhcp_start(iface); //返回IP地址
        printf("netifapi_dhcp_start: %d\r\n", ret);

        // usleep(2000 * 1000);
        sleep(5);
        ; // wait DHCP server give me IP
        ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
        printf("netifapi_netif_common: %d\r\n", ret);
    }
}

#define SSID "xc"
#define PASSWORD "xuchang408"

static void Task(void *arg){
    (void)arg;
    printf("[DEMO] Hello World!!! ------ xc\n");
    connect_wifi(SSID, PASSWORD); // 连接WIFI热点
    printf("[DEMO] WIFI CONNECTED!!! ------ xc\n");

    printf("[DEMO] CONNECTTING MQTT!!! ------ xc\n");
    mqtt_connect(); // 运行mqtt测试程序
}

void HelloWorld(void)
{   
    osThreadAttr_t attr;

    attr.name = "FirstApp";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = 36;

    // 在新线程中执行函数mqtt_test_thread
    if (osThreadNew((osThreadFunc_t)Task, NULL, &attr) == NULL)
    {
        printf("[FirstApp] Falied to create FirstApp!\n");
    }

    AT_RESPONSE_OK;
    // return HI_ERR_SUCCESS;
}

APP_FEATURE_INIT(HelloWorld); // Register the function HelloWorld to the system