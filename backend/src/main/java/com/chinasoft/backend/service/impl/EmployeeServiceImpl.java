package com.chinasoft.backend.service.impl;

import cn.hutool.core.bean.BeanUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.EmployeeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.EmployeeMapper;
import com.chinasoft.backend.model.entity.Employee;
import com.chinasoft.backend.model.request.employee.AddEmployeeRequest;
import com.chinasoft.backend.model.request.employee.GetEmployeeRequest;
import com.chinasoft.backend.model.request.employee.UpdateEmployeeRequest;
import com.chinasoft.backend.service.EmployeeService;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.lang3.ObjectUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.util.DigestUtils;

import javax.servlet.http.HttpServletRequest;
import java.util.List;
import java.util.regex.Pattern;

/**
 * @author 孟祥硕
 * @description 针对表【employee】的数据库操作Service实现
 * @createDate 2024-04-12 17:21:37
 */
@Service
@Slf4j
public class EmployeeServiceImpl extends ServiceImpl<EmployeeMapper, Employee>
        implements EmployeeService {

    private static final String SALT = "chinasoft";

    private static final Integer EMPLOYEE = 0;

    private static final String DEFAULT_AVATAR_URL = "https://smart-amusement-park.oss-cn-chengdu.aliyuncs.com/lamkappa.bmp";

    private static final int MIN_NAME_LENGTH = 0;

    private static final int MAX_NAME_LENGTH = 20;

    @Autowired
    private EmployeeMapper employeeMapper;

    @Override
    public Employee adminLogin(String username, String password, HttpServletRequest request) {
        // 加密
        String encryptPassword = DigestUtils.md5DigestAsHex((SALT + password).getBytes());

        // 查询用户是否存在
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("username", username);
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

    @Override
    public List<Employee> getEmployee(GetEmployeeRequest getEmployeeRequest) {
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();

        if (getEmployeeRequest.getId() != null) {
            queryWrapper.eq("id", getEmployeeRequest.getId());
        }

        if (getEmployeeRequest.getName() != null && !getEmployeeRequest.getName().isEmpty()) {
            queryWrapper.like("name", getEmployeeRequest.getName());
        }

        if (getEmployeeRequest.getGender() != null && !getEmployeeRequest.getGender().isEmpty()) {
            queryWrapper.like("gender", getEmployeeRequest.getGender());
        }

        List<Employee> employeeList = this.baseMapper.selectList(queryWrapper);

        for (Employee employee : employeeList) {
            employee.setPassword(null);
        }

        return employeeList;
    }

    @Override
    public Employee addEmployee(AddEmployeeRequest addEmployeeRequest) {
        String name = addEmployeeRequest.getName();
        String phone = addEmployeeRequest.getPhone();
        Integer age = addEmployeeRequest.getAge();
        String gender = addEmployeeRequest.getGender();

        // 校验
        if (ObjectUtils.anyNull(name, phone, age, gender)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        if (name.length() > MAX_NAME_LENGTH || name.length() < MIN_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "姓名长度不在允许范围内");
        }

        if (!Pattern.matches("^1[3-9]\\d{9}$", phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号格式错误");
        }

        if (age < 18 || age > 65) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "年龄输入错误");
        }

        if (!gender.equals("男") && !gender.equals("女")) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "性别输入错误");
        }


        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("phone", phone);
        if (this.baseMapper.selectOne(queryWrapper) != null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号重复");
        }


        Employee employee = new Employee();
        BeanUtil.copyProperties(addEmployeeRequest, employee);
        if (employee.getAvatarUrl() == null) {
            employee.setAvatarUrl(DEFAULT_AVATAR_URL);
        }
        employee.setRole(EMPLOYEE);

        this.baseMapper.insert(employee);

        return this.baseMapper.selectById(employee.getId());
    }

    @Override
    public Employee updateEmployee(UpdateEmployeeRequest updateEmployeeRequest) {
        Long id = updateEmployeeRequest.getId();
        String name = updateEmployeeRequest.getName();
        String phone = updateEmployeeRequest.getPhone();
        Integer age = updateEmployeeRequest.getAge();
        String gender = updateEmployeeRequest.getGender();

        // 校验
        if (ObjectUtils.anyNull(id, name, phone, age, gender)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        Employee originalEmployee = this.baseMapper.selectById(id);
        if (originalEmployee == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "待更新的人员不存在");
        }

        if (name.length() > MAX_NAME_LENGTH || name.length() < MIN_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "姓名长度不在允许范围内");
        }

        if (!Pattern.matches("^1[3-9]\\d{9}$", phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号格式错误");
        }

        if (age < 18 || age > 65) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "年龄输入错误");
        }

        if (!gender.equals("男") && !gender.equals("女")) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "性别输入错误");
        }


        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("phone", phone);
        if (this.baseMapper.selectOne(queryWrapper) != null && !(phone.equals(originalEmployee.getPhone()))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号重复");
        }

        Employee updateEmployee = new Employee();
        updateEmployee.setId(id);
        updateEmployee.setName(name);
        updateEmployee.setPhone(phone);
        updateEmployee.setAge(age);
        updateEmployee.setGender(gender);

        this.baseMapper.updateById(updateEmployee);

        return this.baseMapper.selectById(id);
    }


}




