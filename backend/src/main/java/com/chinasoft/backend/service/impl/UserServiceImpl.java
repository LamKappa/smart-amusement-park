package com.chinasoft.backend.service.impl;

import cn.hutool.core.util.RandomUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.UserConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.UserMapper;
import com.chinasoft.backend.model.entity.User;
import com.chinasoft.backend.service.UserService;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.util.DigestUtils;

import javax.servlet.http.HttpServletRequest;
import java.util.regex.Pattern;

/**
 * @author mxs
 */
@Service
@Slf4j
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


    /**
     * 用户登录
     */
    @Override
    public User userLogin(String phone, String password, HttpServletRequest request) {
        // 1. 校验
        if (StringUtils.isAnyBlank(phone, password)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }
        if (!Pattern.matches("^1[3-9]\\d{9}$", phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户手机号格式错误");
        }
        if (password.length() < 8 || password.length() > 16) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户密码过短或过长");
        }
        // 2. 加密
        String encryptPassword = DigestUtils.md5DigestAsHex((SALT + password).getBytes());
        // 查询用户是否存在
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("phone", phone);
        queryWrapper.eq("password", encryptPassword);
        User user = userMapper.selectOne(queryWrapper);
        // 脱敏
        user.setPassword(null);
        // 用户不存在
        if (user == null) {
            log.info("user login failed, userAccount cannot match userPassword");
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户不存在或密码错误");
        }
        // 3. 记录用户的登录态
        request.getSession().setAttribute(UserConstant.USER_LOGIN_STATE, user);
        return user;
    }

    /**
     * 获取当前登录用户
     */
    @Override
    public User getLoginUser(HttpServletRequest request) {
        // 先判断是否已登录
        Object userObj = request.getSession().getAttribute(UserConstant.USER_LOGIN_STATE);
        User currentUser = (User) userObj;
        if (currentUser == null || currentUser.getId() == null) {
            throw new BusinessException(ErrorCode.NOT_LOGIN_ERROR);
        }
        // 从数据库查询
        long userId = currentUser.getId();
        currentUser = this.getById(userId);
        if (currentUser == null) {
            throw new BusinessException(ErrorCode.NOT_LOGIN_ERROR);
        }
        return currentUser;
    }

    /**
     * 用户注销
     */
    @Override
    public Boolean userLogout(HttpServletRequest request) {
        if (request.getSession().getAttribute(UserConstant.USER_LOGIN_STATE) == null) {
            throw new BusinessException(ErrorCode.OPERATION_ERROR, "未登录");
        }
        // 移除登录态
        request.getSession().removeAttribute(UserConstant.USER_LOGIN_STATE);
        return true;
    }
}




