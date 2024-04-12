package com.chinasoft.backend.model.request;

import com.chinasoft.backend.model.entity.Facility;
import lombok.Data;

import java.util.List;

@Data
public class AddRouteRequest {
    private String name;
    private String imgUrl;
    List<Integer> facilityIdList;
}
