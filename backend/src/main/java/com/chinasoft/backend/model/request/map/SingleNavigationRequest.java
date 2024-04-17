package com.chinasoft.backend.model.request.map;

import lombok.Data;

/**
 * 单个设施导航请求
 *
 * @author 姜堂蕴之
 */
@Data
public class SingleNavigationRequest {
    /**
     * 用户经度
     */
    private String userLongitude;

    /**
     * 用户纬度
     */
    private String userLatitude;

    /**
     * 目的设施ID
     */
    private Long facilityId;

    /**
     * 目的设施类型（0-游乐设施，1-餐饮设施，2-基础设施）
     */
    private Integer facilityType;
}
