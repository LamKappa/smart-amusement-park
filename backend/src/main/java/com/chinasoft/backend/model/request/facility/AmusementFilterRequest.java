package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 游乐设施筛选请求
 *
 * @author 姜堂蕴之
 */
@Data
public class AmusementFilterRequest {
    /**
     * 游乐设施ID
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
     * 项目类型
     */
    private String type;

    /**
     * 用户身高
     */
    private Integer height;

    /**
     * 适合人群
     */
    private String crowd;
}
