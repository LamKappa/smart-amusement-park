package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.request.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
import com.chinasoft.backend.model.vo.BaseFacilityVO;
import com.chinasoft.backend.model.vo.RestaurantFacilityVO;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.BaseFacilityService;
import com.chinasoft.backend.service.RestaurantFacilityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/filter")
public class FilterController {

    @Autowired
    AmusementFacilityService amusementFacilityService;

    @Autowired
    RestaurantFacilityService restaurantFacilityService;

    @Autowired
    BaseFacilityService baseFacilityService;

    /*
    游乐设施筛选
     */
    @PostMapping("/amusement")
    public BaseResponse<List<AmusementFacilityVO>> filterAmusement(@RequestBody AmusementFilterRequest amusementFilterRequest) {
        // 查询数据库
        List<AmusementFacilityVO> data = amusementFacilityService.getAmusementFacility(amusementFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /*
    餐厅设施筛选
     */

    @PostMapping("/restaurant")
    public BaseResponse<List<RestaurantFacilityVO>> filterRestaurant(@RequestBody RestaurantFilterRequest restaurantFilterRequest) {
        // 查询数据库
        List<RestaurantFacilityVO> data = restaurantFacilityService.getRestaurantFacility(restaurantFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /*
   基础设施（卫生间）筛选
    */

    @PostMapping("/base")
    public BaseResponse<List<BaseFacilityVO>> filterBase(@RequestBody BaseFilterRequest baseFilterRequest) {
        // 查询数据库
        List<BaseFacilityVO> data = baseFacilityService.getBaseFacility(baseFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }
}
