package com.chinasoft.backend.model.vo.facility;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.Data;

import java.util.Date;
import java.util.List;

/**
 * 游乐设施信息视图
 *
 * @author 姜堂蕴之
 */
@Data
public class AmusementFacilityVO {
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
     * 一次游玩的人数
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
     * 预计等待时间
     */
    private Integer expectWaitTime;

}
