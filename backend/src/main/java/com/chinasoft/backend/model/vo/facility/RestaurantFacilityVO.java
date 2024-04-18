package com.chinasoft.backend.model.vo.facility;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.Data;

import java.util.Date;
import java.util.List;

/**
 * 基础设施信息视图
 *
 * @author 姜堂蕴之
 */
@Data
public class RestaurantFacilityVO {
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
     * 每个人的预期用餐时间
     */
    private Integer expectTime;

    /**
     * 最大容纳人数
     */
    private Integer maxCapacity;

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
     * 预计等待时间
     */
    private Integer expectWaitTime;
}
