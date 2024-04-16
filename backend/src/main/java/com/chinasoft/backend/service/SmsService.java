package com.chinasoft.backend.service;


/**
 * 短信服务Service
 *
 * @author 孟祥硕
 */
public interface SmsService {

    Boolean validCode(String phone, String verifyCode);
}
