package com.chinasoft.backend.model.vo.statistic;

import lombok.Data;

import java.util.List;

/**
 * @author 孟祥硕
 */
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
