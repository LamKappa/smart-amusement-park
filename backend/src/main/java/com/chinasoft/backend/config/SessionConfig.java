package com.chinasoft.backend.config;

import org.springframework.boot.autoconfigure.session.DefaultCookieSerializerCustomizer;
import org.springframework.session.MapSessionRepository;
import org.springframework.session.SessionRepository;
import org.springframework.session.web.http.DefaultCookieSerializer;

import java.util.concurrent.ConcurrentHashMap;

/**
 * Session配置
 *
 * @author 孟祥硕
 */
// @Configuration
// @EnableSpringHttpSession
public class SessionConfig {
    // @Bean
    public SessionRepository sessionRepository() {
        return new MapSessionRepository(new ConcurrentHashMap<>());
    }

    // @Bean
    DefaultCookieSerializerCustomizer cookieSerializerCustomizer() {
        return new DefaultCookieSerializerCustomizer() {
            @Override
            public void customize(DefaultCookieSerializer cookieSerializer) {
                cookieSerializer.setSameSite("None");
                cookieSerializer.setUseSecureCookie(true); // 此项必须，否则set-cookie会被chrome浏览器阻拦
            }
        };
    }
}