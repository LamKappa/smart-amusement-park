package com.chinasoft.backend.model.entity;

import lombok.Data;

import java.io.Serializable;
import java.time.LocalDateTime;

/**
 * 行走信息
 *
 * @author 姜堂蕴之
 */
@Data
public class Walk implements Serializable {
    /**
     * 预计行走时间（以分钟为单位）
     */
    private Integer expectWalkTime;

    /**
     * 预计行走路程（以米为单位）
     */
    private Integer expectWalkDistance;

    /**
     * 预计到达时间
     */
    private LocalDateTime expectArriveTime;

}