package com.chinasoft.backend.model.request.facility;

import lombok.Data;

/**
 * 设施删除请求
 *
 * @author 孟祥硕
 */
@Data
public class FacilityDeleteRequest {

    private Long facilityId;

    private Integer facilityType;
}
