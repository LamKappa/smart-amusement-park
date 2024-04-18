package com.chinasoft.backend.model.entity.facility;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * 设施图片
 *
 * @authro 姜堂蕴之
 */
@TableName(value = "facility_image")
@Data
public class FacilityImage implements Serializable {
    /**
     * 主键ID
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 设施ID
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
     * 创建时间
     */
    private Date createTime;

    /**
     * 更新时间
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