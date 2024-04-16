package com.chinasoft.backend.service.route;

import com.chinasoft.backend.model.request.route.AddRouteRequest;
import com.chinasoft.backend.model.request.route.DeleteRouteRequest;
import com.chinasoft.backend.model.request.route.RouteRecommendationRequest;
import com.chinasoft.backend.model.request.route.UpdateRouteRequest;
import com.chinasoft.backend.model.vo.RouteVO;

import java.util.List;

/**
 * @author 姜堂蕴之
 * @description 针对搜索的数据库操作Service
 * @createDate 2024-04-05 16:57:10
 */
public interface RecommendationRouteService {

    /**
     * 根据名称 简介 类型 查询各种设施
     */
    List<RouteVO> getRecommendation(RouteRecommendationRequest routeRecommendationRequest);

    RouteVO sortByVisit();

    RouteVO sortBySubscribe();


    RouteVO sortCrowingLevel();

    List<RouteVO> addRoute(AddRouteRequest addRouteRequest);

    Boolean deleteRoute(DeleteRouteRequest deleteRouteRequest);

    List<RouteVO> updateRoute(UpdateRouteRequest updateRouteRequest);
}
