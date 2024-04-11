package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Route;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.RecommService;
import com.chinasoft.backend.service.RouteService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class RouteController {

    @Autowired
    RecommService recommService;

    @Autowired
    RouteService routeService;

    /**
     * 根据名称 简介 类型 查询各种设施
     */
    @GetMapping("/recommendation")
    public BaseResponse<RouteVO> getRecommendation(@Param("id") Integer routeId) {
        if (routeId == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        RouteVO data = recommService.getRecommendation(routeId);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 推荐路线导航
     */
    @GetMapping("/recommendation/countPlus")
    public BaseResponse<Boolean> recommendationRouteNav(@Param("id") Integer routeId) {

        if (routeId == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 统计次数+1
        Route route = routeService.getById(routeId);
        route.setUseCount(route.getUseCount() + 1);
        boolean b = routeService.updateById(route);

        return ResultUtils.success(b);
    }

    /**
     * 返回由打卡最多的四个设施组成的游玩路线
     * @return
     */
    @GetMapping("/recommendation/sortByVisit")
    public BaseResponse<RouteVO> sortByVisit (){
        // 查询数据库
        RouteVO data = recommService.sortByVisit();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 返回由订阅最多的四个设施组成的游玩路线
     * @return
     */
    @GetMapping("/recommendation/sortBySubscribe")
    public BaseResponse<RouteVO> sortBySubscribe (){
        // 查询数据库
        RouteVO data = recommService.sortBySubscribe();

        // 返回响应
        return ResultUtils.success(data);
    }
}
