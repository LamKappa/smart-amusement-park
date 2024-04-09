package com.chinasoft.backend.model.request;

import lombok.Data;

import java.util.List;

@Data
public class VisitAndSubscribeAddRequest {

    private Long userId;
    
    private FacilityIdType facility;
}
