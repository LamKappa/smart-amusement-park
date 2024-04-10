package com.chinasoft.backend.model.entity;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.time.LocalDateTime;
import java.util.Date;


@Data
public class Walk implements Serializable {
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
    private LocalDateTime expectArriveTime;

}