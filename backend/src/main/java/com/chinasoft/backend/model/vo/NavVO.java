package com.chinasoft.backend.model.vo;

import com.chinasoft.backend.model.entity.Facility;
import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.util.List;

@Data
// 进行Json转换时候，不转换为null的字段
@JsonInclude(JsonInclude.Include.NON_NULL)
public class NavVO {
    /**
     * 总的等待时间
     */
    private Integer totalWaitTime;

    /**
     * 预计行走时间
     */
    private Integer expectWalkTime;

    /**
     * 预计行走路程
     */
    private Integer expectWalkDistance;

    /**
     * 预计到达时间
     */
    private String expectArriveTime;

    /**
     * 设施列表
     */
    private List<Facility> facilities;

    /**
     * 路径点坐标列表
     */
    private List<PositionPoint> paths;

}
