package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.facility.BaseFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.BaseFacilityUpdateRequest;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;

/**
 * 基础设施接口
 *
 * @author 孟祥硕
 */
@RestController
@RequestMapping("/baseFacility")
public class BaseFacilityController {

    @Autowired
    private BaseFacilityService baseFacilityService;

    /**
     * 创建
     */
    @PostMapping("/add")
    public BaseResponse<Long> add(@RequestBody BaseFacilityAddRequest baseFacilityAddRequest, HttpServletRequest request) {
        if (baseFacilityAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        long newFacilityId = baseFacilityService.add(baseFacilityAddRequest);


        return ResultUtils.success(newFacilityId);
    }

    /**
     * 修改
     */
    @PostMapping("/update")
    public BaseResponse<Boolean> update(@RequestBody BaseFacilityUpdateRequest baseFacilityUpdateRequest) {
        if (baseFacilityUpdateRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        Boolean res = baseFacilityService.update(baseFacilityUpdateRequest);


        return ResultUtils.success(res);

    }

}
