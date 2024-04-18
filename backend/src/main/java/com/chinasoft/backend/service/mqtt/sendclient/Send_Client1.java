package com.chinasoft.backend.service.mqtt.sendclient;

import com.chinasoft.backend.config.mqtt.MqttProperties;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.UUID;

/**
 * MQTT 发送者
 * @author 姜堂蕴之
 */
@Component
public class Send_Client1 {

    private static final Logger logger = LoggerFactory.getLogger(Send_Client1.class);

    @Autowired
    private SendClient1_CallBack mqttSendCallBack;

    @Autowired
    private MqttProperties mqttProperties;

    public MqttClient connect() {
        MqttClient client = null;
        try {
            // 定义一个uuid
            String uuid = UUID.randomUUID().toString().replaceAll("-", "");
            // 创建一个id不能重复的客户端
            client = new MqttClient(mqttProperties.getHostUrl(), uuid, new MemoryPersistence());
            // 设置基本的参数
            MqttConnectOptions options = new MqttConnectOptions();
//            options.setUserName(mqttProperties.getUsername());
//            options.setPassword(mqttProperties.getPassword().toCharArray());
            options.setConnectionTimeout(mqttProperties.getTimeout());
            options.setKeepAliveInterval(mqttProperties.getKeepAlive());
            options.setCleanSession(true);
            options.setAutomaticReconnect(false);
            try {
                // 设置回调方法
                client.setCallback(mqttSendCallBack);
                client.connect(options);
            } catch (Exception e) {
                e.printStackTrace();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return client;
    }

    /**
     * 发布消息
     * 主题格式： server:report:$orgCode(参数实际使用机构代码)
     *
     * @param retained    是否保留
     * @param orgCode     orgId
     * @param pushMessage 消息体
     */
    public void publish(boolean retained, String orgCode, String pushMessage) {
        MqttMessage message = new MqttMessage();
        message.setQos(mqttProperties.getQos());
        message.setRetained(retained);
        message.setPayload(pushMessage.getBytes());
        MqttDeliveryToken token;
        MqttClient mqttClient = connect();
        try {
//            mqttClient.publish("server:report:" + orgCode, message);
            mqttClient.publish(orgCode, message);
        } catch (MqttException e) {
            e.printStackTrace();
        } finally {
            disconnect(mqttClient);
            close(mqttClient);
        }
    }

    /**
     * 关闭连接
     *
     * @param mqttClient
     */
    public static void disconnect(MqttClient mqttClient) {
        try {
            if (mqttClient != null) mqttClient.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    /**
     * 释放资源
     *
     * @param mqttClient
     */
    public static void close(MqttClient mqttClient) {
        try {
            if (mqttClient != null) mqttClient.close();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}

