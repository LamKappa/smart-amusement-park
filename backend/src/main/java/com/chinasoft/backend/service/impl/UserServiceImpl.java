package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.model.entity.User;
import com.chinasoft.backend.service.UserService;
import com.chinasoft.backend.mapper.UserMapper;
import org.springframework.stereotype.Service;

/**
* @author 86178
* @description 针对表【user】的数据库操作Service实现
* @createDate 2024-04-03 14:44:10
*/
@Service
public class UserServiceImpl extends ServiceImpl<UserMapper, User>
    implements UserService{

}




