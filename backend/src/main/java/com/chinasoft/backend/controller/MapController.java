package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;
import com.chinasoft.backend.service.MapService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/map")
public class MapController {

    @Autowired
    private MapService mapService;

    /**
     * 单个设施进行最优路径导航
     */
    @PostMapping("/sinFacilityNav")
    public BaseResponse sinFacilityNav(@RequestBody EENavigationRequest eeNavigationRequest) {
        if (eeNavigationRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        NavVO navVO = mapService.sinFacilityNav(eeNavigationRequest);

        return ResultUtils.success(navVO);
    }

    /**
     * 多个设施进行最优路径导航
     */
    @PostMapping("/mulFacilityNav")
    public BaseResponse mulFacilityNav(@RequestBody NavigationRequest navigationRequest) {
        if (navigationRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        NavVO navVO = mapService.mulFacilityNav(navigationRequest);

        return ResultUtils.success(navVO);
    }
}



