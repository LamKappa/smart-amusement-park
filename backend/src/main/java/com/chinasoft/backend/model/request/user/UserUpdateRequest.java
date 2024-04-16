package com.chinasoft.backend.model.request.user;

import lombok.Data;

/**
 * 用户更新请求
 *
 * @author 孟祥硕
 */
@Data
public class UserUpdateRequest {

    private Long id;

    /**
     * 手机号
     */
    private String phone;

    /**
     * 用户名
     */
    private String username;

    /**
     * 密码
     */
    private String password;


    /**
     * 用户头像
     */
    private String avatarUrl;
}
