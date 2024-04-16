package com.chinasoft.backend.model.request.employee;

import lombok.Data;

/**
 * 管理员登录请求
 *
 * @author 孟祥硕
 */
@Data
public class AdminLoginRequest {
    private static final long serialVersionUID = 3191241716373120793L;

    /**
     * 账号（此处为工号）
     */
    private String username;

    /**
     * 密码
     */
    private String password;


}