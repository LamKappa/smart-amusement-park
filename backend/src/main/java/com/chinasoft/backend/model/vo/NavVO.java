package com.chinasoft.backend.model.vo;

import lombok.Data;

import java.time.LocalDateTime;
import java.util.List;

@Data
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
     * 路径点坐标列表
     */
    private List<PositionPoint> paths;

}
