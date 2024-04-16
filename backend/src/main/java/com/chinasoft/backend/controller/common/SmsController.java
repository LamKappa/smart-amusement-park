package com.chinasoft.backend.controller.common;

import com.aliyuncs.dysmsapi.model.v20170525.SendSmsResponse;
import com.aliyuncs.exceptions.ClientException;
import com.aliyuncs.utils.StringUtils;
import com.chinasoft.backend.common.*;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.service.RedisService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;

/**
 * 短信服务接口
 *
 * @author 孟祥硕
 */
@RestController
@RequestMapping("/sms")
public class SmsController {


    /***
     * 注入redis模版
     */
    @Autowired
    private RedisService redisService;

    @Autowired
    private SmsUtil smsUtil;


    /**
     * 发送短信
     *
     * @ResponseBody 返回json数据
     * @RequestMapping 拦截请求，指定请求类型：POST
     * @RequestBody 接受前台传入的json数据 接受类型为Map
     */
    @ResponseBody
    @PostMapping("/sendSms")
    public BaseResponse<Boolean> smsXxs(@RequestBody Map<String, Object> requestMap, HttpServletRequest request) throws ClientException, ClientException {
        Map<String, Object> map = new HashMap<>();
        String phone = requestMap.get("phone").toString();

        // 校验手机号
        if (!Pattern.matches("^1[3-9]\\d{9}$", phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户手机号格式错误");
        }

        // 调用工具栏中生成验证码方法（指定长度的随机数）
        String code = CodeUtil.generateVerifyCode(6);
        // 填充验证码
        SendSmsResponse response = smsUtil.sendSms(phone, code);// 传入手机号码及短信模板中的验证码占位符

        if (!response.getCode().equals("OK")) {
            throw new BusinessException(ErrorCode.SYSTEM_ERROR, "验证码发送失败");
        }

        map.put("code", code);
        map.put("phone", phone);
        request.getSession().setAttribute("CodePhone", map);

        // 验证码绑定手机号并存储到redis
        redisService.set(phone, code);
        redisService.expire(phone, 620);// 调用reids工具类中存储方法设置超时时间

        return ResultUtils.success(true);
    }


    /**
     * 验证码验证
     *
     * @throws ClientException 抛出异常
     * @ResponseBody 返回json数据
     * @RequestMapping 拦截请求，指定请求类型：POST
     * @RequestBody 接受前台传入的json数据 接受类型为Map
     */
    @ResponseBody
    @PostMapping("/validCode")
    public BaseResponse validateNum(@RequestBody Map<String, Object> requestMap) throws ClientException {

        Map<String, Object> map = new HashMap<>();
        String phone = requestMap.get("phone").toString();// 获取注册手机号码
        String verifyCode = requestMap.get("code").toString();// 获取手机验证码

        // 首先比对验证码是否失效
        String redisauthcode = redisService.get(phone); // 传入phone返回redis中的value

        if (StringUtils.isEmpty(redisauthcode)) {
            // 如果未取到则过期验证码已失效
            map.put("ruselt", 404);
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "验证码已失效");
        } else if (!"".equals(redisauthcode) && !verifyCode.equals(redisauthcode)) {
            // 验证码错误
            map.put("ruselt", 500);
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "验证码错误");
        } else {
            // 用户注册成功
            redisService.remove(phone);
            map.put("ruselt", 200);
            // 调用用户注册接口来存入数据
        }
        return ResultUtils.success(map);
    }
}