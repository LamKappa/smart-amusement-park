package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;

import java.io.Serializable;
import java.util.Date;
import lombok.Data;

/**
 * 
 * @TableName amusement_facility
 */
@TableName(value ="amusement_facility")
@Data
public class AmusementFacility implements Serializable {
    /**
     * 设施ID
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 名称
     */
    private String name;

    /**
     * 介绍
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
     * 一次游玩的人数

     */
    private Integer per_user_count;

    /**
     * 预计游玩时间（以分钟为单位）
     */
    private Integer expect_time;

    /**
     * 项目类型 可多选（过山车、轨道、失重、水上、室内、旋转、鬼屋）
     */
    private String type;

    /**
     * 适合人群（成人、老少皆宜、家长监护）
     */
    private String crowd_type;

    /**
     * 设施照片
     */
    private String image_url;

    /**
     * 开放时间
     */
    private Date start_time;

    /**
     * 关闭时间
     */
    private Date close_time;

    /**
     * 状态 0-正常 1-异常（如果是在检修的时候status为1，注意夜晚闭馆未开放的时候status为0）
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
     * 逻辑删除(0-未删除，1-已删除)
     */
    @TableLogic
    private Integer is_deleted;

    /**
     * 游玩须知
     */
    private String instruction;

    /**
     * 身高下限
     */
    private Integer height_low;

    /**
     * 身高上限
     */
    private Integer height_up;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;

}