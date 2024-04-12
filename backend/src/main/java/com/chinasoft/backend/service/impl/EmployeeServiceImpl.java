package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.EmployeeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.EmployeeMapper;
import com.chinasoft.backend.model.entity.Employee;
import com.chinasoft.backend.service.EmployeeService;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.util.DigestUtils;

import javax.servlet.http.HttpServletRequest;
import java.util.regex.Pattern;

/**
 * @author 86178
 * @description 针对表【employee】的数据库操作Service实现
 * @createDate 2024-04-12 17:21:37
 */
@Service
@Slf4j
public class EmployeeServiceImpl extends ServiceImpl<EmployeeMapper, Employee>
        implements EmployeeService {

    private static final String SALT = "chinasoft";

    @Autowired
    private EmployeeMapper employeeMapper;

    @Override
    public Employee adminLogin(String phone, String password, HttpServletRequest request) {
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
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("phone", phone);
        queryWrapper.eq("password", encryptPassword);
        Employee admin = employeeMapper.selectOne(queryWrapper);
        // 用户不存在
        if (admin == null) {
            log.info("admin login failed, adminAccount cannot match adminPassword");
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户不存在或密码错误");
        }
        // 脱敏
        admin.setPassword(null);
        // 3. 记录用户的登录态
        request.getSession().setAttribute(EmployeeConstant.USER_LOGIN_STATE, admin);
        return admin;

    }

    /**
     * 获取当前登录用户
     */
    @Override
    public Employee getLoginEmployee(HttpServletRequest request) {
        // 先判断是否已登录
        Object employeeObj = request.getSession().getAttribute(EmployeeConstant.USER_LOGIN_STATE);
        Employee currentEmployee = (Employee) employeeObj;
        if (currentEmployee == null || currentEmployee.getId() == null) {
            throw new BusinessException(ErrorCode.NOT_LOGIN_ERROR);
        }
        // 从数据库查询
        long employeeId = currentEmployee.getId();
        currentEmployee = this.getById(employeeId);
        if (currentEmployee == null) {
            throw new BusinessException(ErrorCode.NOT_LOGIN_ERROR);
        }
        return currentEmployee;
    }
    

}




