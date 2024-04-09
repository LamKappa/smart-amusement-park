package com.chinasoft.backend.model.vo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.Data;

import java.util.Date;
import java.util.List;

@Data
public class RestaurantVandSVO {
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

}
