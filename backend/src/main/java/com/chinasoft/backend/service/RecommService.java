package com.chinasoft.backend.service;

import com.chinasoft.backend.model.vo.RouteVO;

/**
 * @author 皎皎
 * @description 针对搜索的数据库操作Service
 * @createDate 2024-04-05 16:57:10
 */
public interface RecommService {

    /**
     * 根据名称 简介 类型 查询各种设施
     */
    RouteVO getRecommendation(Integer routeId);

    RouteVO sortByVisit();

    RouteVO sortBySubscribe();
}
