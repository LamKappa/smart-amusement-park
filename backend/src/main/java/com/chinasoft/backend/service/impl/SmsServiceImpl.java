package com.chinasoft.backend.service.impl;

import com.aliyuncs.utils.StringUtils;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.SmsUtil;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.service.RedisService;
import com.chinasoft.backend.service.SmsService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

/**
 * 验证码
 *
 * @author 孟祥硕
 */
@Service
public class SmsServiceImpl implements SmsService {


    /***
     * 注入redis模版
     */
    @Autowired
    private RedisService redisService;

    @Autowired
    private SmsUtil smsUtil;

    @Override
    public Boolean validCode(String phone, String verifyCode) {
        // 首先比对验证码是否失效
        String redisauthcode = redisService.get(phone); // 传入phone返回redis中的value

        if (StringUtils.isEmpty(redisauthcode)) {
            // 如果未取到则过期验证码已失效
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "验证码已失效");
        } else if (!"".equals(redisauthcode) && !verifyCode.equals(redisauthcode)) {
            // 验证码错误
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "验证码错误");
        } else {
            // 验证成功
            redisService.remove(phone);
            return true;
        }
    }
}
