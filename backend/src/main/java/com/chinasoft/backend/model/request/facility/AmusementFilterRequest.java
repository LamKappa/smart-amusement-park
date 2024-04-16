package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 游乐设施筛选请求
 *
 * @author 姜堂蕴之
 */
@Data
public class AmusementFilterRequest {
    private Long id;

    private Long userId;
    private String name;
    private String type;
    private Integer height;
    private String crowd;
}
