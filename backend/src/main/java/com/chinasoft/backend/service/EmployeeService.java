package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Employee;
import com.chinasoft.backend.model.request.employee.AddEmployeeRequest;
import com.chinasoft.backend.model.request.employee.GetEmployeeRequest;
import com.chinasoft.backend.model.request.employee.UpdateEmployeeRequest;

import javax.servlet.http.HttpServletRequest;
import java.util.List;

/**
 * 针对表【employee】的数据库操作Service
 *
 * @author 孟祥硕 姜堂蕴之
 */
public interface EmployeeService extends IService<Employee> {
    /**
     * 用户登录
     *
     * @author 孟祥硕
     */
    Employee adminLogin(String phone, String password, HttpServletRequest request);

    /**
     * 获取当前登录用户
     *
     * @author 孟祥硕
     */
    Employee getLoginEmployee(HttpServletRequest request);

    /**
     * 查询员工
     *
     * @param getEmployeeRequest 查询员工的请求对象，支持员工ID、员工姓名、员工性别等查询条件。用户不输入查询条件则返回所有员工列表。
     * @return BaseResponse<List<Employee>> 包含员工列表的响应对象，如果查询成功则返回员工列表，否则返回错误信息
     * @author 姜堂蕴之
     */
    List<Employee> getEmployee(GetEmployeeRequest getEmployeeRequest);

    /**
     * 增加员工（此处仅支持添加普通员工）
     *
     * @param addEmployeeRequest 增加员工的请求对象，包含员工的详细信息
     * @return BaseResponse<Employee> 包含新增员工的响应对象，如果增加成功则返回新增员工的信息，否则返回错误信息
     * @throws BusinessException 当请求参数为空时，抛出业务异常
     * @author 姜堂蕴之
     */
    Employee addEmployee(AddEmployeeRequest addEmployeeRequest);

    /**
     * 更新员工
     *
     * @param updateEmployeeRequest 更新员工的请求对象，包含待更新员工的ID和更新后的员工信息
     * @return BaseResponse<Employee> 包含更新后员工信息的响应对象，如果更新成功则返回更新后的员工信息，否则返回错误信息
     * @throws BusinessException 当请求参数为空时，抛出业务异常
     * @author 姜堂蕴之
     */
    Employee updateEmployee(UpdateEmployeeRequest updateEmployeeRequest);
}
