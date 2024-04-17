package com.chinasoft.backend.model.request.route;

import lombok.Data;

/**
 * 推荐路线请求
 *
 * @author 姜堂蕴之
 */
@Data
public class RouteRecommendationRequest {
    /**
     * 路线ID
     */
    private Long id;
    /**
     * 路线名称
     */
    private String name;
}
