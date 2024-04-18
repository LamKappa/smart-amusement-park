package com.chinasoft.backend.model.request.map;

import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import lombok.Data;

import java.util.List;

/**
 * 多个设施导航请求
 *
 * @author 孟祥硕
 */
@Data
public class MultipleNavigationRequest {
    /**
     * 用户经度
     */
    private String userLongitude;

    /**
     * 用户纬度
     */
    private String userLatitude;

    /**
     * 设施列表
     */
    private List<FacilityIdType> facilities;
}
