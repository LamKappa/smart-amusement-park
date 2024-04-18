package com.chinasoft.backend.model.entity.facility;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

/**
 * 设施通用属性
 *
 * @author 孟祥硕
 */
@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class Facility {
    /**
     * 设施ID
     */
    private Long id;

    /**
     * 设施类型
     */
    private Integer facilityType;

    /**
     * 名称
     */
    private String name;

    /**
     * 经度
     */
    private String longitude;

    /**
     * 纬度
     */
    private String latitude;

    /**
     * 预期等待时间
     */
    private Integer expectWaitTime;

    /**
     * 走到当前建筑的预计行走时间
     */
    private Integer expectWalkTime;
}
