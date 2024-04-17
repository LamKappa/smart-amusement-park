package com.chinasoft.backend.model.request.employee;

import lombok.Data;

/**
 * 员工添加请求
 *
 * @author 姜堂蕴之
 */
@Data
public class AddEmployeeRequest {

    /**
     * 姓名
     */
    private String name;

    /**
     * 手机号
     */
    private String phone;

    /**
     * 年龄
     */
    private Integer age;

    /**
     * 性别
     */
    private String gender;

}