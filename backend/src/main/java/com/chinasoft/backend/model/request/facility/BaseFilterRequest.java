package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 游乐设施筛选请求
 *
 * @author 姜堂蕴之
 */
@Data
public class BaseFilterRequest {
    private Long id;

    private Long userId;
    private String name;
}
