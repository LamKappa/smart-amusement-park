package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.RestaurantFacilityAddRequest;
import com.chinasoft.backend.model.request.RestaurantFacilityUpdateRequest;
import com.chinasoft.backend.service.RestaurantFacilityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;

@RestController
@RequestMapping("/restaurantFacility")
public class RestaurantFacilityController {

    @Autowired
    private RestaurantFacilityService restaurantFacilityService;

    /**
     * 创建
     */
    @PostMapping("/add")
    public BaseResponse<Long> add(@RequestBody RestaurantFacilityAddRequest restaurantFacilityAddRequest, HttpServletRequest request) {
        if (restaurantFacilityAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        long newFacilityId = restaurantFacilityService.add(restaurantFacilityAddRequest);


        return ResultUtils.success(newFacilityId);
    }

    /**
     * 修改
     */
    @PostMapping("/update")
    public BaseResponse<Boolean> update(@RequestBody RestaurantFacilityUpdateRequest restaurantFacilityUpdateRequest) {
        if (restaurantFacilityUpdateRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        Boolean res = restaurantFacilityService.update(restaurantFacilityUpdateRequest);


        return ResultUtils.success(res);

    }

}
