package com.chinasoft.backend.config.mqtt;

import com.chinasoft.backend.mqtt.receiveclient.MqttAcceptClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Conditional;
import org.springframework.context.annotation.Configuration;

/**
 * Mqtt配置
 *
 * @author 孟祥硕
 */
@Configuration
public class MqttConfig {

    @Autowired
    private MqttAcceptClient mqttAcceptClient;


    /**
     * 订阅mqtt
     *
     * @return
     */
    @Conditional(MqttCondition.class)
    @Bean
    public MqttAcceptClient getMqttPushClient() {
        mqttAcceptClient.connect();
        return mqttAcceptClient;
    }
}