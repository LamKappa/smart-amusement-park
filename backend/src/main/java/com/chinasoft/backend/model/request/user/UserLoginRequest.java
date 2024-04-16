package com.chinasoft.backend.model.request.user;

import lombok.Data;

/**
 * 用户登录请求
 *
 * @author 孟祥硕
 */
@Data
public class UserLoginRequest {
    private static final long serialVersionUID = 3191241716373120793L;

    /**
     * 手机号
     */
    private String phone;

    /**
     * 密码
     */
    private String password;


}
