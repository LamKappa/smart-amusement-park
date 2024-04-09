package com.chinasoft.backend.service;

import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;

public interface MapService {
    /**
     * 多个设施进行最优路径导航
     */
    NavVO mulFacilityNav(NavigationRequest navigationRequest);

    /**
     * 单个设施进行导航
     */
    NavVO sinFacilityNav(EENavigationRequest eeNavigationRequest);
}
