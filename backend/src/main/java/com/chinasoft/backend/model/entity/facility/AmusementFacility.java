package com.chinasoft.backend.model.entity.facility;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.sql.Time;
import java.util.Date;

/**
 * 游乐设施表
 *
 * @TableName amusement_facility
 * @author 姜堂蕴之
 */
@TableName(value = "amusement_facility")
@Data
public class AmusementFacility implements Serializable {
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
     * 设施简介
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
     * 平均一次游玩的人数
     */
    private Integer perUserCount;

    /**
     * 预计游玩时间（以分钟为单位）
     */
    private Integer expectTime;

    /**
     * 项目类型
     */
    private String type;

    /**
     * 适合人群
     */
    private String crowdType;

    /**
     * 开放时间
     */
    private Time startTime;

    /**
     * 关闭时间
     */
    private Time closeTime;

    /**
     * 状态 0-正常 1-异常（如果是在检修的时候status为1，注意夜晚闭馆未开放的时候status为0）
     */
    private Integer status;

    /**
     * 游玩须知
     */
    private String instruction;

    /**
     * 身高下限（以厘米为单位，无限制为0）
     */
    private Integer heightLow;

    /**
     * 身高上限（以厘米为单位，无限制为300）
     */
    private Integer heightUp;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 逻辑删除(0-未删除，1-已删除)
     */
    @TableLogic
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}