package com.chinasoft.backend.service;

import com.chinasoft.backend.model.request.map.MultipleNavigationRequest;
import com.chinasoft.backend.model.request.map.SingleNavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;

/**
 * 地图Service
 *
 * @author 孟祥硕
 */
public interface MapService {
    /**
     * 多个设施进行最优路径导航
     */
    NavVO mulFacilityNav(MultipleNavigationRequest multipleNavigationRequest);

    /**
     * 单个设施进行导航
     */
    NavVO sinFacilityNav(SingleNavigationRequest singleNavigationRequest);
}
