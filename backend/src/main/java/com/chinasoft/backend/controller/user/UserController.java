package com.chinasoft.backend.controller.user;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.User;
import com.chinasoft.backend.model.request.user.UserLoginRequest;
import com.chinasoft.backend.model.request.user.UserRegisterRequest;
import com.chinasoft.backend.service.UserService;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.servlet.http.HttpServletRequest;

@RestController
@RequestMapping("/user")
public class UserController {
    @Autowired
    UserService userService;

    private static final String DEFAULT_AVATAR_URL = "https://leimo-picgo.oss-cn-chengdu.aliyuncs.com/picgoimg/leimo.png";

    /**
     * 用户注册
     *
     * @param userRegisterRequest
     * @return 用户id
     */
    @PostMapping("/register")
    public BaseResponse userRegister(@RequestBody UserRegisterRequest userRegisterRequest) {
        if (userRegisterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        String phone = userRegisterRequest.getPhone();
        String userPassword = userRegisterRequest.getPassword();
        String checkPassword = userRegisterRequest.getCheckPassword();
        String avatarUrl = userRegisterRequest.getAvatarUrl();
        String username = userRegisterRequest.getUsername();

        // 校验是否为空
        if (StringUtils.isAnyBlank(phone, userPassword, checkPassword)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }


        Long result = userService.userRegister(phone, userPassword, checkPassword, avatarUrl, username);
        return ResultUtils.success(result);
    }

    /**
     * 用户登录
     */
    @PostMapping("/login")
    public BaseResponse userLogin(@RequestBody UserLoginRequest userLoginRequest, HttpServletRequest request) {
        if (userLoginRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        String phone = userLoginRequest.getPhone();
        String password = userLoginRequest.getPassword();
        if (StringUtils.isAnyBlank(phone, password)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }
        User user = userService.userLogin(phone, password, request);
        return ResultUtils.success(user);
    }

    /**
     * 获取当前登录用户
     */
    @GetMapping("/getLoginUser")
    public BaseResponse<User> getLoginUser(HttpServletRequest request) {
        User user = userService.getLoginUser(request);
        return ResultUtils.success(user);
    }

    /**
     * 用户注销
     */
    @GetMapping("/logout")
    public BaseResponse<Boolean> userLogout(HttpServletRequest request) {
        if (request == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        Boolean result = userService.userLogout(request);
        return ResultUtils.success(result);
    }
}
