package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;

import java.io.Serializable;
import java.util.Date;
import lombok.Data;

/**
 * 餐饮设施表
 * @TableName restaurant_facility
 */
@TableName(value ="restaurant_facility")
@Data
public class RestaurantFacility implements Serializable {
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
     * 设施介绍
     */
    private String introduction;

    /**
     * 纬度
     */
    private String longitude;

    /**
     * 经度
     */
    private String latitude;

    /**
     * 设施类型（中式快餐、西式快餐、面点、饮品、小吃）
     */
    private String type;

    /**
     * 照片URL
     */
    private String image_url;

    /**
     * 开放开始时间
     */
    private Date start_time;

    /**
     * 开放结束时间
     */
    private Date close_time;

    /**
     * 状态（0-正常，1-异常）
     */
    private Integer status;

    /**
     * 添加时间
     */
    private Date create_time;

    /**
     * 修改时间
     */
    private Date update_time;

    /**
     * 逻辑删除标志（0-未删除，1-已删除）
     */
    @TableLogic
    private Integer is_deleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}