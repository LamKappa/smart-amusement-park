package com.chinasoft.backend.model.entity;

import lombok.Data;

import java.io.Serializable;
import java.time.LocalDateTime;


@Data
public class FacilityHeadCount implements Serializable {
    /**
     * 设施id
     */
    private Integer facilityId;

    /**
     * 设施名称
     */
    private String facilityName;

    /**
     * 当前游玩人数
     */
    private Integer headCount;

}