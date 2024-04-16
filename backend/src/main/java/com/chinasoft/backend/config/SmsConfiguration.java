package com.chinasoft.backend.config;


import com.chinasoft.backend.common.SmsUtil;
import com.chinasoft.backend.config.alioss.AliOssProperties;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnMissingBean;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

/**
 * 短信发送服务配置
 *
 * @author 孟祥硕
 */
@Configuration
@Slf4j
public class SmsConfiguration {

    @Autowired
    AliOssProperties aliOssProperties;

    private static final String PRODUCT = "Dysmsapi";

    private static final String DOMAIN = "dysmsapi.aliyuncs.com";

    @Bean
    @ConditionalOnMissingBean(SmsUtil.class)
    public SmsUtil smsUtil() {
        return new SmsUtil(PRODUCT, DOMAIN,
                aliOssProperties.getAccessKeyId(), aliOssProperties.getAccessKeySecret());
    }
}
