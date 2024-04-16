package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 餐饮设施筛选请求
 *
 * @author 孟祥硕
 */
@Data
public class RestaurantFilterRequest {
    private Long id;

    private Long userId;
    private String name;
    private String type;
}
