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
 * 员工接口
 *
 * @author 孟祥硕
 */
@RestController
@RequestMapping("/employee")
public class EmployeeController {

    @Autowired
    private EmployeeService employeeService;

    /**
     * 登录
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
     */
    @PostMapping("/getEmployee")
    public BaseResponse<List<Employee>> getEmployee(@RequestBody GetEmployeeRequest getEmployeeRequest) {
        if (getEmployeeRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        List<Employee> employeeList = employeeService.getEmployee(getEmployeeRequest);
        return ResultUtils.success(employeeList);
    }

    /**
     * 添加员工
     */
    @PostMapping("/addEmployee")
    public BaseResponse<Employee> addEmployee(@RequestBody AddEmployeeRequest addEmployeeRequest) {
        if (addEmployeeRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        Employee employee = employeeService.addEmployee(addEmployeeRequest);
        return ResultUtils.success(employee);
    }

    /**
     * 删除员工
     */
    @PostMapping("/deleteEmployee")
    public BaseResponse<Boolean> deleteEmployee(@RequestBody DeleteEmployeeRequest deleteEmployeeRequest) {
        Long employeeId = deleteEmployeeRequest.getId();

        if (ObjectUtils.anyNull(deleteEmployeeRequest, employeeId)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        boolean res = false;

        Employee employee = employeeService.getById(employeeId);
        if (employee == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待删除的员工不存在");
        }

        res = employeeService.removeById(employeeId);

        return ResultUtils.success(res);
    }

    /**
     * 更新员工
     */
    @PostMapping("/updateEmployee")
    public BaseResponse<Employee> updateEmployee(@RequestBody UpdateEmployeeRequest updateEmployeeRequest) {
        if (updateEmployeeRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        Employee employee = employeeService.updateEmployee(updateEmployeeRequest);
        return ResultUtils.success(employee);
    }

}
