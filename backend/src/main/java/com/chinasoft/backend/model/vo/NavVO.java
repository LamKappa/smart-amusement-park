package com.chinasoft.backend.model.vo;

import lombok.Data;

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

    /*
     * 路径点坐标列表
     */
    private List<PositionPoint> paths;

}
