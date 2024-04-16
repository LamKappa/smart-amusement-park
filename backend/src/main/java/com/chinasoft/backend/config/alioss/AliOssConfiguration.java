package com.chinasoft.backend.config.alioss;

import com.chinasoft.backend.common.AliOssUtil;
import lombok.extern.slf4j.Slf4j;
import org.springframework.boot.autoconfigure.condition.ConditionalOnMissingBean;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

/**
 * 创建AliOssUtil的配置
 *
 * @author 孟祥硕
 */
@Configuration
@Slf4j
public class AliOssConfiguration {

    @Bean
    @ConditionalOnMissingBean(AliOssUtil.class)
    public AliOssUtil aliOssUtil(AliOssProperties aliOssProperties) {
        AliOssUtil aliOssUtil = new AliOssUtil(aliOssProperties.getEndpoint(),
                aliOssProperties.getAccessKeyId(),
                aliOssProperties.getAccessKeySecret(),
                aliOssProperties.getBucketName());
        return aliOssUtil;
    }
}
