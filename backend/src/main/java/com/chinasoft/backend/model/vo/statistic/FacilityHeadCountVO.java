package com.chinasoft.backend.model.vo.statistic;

import lombok.Data;

@Data
public class FacilityHeadCountVO {
    /**
     * 设施id
     */
    private Long facilityId;

    /**
     * 设施名称
     */
    private String facilityName;

    /**
     * 游玩人数
     */
    private Integer headCount;
}
