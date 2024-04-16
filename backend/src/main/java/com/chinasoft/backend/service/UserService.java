package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.User;
import com.chinasoft.backend.model.request.user.UserUpdateRequest;

import javax.servlet.http.HttpServletRequest;

/**
 * @author 86178
 * @description 针对表【user】的数据库操作Service
 * @createDate 2024-04-03 14:44:10
 */
public interface UserService extends IService<User> {

    /**
     * 用户注册
     */
    Long userRegister(String phone, String userPassword, String checkPassword, String avatarUrl, String username, String verifyCode);

    /**
     * 用户登录
     */
    User userLogin(String phone, String password, HttpServletRequest request);

    /**
     * 获取当前登录用户
     */
    User getLoginUser(HttpServletRequest request);

    /**
     * 用户注销
     */
    Boolean userLogout(HttpServletRequest request);

    /**
     * 用户信息修改
     */
    User userUpdate(UserUpdateRequest userUpdateRequest, HttpServletRequest request);

    /**
     * 通过手机注册（验证码校验）
     */
    Long registerByPhone(String phone, String verifyCode);

    /**
     * 通过手机注册（验证码校验）
     */
    User loginByPhone(String phone, String verifyCode, HttpServletRequest request);
}
