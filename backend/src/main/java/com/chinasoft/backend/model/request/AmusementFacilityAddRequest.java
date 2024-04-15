package com.chinasoft.backend.model.request;

import com.baomidou.mybatisplus.annotation.TableField;
import lombok.Data;

import java.sql.Time;
import java.util.List;

@Data
public class AmusementFacilityAddRequest {

    /**
     * 名称
     */
    private String name;

    /**
     * 介绍
     */
    private String introduction;

    /**
     * 经度
     */
    private String longitude;

    /**
     * 维度
     */
    private String latitude;

    /**
     * 一次游玩的人数
     */
    private Integer perUserCount;

    /**
     * 预计游玩时间（以分钟为单位）
     */
    private Integer expectTime;

    /**
     * 项目类型 可多选（过山车、轨道、失重、水上、室内、旋转、鬼屋）
     */
    private String type;

    /**
     * 适合人群（成人、老少皆宜、家长监护）
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
     * 游玩须知
     */
    private String instruction;

    /**
     * 身高下限
     */
    private Integer heightLow;

    /**
     * 身高上限
     */
    private Integer heightUp;

    /**
     * 设施照片列表
     */
    private List<String> imageUrls;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}
