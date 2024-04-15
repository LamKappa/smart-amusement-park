package com.chinasoft.backend.config;

import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.data.redis.connection.RedisConnectionFactory;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.core.ValueOperations;
import org.springframework.data.redis.serializer.GenericJackson2JsonRedisSerializer;
import org.springframework.data.redis.serializer.RedisSerializer;
import org.springframework.data.redis.serializer.StringRedisSerializer;

@Configuration
@Slf4j
public class RedisConfiguration {

    @Bean
    public RedisTemplate redisTemplate(RedisConnectionFactory redisConnectionFactory) {
        RedisTemplate redisTemplate = new RedisTemplate();
        // 设置redis连接工厂对象
        redisTemplate.setConnectionFactory(redisConnectionFactory);
        // 设置redis的序列化器
        RedisSerializer stringSerializer = new StringRedisSerializer();// 序列化为String
        GenericJackson2JsonRedisSerializer jsonSerializer = new GenericJackson2JsonRedisSerializer();
        redisTemplate.setKeySerializer(stringSerializer);
        // redisTemplate.setValueSerializer(jsonSerializer);
        // redisTemplate.setHashKeySerializer(stringSerializer);
        // redisTemplate.setHashValueSerializer(serializer);
        return redisTemplate;
    }
}