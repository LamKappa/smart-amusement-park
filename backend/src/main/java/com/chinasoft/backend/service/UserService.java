package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.User;

/**
 * @author 86178
 * @description 针对表【user】的数据库操作Service
 * @createDate 2024-04-03 14:44:10
 */
public interface UserService extends IService<User> {

    Long userRegister(String phone, String userPassword, String checkPassword, String avatarUrl, String username);
}
