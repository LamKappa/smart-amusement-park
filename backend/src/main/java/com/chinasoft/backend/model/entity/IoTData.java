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
    private Integer deviceId;
    private Integer facilityId;
    private Integer facilityType;
    private Integer detection;
}
