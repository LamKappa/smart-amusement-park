package com.chinasoft.backend.model.request.map;

import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import lombok.Data;

import java.util.List;

/**
 * 多个设施导航请求
 *
 * @author 孟祥硕
 */
@Data
public class MultipleNavigationRequest {

    private String userLongitude;

    private String userLatitude;

    private List<FacilityIdType> facilities;
}
