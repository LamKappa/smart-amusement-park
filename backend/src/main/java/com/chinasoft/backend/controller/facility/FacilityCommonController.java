package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.entity.facility.BaseFacility;
import com.chinasoft.backend.model.entity.facility.RestaurantFacility;
import com.chinasoft.backend.model.request.facility.FacilityDeleteRequest;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import com.chinasoft.backend.service.facility.RestaurantFacilityService;
import org.apache.commons.lang3.ObjectUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

/**
 * 通用设施接口
 *
 * @author 孟祥硕
 */
@RestController
@RequestMapping("/facility")
public class FacilityCommonController {

    @Autowired
    private AmusementFacilityService amusementFacilityService;
    @Autowired
    private RestaurantFacilityService restaurantFacilityService;
    @Autowired
    private BaseFacilityService baseFacilityService;


    @PostMapping("/delete")
    public BaseResponse<Boolean> deleteFacility(@RequestBody FacilityDeleteRequest facilityDeleteRequest) {
        Long facilityId = facilityDeleteRequest.getFacilityId();
        Integer facilityType = facilityDeleteRequest.getFacilityType();

        if (ObjectUtils.anyNull(facilityDeleteRequest, facilityId, facilityType)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        boolean res = false;
        if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
            AmusementFacility facility = amusementFacilityService.getById(facilityId);
            if (facility == null) {
                throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "要删除的设施不存在");
            }
            res = amusementFacilityService.removeById(facilityId);
        }
        if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
            RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
            if (facility == null) {
                throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "要删除的设施不存在");
            }
            res = restaurantFacilityService.removeById(facilityId);

        }
        if (facilityType == FacilityTypeConstant.BASE_TYPE) {
            BaseFacility facility = baseFacilityService.getById(facilityId);
            if (facility == null) {
                throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "要删除的设施不存在");
            }
            res = baseFacilityService.removeById(facilityId);
        }
        return ResultUtils.success(res);
    }
}
