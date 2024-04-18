package com.chinasoft.backend.model.vo.statistic;

import lombok.Data;

/**
 * @author 孟祥硕
 */
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
