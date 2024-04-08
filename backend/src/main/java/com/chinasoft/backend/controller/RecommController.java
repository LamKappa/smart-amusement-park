package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.RecommService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class RecommController {

    @Autowired
    RecommService recommService;

    /**
     * 根据名称 简介 类型 查询各种设施
     *
     * @param routeId
     * @return
     */
    @GetMapping("/recommendation")
    public BaseResponse<RouteVO> getRecommendation(@Param("id") Integer routeId) {
        if (routeId == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        RouteVO data = recommService.getRecommdation(routeId);

        // 返回响应
        return ResultUtils.success(data);
    }

}
