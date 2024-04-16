package com.chinasoft.backend.model.request.map;

import lombok.Data;

/**
 * 单个设施导航请求
 *
 * @author 姜堂蕴之
 */
@Data
public class SingleNavigationRequest {

    private String userLongitude;

    private String userLatitude;

    private Long facilityId;

    private Integer facilityType;
}
