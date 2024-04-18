package com.chinasoft.backend.model.vo.facility;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.fasterxml.jackson.annotation.JsonFormat;
import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.util.Date;
import java.util.List;

/**
 * 游乐设施信息视图（加入打卡订阅）
 *
 * @author 姜堂蕴之
 */
@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class AmusementFacilityInfoVO {
    /**
     * 设施id
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 设施类型
     */
    private Integer facilityType = FacilityTypeConstant.AMUSEMENT_TYPE;

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
    private Date startTime;

    /**
     * 关闭时间
     */
    private Date closeTime;

    /**
     * 状态 0-正常 1-异常（如果是在检修的时候status为1，注意夜晚闭馆未开放的时候status为0）
     */
    private Integer status;

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
     * 设施图片
     */
    private List<String> imageUrls;


    /**
     * 是否打卡
     */
    private Integer isVisited;

    /**
     * 是否订阅
     */
    private Integer isSubscribed;

    /**
     * 打卡记录ID
     */
    private Long visitId;

    /**
     * 订阅记录ID
     */
    private Long subscribeId;

    /**
     * 预计等待时间
     */
    private Integer expectWaitTime;

    /**
     * 打卡时间
     */
    @JsonFormat(pattern = "yyyy/MM/dd HH:mm:ss")
    private Date visitTime;

    /**
     * 订阅时间
     */
    @JsonFormat(pattern = "yyyy/MM/dd HH:mm:ss")
    private Date subscribeTime;

}
