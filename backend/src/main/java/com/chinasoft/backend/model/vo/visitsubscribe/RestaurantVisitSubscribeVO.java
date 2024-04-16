package com.chinasoft.backend.model.vo.visitsubscribe;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.fasterxml.jackson.annotation.JsonFormat;
import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.util.Date;
import java.util.List;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class RestaurantVisitSubscribeVO {
    /**
     * 设施ID
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 设施类型
     */
    private Integer facilityType = FacilityTypeConstant.RESTAURANT_TYPE;

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
     * 每个人的预期用餐时间
     */
    private Integer expectTime;

    /**
     * 最大容纳人数
     */
    private Integer maxCapacity;

    /**
     * 设施类型（中式快餐、西式快餐、面点、饮品、小吃）
     */
    private String type;

    /**
     * 开放开始时间
     */
    private Date startTime;

    /**
     * 开放结束时间
     */
    private Date closeTime;

    /**
     * 状态（0-正常，1-异常）
     */
    private Integer status;

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
