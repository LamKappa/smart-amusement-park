package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.Employee;
import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.request.GetEmployeeRequest;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【employee】的数据库操作Service
* @createDate 2024-04-12 17:47:21
*/
public interface EmployeeService extends IService<Employee> {

    List<Employee> getEmployee(GetEmployeeRequest getEmployeeRequest);
}
