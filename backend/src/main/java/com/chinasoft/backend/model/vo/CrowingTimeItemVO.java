package com.chinasoft.backend.model.vo;

import lombok.Data;

@Data
public class CrowingTimeItemVO {

    /**
     * 时间（小时）
     */
    private Integer time;

    /**
     * 预期等待时间
     */
    private Integer expectWaitTime;
}
