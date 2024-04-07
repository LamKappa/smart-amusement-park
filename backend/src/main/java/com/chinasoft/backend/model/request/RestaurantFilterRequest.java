package com.chinasoft.backend.model.request;

import lombok.Data;

@Data
public class RestaurantFilterRequest {
    private String name;
    private String type;
}
