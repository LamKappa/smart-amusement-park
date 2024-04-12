package com.chinasoft.backend.model.request;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.Data;

@Data
public class GetEmployeeRequest {

    /**
     * 姓名
     */
    private String name;

    /**
     * 性别
     */
    private String gender;

}