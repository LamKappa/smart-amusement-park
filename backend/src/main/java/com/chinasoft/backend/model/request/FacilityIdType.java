package com.chinasoft.backend.model.request;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class FacilityIdType {
    private Long facilityId;
    private Integer facilityType;

}
