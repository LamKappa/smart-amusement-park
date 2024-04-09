package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;

import java.io.Serializable;
import java.util.Date;
import lombok.Data;

/**
 * 用户订阅记录表
 * @TableName subscribe
 */
@TableName(value ="subscribe")
@Data
public class Subscribe implements Serializable {
    /**
     * 主键ID
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 用户ID
     */
    private Long userId;

    /**
     * 设施ID
     */
    private Long facilityId;

    /**
     * 设施类别（0-游乐设施，1-餐厅设施，2-基础设施）
     */
    private Integer facilityType;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 逻辑删除（0-未删除 1-已删除）
     */
    @TableLogic
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}