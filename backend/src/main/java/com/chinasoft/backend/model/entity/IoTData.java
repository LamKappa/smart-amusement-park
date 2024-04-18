package com.chinasoft.backend.model.entity;

import lombok.Data;
import lombok.extern.slf4j.Slf4j;

/**
 * 硬件IoT数据
 *
 * @author 孟祥硕
 */
@Slf4j
@Data
public class IoTData {
    // {'deviceId': 1, 'facilityId': 2, 'facilityType': 0, 'detection': 0}
    /**
     * 设备ID
     */
    private Integer deviceId;

    /**
     * 设施ID
     */
    private Integer facilityId;

    /**
     * 设施类型
     */
    private Integer facilityType;

    /**
     * 人体感应（0-无人，1-有人）
     */
    private Integer detection;
}
