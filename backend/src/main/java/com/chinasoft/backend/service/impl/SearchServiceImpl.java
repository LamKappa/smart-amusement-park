package com.chinasoft.backend.service.impl;

import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;
import com.chinasoft.backend.model.vo.facility.BaseFacilityVO;
import com.chinasoft.backend.model.vo.facility.RestaurantFacilityVO;
import com.chinasoft.backend.service.facility.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

/**
 * 设施搜索Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class SearchServiceImpl implements SearchService {
    @Autowired
    private FilterService filterService;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private RestaurantFacilityService restaurantFacilityService;

    @Autowired
    private BaseFacilityService baseFacilityService;

    /**
     * 根据关键字对设施名称、设施简介、设施类型进行搜索
     *
     * @param keyword 搜索关键字，可能包含设施名称、简介、类型等信息
     * @return BaseResponse<List<Object>> 包含设施信息的列表的响应对象
     * @throws BusinessException 当传入的参数为空时，抛出业务异常
     */
    @Override
    public List<Object> search(String keyword) {
        // 创建一个返回设施列表
        List<Object> resultList = new ArrayList<>();

        // 对游乐设施进行关键字搜索
        List<AmusementFacilityVO> amusementFacilities = amusementFacilityService.searchAmusementFacility(keyword);

        // 对餐饮设施进行关键字搜索
        List<RestaurantFacilityVO> restaurantFacilities = restaurantFacilityService.searchRestaurantFacility(keyword);

        // 对餐饮设施进行关键字搜索
        List<BaseFacilityVO> baseFacilities = baseFacilityService.searchBaseFacility(keyword);

        resultList.addAll(amusementFacilities);
        resultList.addAll(restaurantFacilities);
        resultList.addAll(baseFacilities);

        return resultList;
    }
}




