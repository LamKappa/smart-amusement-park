package com.chinasoft.backend.config;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

/**
 * 高德地图配置属性
 *
 * @author 孟祥硕
 */
@Component
@ConfigurationProperties(prefix = "amap")
@Data
public class AmapProperties {
    String key;
}
