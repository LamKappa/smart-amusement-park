package com.chinasoft.backend.model.request;

import lombok.Data;

import java.util.List;

@Data
public class EENavigationRequest {

    private String userLongitude;

    private String userLatitude;
    
    private Long facilityId;

    private Integer facilityType;
}
