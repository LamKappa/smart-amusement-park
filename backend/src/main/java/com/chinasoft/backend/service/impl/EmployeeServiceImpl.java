package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.model.entity.Employee;
import com.chinasoft.backend.model.entity.RecommRoute;
import com.chinasoft.backend.model.entity.Route;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.request.GetEmployeeRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.EmployeeService;
import com.chinasoft.backend.mapper.EmployeeMapper;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

/**
* @author 皎皎
* @description 针对表【employee】的数据库操作Service实现
* @createDate 2024-04-12 17:47:21
*/
@Service
public class EmployeeServiceImpl extends ServiceImpl<EmployeeMapper, Employee>
    implements EmployeeService{

    @Override
    public List<Employee> getEmployee(GetEmployeeRequest getEmployeeRequest) {
        QueryWrapper<Employee> queryWrapper = new QueryWrapper<>();

        if (getEmployeeRequest.getId() != null) {
            queryWrapper.eq("id", getEmployeeRequest.getId());
        }

        if (getEmployeeRequest.getName() != null) {
            queryWrapper.eq("name", getEmployeeRequest.getName());
        }

        if (getEmployeeRequest.getGender() != null) {
            queryWrapper.eq("gender", getEmployeeRequest.getGender());
        }

        return this.baseMapper.selectList(queryWrapper);
    }
}




