#include "MQTTPacket.h"
#include "transport.h"
#include <stdint.h>

#define HOST_ADDR "120.46.211.213"
extern uint8_t Get_Human_Status(void);
extern int facilityId;
extern int facilityType;
extern int deviceId;

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
	// int payloadlen = strlen(payload);
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

	// /* subscribe 订阅主题 */
	// topicString.cstring = "subscribe";
	// len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos); // 打包橙订阅主题包保存在buf中
	// transport_sendPacketBuffer(mysock, buf, len);									 // 客户端发送订阅主题至服务器
	// if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK)					 /* 等待服务器返回订阅主题ACK响应包 */
	// {
	// 	unsigned short submsgid;
	// 	int subcount;
	// 	int granted_qos;

	// 	MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
	// 	if (granted_qos != 0)
	// 	{
	// 		printf("granted qos != 0, %d\n", granted_qos);
	// 		goto exit;
	// 	}
	// }
	// else
	// 	goto exit;

	/* loop getting msgs on subscribed topic */

	while (1)
	{
		/* transport_getdata() has a built-in 1 second timeout,
		your mileage will vary */
		// if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH)
		// {
		// 	unsigned char dup;
		// 	int qos;
		// 	unsigned char retained;
		// 	unsigned short msgid;
		// 	int payloadlen_in;
		// 	unsigned char *payload_in;
		// 	MQTTString receivedTopic;

		// 	// MQTTSerialize_publish  构造PUBLISH报文的函数，需要发送消息的依靠此函数构造报文
		// 	// MQTTDeserialize_publish  函数是解析PUBLISH报文的函数，接收消息就靠它
		// 	MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, buf, buflen);
		// 	printf("message arrived %d,%s\n", payloadlen_in, payload_in);
		// }

		topicString.cstring = "IoTDevice"; // 发布设置主题

		snprintf(payload, sizeof(payload), "{'deviceId': %d, 'facilityId': %d, 'facilityType': %d, 'detection': %d}", deviceId, facilityId, facilityType, (int)Get_Human_Status());

		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char *)payload, strlen(payload));
		transport_sendPacketBuffer(mysock, buf, len);

		usleep(1000*1000);
	}
exit:
	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	transport_sendPacketBuffer(mysock, buf, len); // 客户端发送发布主题包至服务器
	transport_close(mysock);

	return 0;
}