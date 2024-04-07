package com.chinasoft.backend.service.impl;

import cn.hutool.core.util.RandomUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.UserMapper;
import com.chinasoft.backend.model.entity.User;
import com.chinasoft.backend.service.UserService;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.util.DigestUtils;

import java.util.regex.Pattern;

/**
 * @author mxs
 */
@Service
public class UserServiceImpl extends ServiceImpl<UserMapper, User>
        implements UserService {

    private static final String DEFAULT_AVATAR_URL = "https://leimo-picgo.oss-cn-chengdu.aliyuncs.com/picgoimg/leimo.png";

    private static final String SALT = "chinasoft";

    @Autowired
    private UserMapper userMapper;

    /**
     * 用户注册
     */
    @Override
    public Long userRegister(String phone, String userPassword, String checkPassword, String avatarUrl, String username) {
        // 设置为默认头像和用户名
        if (StringUtils.isBlank(avatarUrl)) {
            avatarUrl = DEFAULT_AVATAR_URL;
        }
        if (StringUtils.isBlank(username)) {
            username = "user" + RandomUtil.randomNumbers(5);
        }

        // 校验
        if (StringUtils.isAnyBlank(phone, userPassword, checkPassword)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }
        if (!Pattern.matches("^1[3-9]\\d{9}$", phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户手机号格式错误");
        }
        if (userPassword.length() < 8 || checkPassword.length() < 8) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户密码过短");
        }
        if (userPassword.length() > 16 || checkPassword.length() > 16) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户密码过长");
        }
        // 密码和校验密码相同
        if (!userPassword.equals(checkPassword)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "两次输入的密码不一致");
        }

        synchronized (phone.intern()) {
            // 账户不能重复
            QueryWrapper<User> queryWrapper = new QueryWrapper<>();
            queryWrapper.eq("phone", phone);
            long count = userMapper.selectCount(queryWrapper);
            if (count > 0) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号重复");
            }
            // 2. 加密
            String encryptPassword = DigestUtils.md5DigestAsHex((SALT + userPassword).getBytes());
            // 3. 插入数据
            User user = new User();
            user.setPhone(phone);
            user.setPassword(encryptPassword);
            user.setUsername(username);
            user.setAvatarUrl(avatarUrl);

            boolean saveResult = this.save(user);
            if (!saveResult) {
                throw new BusinessException(ErrorCode.SYSTEM_ERROR, "注册失败，数据库错误");
            }
            return user.getId();
        }

    }
}




