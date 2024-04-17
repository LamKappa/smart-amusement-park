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
 * 针对表【employee】的数据库操作Service实现
 *
 * @author 孟祥硕 姜堂蕴之
 */
@Service
@Slf4j
public class EmployeeServiceImpl extends ServiceImpl<EmployeeMapper, Employee>
        implements EmployeeService {

    /**
     * 加密盐值
     */
    private static final String SALT = "chinasoft";

    /**
     * 员工角色默认为0（0-普通员工，1-管理员）
     */
    private static final Integer EMPLOYEE_ROLE = 0;

    /**
     * 默认头像
     */
    private static final String DEFAULT_AVATAR_URL = "https://smart-amusement-park.oss-cn-chengdu.aliyuncs.com/lamkappa.bmp";

    /**
     * 员工姓名允许的最小长度
     */
    private static final int MIN_NAME_LENGTH = 0;

    /**
     * 员工姓名允许的最大长度
     */
    private static final int MAX_NAME_LENGTH = 20;

    /**
     * 手机号的正则表达式常量
     */
    private static final String MOBILE_PHONE_PATTERN = "^1[3-9]\\d{9}$";

    @Autowired
    private EmployeeMapper employeeMapper;

    /**
     * 用户登录
     *
     * @author 孟祥硕
     */
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
     *
     * @author 孟祥硕
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

    /**
     * 查询员工
     *
     * @param getEmployeeRequest 查询员工的请求对象，支持员工ID、员工姓名、员工性别等查询条件。用户不输入查询条件则返回所有员工列表。
     * @return BaseResponse<List<Employee>> 包含员工列表的响应对象，如果查询成功则返回员工列表，否则返回错误信息
     * @author 姜堂蕴之
     */
    @Override
    public List<Employee> getEmployee(GetEmployeeRequest getEmployeeRequest) {
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();

        // 加入员工ID筛选条件
        if (getEmployeeRequest.getId() != null) {
            queryWrapper.eq("id", getEmployeeRequest.getId());
        }

        // 加入员工姓名筛选条件
        if (getEmployeeRequest.getName() != null && !getEmployeeRequest.getName().isEmpty()) {
            queryWrapper.like("name", getEmployeeRequest.getName());
        }

        // 加入员工性别筛选条件
        if (getEmployeeRequest.getGender() != null && !getEmployeeRequest.getGender().isEmpty()) {
            queryWrapper.like("gender", getEmployeeRequest.getGender());
        }

        //获取符合条件的员工列表
        List<Employee> employeeList = this.baseMapper.selectList(queryWrapper);

        // 密码脱敏
        for (Employee employee : employeeList) {
            employee.setPassword(null);
        }

        // 返回处理后的员工列表
        return employeeList;
    }

    /**
     * 增加员工（此处仅支持添加普通员工）
     *
     * @param addEmployeeRequest 增加员工的请求对象，包含员工的详细信息
     * @return BaseResponse<Employee> 包含新增员工的响应对象，如果增加成功则返回新增员工的信息，否则返回错误信息
     * @throws BusinessException 当请求参数为空时，抛出业务异常
     * @author 姜堂蕴之
     */
    @Override
    public Employee addEmployee(AddEmployeeRequest addEmployeeRequest) {
        // 获取请求对象中的员工信息
        String name = addEmployeeRequest.getName();
        String phone = addEmployeeRequest.getPhone();
        Integer age = addEmployeeRequest.getAge();
        String gender = addEmployeeRequest.getGender();

        // 校验参数
        if (ObjectUtils.anyNull(name, phone, age, gender)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 校验姓名长度
        if (name.length() > MAX_NAME_LENGTH || name.length() < MIN_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "姓名长度不在允许范围内");
        }

        // 校验手机号格式
        if (!Pattern.matches(MOBILE_PHONE_PATTERN, phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号格式错误");
        }

        // 校验年龄范围
        if (age < 18 || age > 65) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "年龄输入错误");
        }

        // 校验性别
        if (!gender.equals("男") && !gender.equals("女")) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "性别输入错误");
        }

        // 检查手机号是否已存在
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("phone", phone);
        if (this.baseMapper.selectOne(queryWrapper) != null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号重复");
        }

        // 创建员工对象，并复制请求对象中的属性值
        Employee employee = new Employee();
        BeanUtil.copyProperties(addEmployeeRequest, employee);

        // 设置默认头像
        if (employee.getAvatarUrl() == null) {
            employee.setAvatarUrl(DEFAULT_AVATAR_URL);
        }

        // 设置默认员工角色
        employee.setRole(EMPLOYEE_ROLE);

        // 插入员工信息到数据库
        this.baseMapper.insert(employee);

        // 返回新插入的员工信息
        return this.baseMapper.selectById(employee.getId());
    }

    /**
     * 更新员工
     *
     * @param updateEmployeeRequest 更新员工的请求对象，包含待更新员工的ID和更新后的员工信息
     * @return BaseResponse<Employee> 包含更新后员工信息的响应对象，如果更新成功则返回更新后的员工信息，否则返回错误信息
     * @throws BusinessException 当请求参数为空时，抛出业务异常
     * @author 姜堂蕴之
     */
    @Override
    public Employee updateEmployee(UpdateEmployeeRequest updateEmployeeRequest) {
        // 获取请求中的人员ID、姓名、手机号、年龄和性别
        Long id = updateEmployeeRequest.getId();
        String name = updateEmployeeRequest.getName();
        String phone = updateEmployeeRequest.getPhone();
        Integer age = updateEmployeeRequest.getAge();
        String gender = updateEmployeeRequest.getGender();

        // 校验参数是否为空
        if (ObjectUtils.anyNull(id, name, phone, age, gender)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 根据ID查询原始员工信息
        Employee originalEmployee = this.baseMapper.selectById(id);
        if (originalEmployee == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "待更新的人员不存在");
        }

        // 校验姓名长度
        if (name.length() > MAX_NAME_LENGTH || name.length() < MIN_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "姓名长度不在允许范围内");
        }

        // 校验手机号格式
        if (!Pattern.matches(MOBILE_PHONE_PATTERN, phone)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号格式错误");
        }

        // 校验年龄范围
        if (age < 18 || age > 65) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "年龄输入错误");
        }

        // 校验性别
        if (!gender.equals("男") && !gender.equals("女")) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "性别输入错误");
        }

        // 校验手机号是否重复
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("phone", phone);
        if (this.baseMapper.selectOne(queryWrapper) != null && !(phone.equals(originalEmployee.getPhone()))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "手机号重复");
        }

        // 创建更新后的员工对象
        Employee updateEmployee = new Employee();
        updateEmployee.setId(id);
        updateEmployee.setName(name);
        updateEmployee.setPhone(phone);
        updateEmployee.setAge(age);
        updateEmployee.setGender(gender);

        // 执行更新操作
        this.baseMapper.updateById(updateEmployee);

        // 返回更新后的员工信息
        return this.baseMapper.selectById(id);
    }
}




