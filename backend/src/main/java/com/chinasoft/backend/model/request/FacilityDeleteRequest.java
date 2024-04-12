package com.chinasoft.backend.model.request;

import lombok.Data;

@Data
public class FacilityDeleteRequest {

    private Long facilityId;
    
    private Integer facilityType;
}
