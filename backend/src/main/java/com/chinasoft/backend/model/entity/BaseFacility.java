package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * 基础设施表
 *
 * @TableName base_facility
 */
@TableName(value = "base_facility")
@Data
public class BaseFacility implements Serializable {
    /**
     * 设施ID
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 设施名称
     */
    private String name;

    /**
     * 纬度
     */
    private String longitude;

    /**
     * 经度
     */
    private String latitude;

    /**
     * 照片URL
     */
    private String imageUrl;

    /**
     * 开放开始时间
     */
    private Date startTime;

    /**
     * 开放结束时间
     */
    private Date closeTime;

    /**
     * 状态（0-正常，1-异常）
     */
    private Integer status;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 逻辑删除标志（0-未删除，1-已删除）
     */
    @TableLogic
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}