package com.chinasoft.backend.model.entity.facility;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.sql.Time;
import java.util.Date;

/**
 * 餐饮设施表
 *
 * @TableName restaurant_facility
 * @author 姜堂蕴之
 */
@TableName(value = "restaurant_facility")
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
     * 设施经度
     */
    private String longitude;

    /**
     * 设施纬度
     */
    private String latitude;

    /**
     * 餐饮类型
     */
    private String type;

    /**
     * 开始时间
     */
    private Time startTime;

    /**
     * 结束时间
     */
    private Time closeTime;

    /**
     * 状态（0-正常，1-异常）
     */
    private Integer status;

    /**
     * 每个人的预期用餐时间
     */
    private Integer expectTime;

    /**
     * 最大容纳人数
     */
    private Integer maxCapacity;

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