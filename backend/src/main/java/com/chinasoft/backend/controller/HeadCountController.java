package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.FacilityHeadCount;
import com.chinasoft.backend.service.MqttService;
import com.chinasoft.backend.service.SearchService;
import com.chinasoft.backend.service.TotalHeadcountService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class HeadCountController {

    @Autowired
    MqttService mqttService;

    /**
     * 返回当前的今日游玩总人数
     *
     * @param
     * @return
     */
    @GetMapping("/headCount/total")
    public BaseResponse<Integer> getTotalCount() {

        // 查询数据库
        Integer data = mqttService.getTotalCount();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 按设施id返回当前的今日游玩总人数
     *
     * @param
     * @return
     */
    @GetMapping("/headCount/facility")
    public BaseResponse<List<FacilityHeadCount>> getFacilityCount(@Param("facilityId") Integer facilityId) {

        // 查询数据库
        List<FacilityHeadCount> data = mqttService.getFacilityCount();

        // 返回响应
        return ResultUtils.success(data);
    }

}
