package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.BaseFacilityService;
import com.chinasoft.backend.service.RestaurantFacilityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

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
    @GetMapping("/amusement/name")
    public BaseResponse<List<AmusementFacility>> filterAmusementName(@Param("name") String name) {
        // 查询数据库
        List<AmusementFacility> data = amusementFacilityService.getAmusementName(name);

        // 返回响应
        return ResultUtils.success(data);
    }

    @GetMapping("/amusement/type")
    public BaseResponse<List<AmusementFacility>> filterAmusementType(@Param("type") String type) {
        // 查询数据库
        List<AmusementFacility> data = amusementFacilityService.getAmusementType(type);

        // 返回响应
        return ResultUtils.success(data);
    }

    @GetMapping("/amusement/height")
    public BaseResponse<List<AmusementFacility>> filterAmusementHeight(@Param("height") Integer height) {
        // 查询数据库
        List<AmusementFacility> data = amusementFacilityService.getAmusementHeight(height);

        // 返回响应
        return ResultUtils.success(data);
    }

    @GetMapping("/amusement/crowd")
    public BaseResponse<List<AmusementFacility>> filterAmusementCrowd(@Param("crowd") String crowd) {
        // 查询数据库
        List<AmusementFacility> data = amusementFacilityService.getAmusementCrowd(crowd);

        // 返回响应
        return ResultUtils.success(data);
    }

    /*
    餐厅设施筛选
     */

    @GetMapping("/restaurant/name")
    public BaseResponse<List<RestaurantFacility>> filterRestaurantName(@Param("name") String name) {
        // 查询数据库
        List<RestaurantFacility> data = restaurantFacilityService.getRestaurantName(name);

        // 返回响应
        return ResultUtils.success(data);
    }

    @GetMapping("/restaurant/type")
    public BaseResponse<List<RestaurantFacility>> filterRestaurantType(@Param("type") String type) {
        // 查询数据库
        List<RestaurantFacility> data = restaurantFacilityService.getRestaurantType(type);

        // 返回响应
        return ResultUtils.success(data);
    }

     /*
    基础设施（卫生间）筛选
     */

    @GetMapping("/base/name")
    public BaseResponse<List<BaseFacility>> filterBaseName(@Param("name") String name) {
        // 查询数据库
        List<BaseFacility> data = baseFacilityService.getBaseName(name);

        // 返回响应
        return ResultUtils.success(data);
    }
    
}
