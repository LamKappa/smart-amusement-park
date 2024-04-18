package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 设施删除请求
 *
 * @author 孟祥硕
 */
@Data
public class FacilityDeleteRequest {
    /**
     * 设施ID
     */
    private Long facilityId;

    /**
     * 设施类型
     */
    private Integer facilityType;
}
