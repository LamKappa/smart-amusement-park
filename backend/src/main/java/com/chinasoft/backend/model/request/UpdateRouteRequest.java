package com.chinasoft.backend.model.request;

import lombok.Data;

import java.util.List;

@Data
public class UpdateRouteRequest {
    private Long id;
    private String name;
    private String imgUrl;
    List<Integer> facilityIdList;
}
