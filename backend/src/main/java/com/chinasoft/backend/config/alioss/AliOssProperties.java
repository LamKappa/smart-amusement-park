package com.chinasoft.backend.config.alioss;

import lombok.Data;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

/**
 * 阿里服务配置信息
 *
 * @author 孟祥硕
 */
@Component
@ConfigurationProperties(prefix = "alioss")
@Data
public class AliOssProperties {

    private String endpoint;
    private String accessKeyId;
    private String accessKeySecret;
    private String bucketName;

}