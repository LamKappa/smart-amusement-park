package com.chinasoft.backend.model.request.employee;

import lombok.Data;

/**
 * 查询员工请求
 *
 * @author 姜堂蕴之
 */
@Data
public class GetEmployeeRequest {

    /**
     * 人员id
     */
    private Long id;

    /**
     * 姓名
     */
    private String name;

    /**
     * 性别
     */
    private String gender;

}