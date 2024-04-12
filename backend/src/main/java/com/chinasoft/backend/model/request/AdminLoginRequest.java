package com.chinasoft.backend.model.request;

import lombok.Data;

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