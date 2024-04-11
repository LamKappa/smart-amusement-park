package com.chinasoft.backend.model.vo;

import lombok.Data;

import java.util.List;

@Data
public class CrowingTimeCountVO {

    /**
     * 设施名称
     */
    private Long facilityId;

    /**
     * 设施名字
     */
    private String facilityName;

    /**
     * 拥挤度时间统计
     */
    List<CrowingTimeItemVO> crowingTimeItemVOList;

}
