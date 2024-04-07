package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * @TableName facility_image
 */
@TableName(value = "facility_image")
@Data
public class FacilityImage implements Serializable {
    /**
     *
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 设施id
     */
    private Long facilityId;

    /**
     * 设施类型（0-游乐设施，1-餐饮设施，2-基础设施）
     */
    private Integer facilityType;

    /**
     * 图片url
     */
    private String imageUrl;

    /**
     *
     */
    private Date createTime;

    /**
     *
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