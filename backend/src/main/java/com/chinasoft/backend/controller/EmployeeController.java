package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Employee;
import com.chinasoft.backend.model.request.employee.*;
import com.chinasoft.backend.service.EmployeeService;
import org.apache.commons.lang3.ObjectUtils;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;
import java.util.List;

/**
 * 员工相关接口
 *
 * @author 孟祥硕 姜堂蕴之
 */
@RestController
@RequestMapping("/employee")
public class EmployeeController {

    @Autowired
    private EmployeeService employeeService;

    /**
     * 用户登录
     *
     * @author 孟祥硕
     */
    @PostMapping("/login")
    public BaseResponse adminLogin(@RequestBody AdminLoginRequest userLoginRequest, HttpServletRequest request) {
        if (userLoginRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        String username = userLoginRequest.getUsername();
        String password = userLoginRequest.getPassword();
        if (StringUtils.isAnyBlank(username, password)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }
        Employee user = employeeService.adminLogin(username, password, request);
        return ResultUtils.success(user);
    }

    /**
     * 查询员工
     *
     * @param getEmployeeRequest 查询员工的请求对象，支持员工ID、员工姓名、员工性别等查询条件。用户不输入查询条件则返回所有员工列表。
     * @return BaseResponse<List<Employee>> 包含员工列表的响应对象，如果查询成功则返回员工列表，否则返回错误信息
     * @author 姜堂蕴之
     */
    @PostMapping("/getEmployee")
    public BaseResponse<List<Employee>> getEmployee(@RequestBody GetEmployeeRequest getEmployeeRequest) {
        // 检查请求参数是否为空
        if (getEmployeeRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用员工服务层的查询方法，获取员工列表
        List<Employee> employeeList = employeeService.getEmployee(getEmployeeRequest);

        // 使用工具类将员工列表封装为成功的响应对象并返回
        return ResultUtils.success(employeeList);
    }

    /**
     * 增加员工（此处仅支持添加普通员工）
     *
     * @param addEmployeeRequest 增加员工的请求对象，包含员工的详细信息
     * @return BaseResponse<Employee> 包含新增员工的响应对象，如果增加成功则返回新增员工的信息，否则返回错误信息
     * @throws BusinessException 当请求参数为空时，抛出业务异常
     * @author 姜堂蕴之
     */
    @PostMapping("/addEmployee")
    public BaseResponse<Employee> addEmployee(@RequestBody AddEmployeeRequest addEmployeeRequest) {
        // 检查请求参数是否为空
        if (addEmployeeRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用员工服务层的增加方法，添加员工并返回新增员工信息
        Employee employee = employeeService.addEmployee(addEmployeeRequest);

        // 使用工具类将新增员工信息封装为成功的响应对象并返回
        return ResultUtils.success(employee);
    }

    /**
     * 删除员工
     *
     * @param deleteEmployeeRequest 删除员工的请求对象，包含待删除员工的ID
     * @return BaseResponse<Boolean> 包含删除结果的响应对象，如果删除成功则返回true，否则返回false，同时包含错误信息
     * @throws BusinessException 当请求参数为空或待删除的员工不存在时，抛出业务异常
     * @author 姜堂蕴之
     */
    @PostMapping("/deleteEmployee")
    public BaseResponse<Boolean> deleteEmployee(@RequestBody DeleteEmployeeRequest deleteEmployeeRequest) {
        // 获取待删除员工的ID
        Long employeeId = deleteEmployeeRequest.getId();

        // 检查请求参数和员工ID是否为空
        if (ObjectUtils.anyNull(deleteEmployeeRequest, employeeId)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 初始化删除结果为false
        boolean res = false;

        // 根据员工ID获取员工信息
        Employee employee = employeeService.getById(employeeId);

        // 如果员工信息为空，则待删除的员工不存在
        if (employee == null) {
            // 抛出业务异常，异常码为未找到错误，并附带错误信息
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待删除的员工不存在");
        }

        // 根据员工ID删除员工，并获取删除结果
        res = employeeService.removeById(employeeId);

        // 使用工具类将删除结果封装为成功的响应对象并返回
        return ResultUtils.success(res);
    }

    /**
     * 更新员工
     *
     * @param updateEmployeeRequest 更新员工的请求对象，包含待更新员工的ID和更新后的员工信息
     * @return BaseResponse<Employee> 包含更新后员工信息的响应对象，如果更新成功则返回更新后的员工信息，否则返回错误信息
     * @throws BusinessException 当请求参数为空时，抛出业务异常
     * @author 姜堂蕴之
     */
    @PostMapping("/updateEmployee")
    public BaseResponse<Employee> updateEmployee(@RequestBody UpdateEmployeeRequest updateEmployeeRequest) {
        // 检查请求参数是否为空
        if (updateEmployeeRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 根据请求参数更新员工信息，并返回更新后的员工对象
        Employee employee = employeeService.updateEmployee(updateEmployeeRequest);

        // 使用工具类将更新后的员工信息封装为成功的响应对象并返回
        return ResultUtils.success(employee);
    }

}
