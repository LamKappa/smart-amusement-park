package com.chinasoft.backend.model.request.employee;

import lombok.Data;

/**
 * 员工更改
 *
 * @author 孟祥硕
 */
@Data
public class UpdateEmployeeRequest {
    /**
     * 用户id
     */
    private Long id;

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