package com.chinasoft.backend.service;

public interface SmsService {

    Boolean validCode(String phone, String verifyCode);
}
