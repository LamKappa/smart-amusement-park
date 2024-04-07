package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * 拥挤度表
 *
 * @TableName crowding_level
 */
@TableName(value = "crowding_level")
@Data
public class CrowdingLevel implements Serializable {
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
     * 设施类别
     */
    private Integer facilityType;

    /**
     * 预计等待时长（分钟）
     */
    private Integer expectWaitTime;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 逻辑删除（0-未删除，1-已删除）
     */
    @TableLogic
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}