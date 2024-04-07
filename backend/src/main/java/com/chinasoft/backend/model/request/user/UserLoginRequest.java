package com.chinasoft.backend.model.request.user;

import lombok.Data;

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
