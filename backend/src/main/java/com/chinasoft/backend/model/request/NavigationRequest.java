package com.chinasoft.backend.model.request;

import lombok.Data;

import java.util.List;

@Data
public class NavigationRequest {

    private String userLongitude;

    private String userLatitude;
    
    private List<FacilityIdType> facilities;
}
