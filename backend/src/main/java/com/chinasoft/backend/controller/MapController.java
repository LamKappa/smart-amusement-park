package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.map.MultipleNavigationRequest;
import com.chinasoft.backend.model.request.map.SingleNavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;
import com.chinasoft.backend.service.MapService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

/**
 * 地图接口
 *
 * @author 孟祥硕 姜堂蕴之
 */
@RestController
@RequestMapping("/map")
public class MapController {

    @Autowired
    private MapService mapService;

    /**
     * 单个设施最优路径导航
     *
     * @param singleNavigationRequest 包含导航请求的参数的请求体对象
     * @return 包含最优路径导航信息的BaseResponse对象
     * @throws BusinessException 当请求体为null时，抛出参数错误异常
     * @author 姜堂蕴之
     */
    @PostMapping("/sinFacilityNav")
    public BaseResponse sinFacilityNav(@RequestBody SingleNavigationRequest singleNavigationRequest) {
        // 检查请求体是否为空
        if (singleNavigationRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 获取导航结果，返回NavVO对象
        NavVO navVO = mapService.sinFacilityNav(singleNavigationRequest);

        // 将NavVO对象封装成成功的BaseResponse对象并返回
        return ResultUtils.success(navVO);
    }

    /**
     * 基于多个设施进行最优路径导航
     *
     * @author 孟祥硕
     */
    @PostMapping("/mulFacilityNav")
    public BaseResponse mulFacilityNav(@RequestBody MultipleNavigationRequest multipleNavigationRequest) {
        if (multipleNavigationRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        NavVO navVO = mapService.mulFacilityNav(multipleNavigationRequest);

        return ResultUtils.success(navVO);
    }
}



