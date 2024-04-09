package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.request.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.*;
import com.chinasoft.backend.service.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/filter")
public class FilterController {

//    @Autowired
//    AmusementFacilityService amusementFacilityService;
//
//    @Autowired
//    RestaurantFacilityService restaurantFacilityService;
//
//    @Autowired
//    BaseFacilityService baseFacilityService;
//
//
//    /*
//    游乐设施筛选
//     */
//    @PostMapping("/amusement")
//    public BaseResponse<List<AmusementFacilityVO>> filterAmusement(@RequestBody AmusementFilterRequest amusementFilterRequest) {
//        // 异常处理
//        if (amusementFilterRequest == null) {
//            throw new BusinessException(ErrorCode.PARAMS_ERROR);
//        }
//
//        // 查询数据库
//        List<AmusementFacilityVO> data = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
//
//        // 返回响应
//        return ResultUtils.success(data);
//    }
//
//    /*
//    餐厅设施筛选
//     */
//
//    @PostMapping("/restaurant")
//    public BaseResponse<List<RestaurantFacilityVO>> filterRestaurant(@RequestBody RestaurantFilterRequest restaurantFilterRequest) {
//        // 异常处理
//        if (restaurantFilterRequest == null) {
//            throw new BusinessException(ErrorCode.PARAMS_ERROR);
//        }
//
//        // 查询数据库
//        List<RestaurantFacilityVO> data = restaurantFacilityService.getRestaurantFacility(restaurantFilterRequest);
//
//        // 返回响应
//        return ResultUtils.success(data);
//    }
//
//    /*
//   基础设施（卫生间）筛选
//    */
//
//    @PostMapping("/base")
//    public BaseResponse<List<BaseFacilityVO>> filterBase(@RequestBody BaseFilterRequest baseFilterRequest) {
//        // 异常处理
//        if (baseFilterRequest == null) {
//            throw new BusinessException(ErrorCode.PARAMS_ERROR);
//        }
//
//        // 查询数据库
//        List<BaseFacilityVO> data = baseFacilityService.getBaseFacility(baseFilterRequest);
//
//        // 返回响应
//        return ResultUtils.success(data);
//    }

    @Autowired
    VisitAndSubscribeService visitAndSubscribeService;

    /**
     * 展示游乐设施所有信息+打卡信息+订阅信息。
     */
    @PostMapping("/amusement")
    public BaseResponse<List<AmusementVandSVO>> getAmusementVandS(@RequestBody AmusementFilterRequest amusementFilterRequest) {
        if (amusementFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<AmusementVandSVO> data = visitAndSubscribeService.getAmusementVAndS(amusementFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 展示餐厅设施所有信息+打卡信息+订阅信息。
     */
    @PostMapping("/restaurant")
    public BaseResponse<List<RestaurantVandSVO>> getRestaurantVandS(@RequestBody RestaurantFilterRequest restaurantFilterRequest) {
        if (restaurantFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<RestaurantVandSVO> data = visitAndSubscribeService.getRestaurantVAndS(restaurantFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 展示基础设施所有信息+打卡信息+订阅信息。
     */
    @PostMapping("/base")
    public BaseResponse<List<BaseVandSVO>> getBaseVandS(@RequestBody BaseFilterRequest baseFilterRequest) {
        if (baseFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<BaseVandSVO> data = visitAndSubscribeService.getBaseVAndS(baseFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

}
