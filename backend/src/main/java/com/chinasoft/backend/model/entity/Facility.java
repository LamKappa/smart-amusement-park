package com.chinasoft.backend.model.entity;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class Facility {

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
     * 维度
     */
    private String latitude;

    /**
     * 预期等待时间
     */
    private Integer expectWaitTime;
}
