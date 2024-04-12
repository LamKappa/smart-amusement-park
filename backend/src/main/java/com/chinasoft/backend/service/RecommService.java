package com.chinasoft.backend.service;

import com.chinasoft.backend.model.request.AddRouteRequest;
import com.chinasoft.backend.model.request.RecommendationRequest;
import com.chinasoft.backend.model.vo.RouteVO;

import java.util.List;

/**
 * @author 皎皎
 * @description 针对搜索的数据库操作Service
 * @createDate 2024-04-05 16:57:10
 */
public interface RecommService {

    /**
     * 根据名称 简介 类型 查询各种设施
     */
    List<RouteVO> getRecommendation(RecommendationRequest recommendationRequest);

    RouteVO sortByVisit();

    RouteVO sortBySubscribe();

    
    RouteVO sortCrowingLevel();

    List<RouteVO> addRoute(AddRouteRequest addRouteRequest);
}
