package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.AmusementFacilityAddRequest;
import com.chinasoft.backend.service.AmusementFacilityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.servlet.http.HttpServletRequest;

@RestController
@RequestMapping("/amusementFacility")
public class AmusementFacilityController {

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    /**
     * 创建
     */
    @PostMapping("/add")
    public BaseResponse<Long> add(@RequestBody AmusementFacilityAddRequest amusementFacilityAddRequest, HttpServletRequest request) {
        if (amusementFacilityAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        long newFacilityId = amusementFacilityService.add(amusementFacilityAddRequest);

       
        return ResultUtils.success(newFacilityId);
    }

}
