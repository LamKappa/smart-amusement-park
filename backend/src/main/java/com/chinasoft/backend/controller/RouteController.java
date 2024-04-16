package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.route.Route;
import com.chinasoft.backend.model.request.route.AddRouteRequest;
import com.chinasoft.backend.model.request.route.DeleteRouteRequest;
import com.chinasoft.backend.model.request.route.RouteRecommendationRequest;
import com.chinasoft.backend.model.request.route.UpdateRouteRequest;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.route.RecommendationRouteService;
import com.chinasoft.backend.service.route.RouteService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 路线接口
 *
 * @author 姜堂蕴之
 */
@RestController
public class RouteController {

    @Autowired
    RecommendationRouteService recommendationRouteService;

    @Autowired
    RouteService routeService;

    /**
     * 根据名称 简介 类型 查询各种设施
     */
    @PostMapping("/recommendation")
    public BaseResponse<List<RouteVO>> getRecommendation(@RequestBody RouteRecommendationRequest routeRecommendationRequest) {
        if (routeRecommendationRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }


        // 查询数据库
        List<RouteVO> data = recommendationRouteService.getRecommendation(routeRecommendationRequest);

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
     *
     * @return
     */
    @GetMapping("/recommendation/sortByVisit")
    public BaseResponse<RouteVO> sortByVisit() {
        // 查询数据库
        RouteVO data = recommendationRouteService.sortByVisit();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 返回由订阅最多的四个设施组成的游玩路线
     *
     * @return
     */
    @GetMapping("/recommendation/sortBySubscribe")
    public BaseResponse<RouteVO> sortBySubscribe() {
        // 查询数据库
        RouteVO data = recommendationRouteService.sortBySubscribe();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 返回由订拥挤度排行的四个设施组成的游玩路线
     */
    @GetMapping("/recommendation/sortCrowingLevel")
    public BaseResponse<RouteVO> sortCrowingLevel() {
        // 查询数据库
        RouteVO data = recommendationRouteService.sortCrowingLevel();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 添加新路线
     */
    @PostMapping("/recommendation/addRoute")
    public BaseResponse<List<RouteVO>> addRoute(@RequestBody AddRouteRequest addRouteRequest) {
        if (addRouteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<RouteVO> data = recommendationRouteService.addRoute(addRouteRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    @PostMapping("/recommendation/deleteRoute")
    public BaseResponse<Boolean> deleteRoute(@RequestBody DeleteRouteRequest deleteRouteRequest) {
        if (deleteRouteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 查询数据库
        Boolean data = recommendationRouteService.deleteRoute(deleteRouteRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    @PostMapping("/recommendation/updateRoute")
    public BaseResponse<List<RouteVO>> updateRoute(@RequestBody UpdateRouteRequest updateRouteRequest) {
        if (updateRouteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<RouteVO> data = recommendationRouteService.updateRoute(updateRouteRequest);

        // 返回响应
        return ResultUtils.success(data);
    }
}
