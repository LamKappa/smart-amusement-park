package com.chinasoft.backend.service;

import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.map.MultipleNavigationRequest;
import com.chinasoft.backend.model.request.map.SingleNavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;

/**
 * 地图Service
 *
 * @author 孟祥硕 姜堂蕴之
 */
public interface MapService {
    /**
     * 多个设施进行最优路径导航
     *
     * @author 孟祥硕
     */
    NavVO mulFacilityNav(MultipleNavigationRequest multipleNavigationRequest);

    /**
     * 单个设施最优路径导航
     *
     * @param singleNavigationRequest 包含导航请求的参数的请求体对象
     * @return 包含最优路径导航信息的BaseResponse对象
     * @throws BusinessException 当请求体为null时，抛出参数错误异常
     * @author 姜堂蕴之
     */
    NavVO sinFacilityNav(SingleNavigationRequest singleNavigationRequest);
}
