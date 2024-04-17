package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 基础设施筛选请求
 *
 * @author 姜堂蕴之
 */
@Data
public class BaseFilterRequest {
    /**
     * 基础设施ID
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
}
