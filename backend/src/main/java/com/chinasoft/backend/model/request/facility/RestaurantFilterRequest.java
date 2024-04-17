package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 餐饮设施筛选请求
 *
 * @author 姜堂蕴之
 */
@Data
public class RestaurantFilterRequest {
    /**
     * 餐饮设施ID
     */
    private Long id;

    /**
     * 用户ID
     */
    private Long userId;

    /**
     * 设施名称
     */
    private String name;

    /**
     * 餐饮类型
     */
    private String type;
}
