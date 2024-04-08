package com.chinasoft.backend.service;

import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.PositionPoint;

import java.util.List;

public interface MapService {
    /**
     * 多个设施进行最优路径导航
     */
    List<PositionPoint> mulFacilityNav(NavigationRequest navigationRequest);
}
