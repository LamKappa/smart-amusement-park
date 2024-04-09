package com.chinasoft.backend.model.request;

import lombok.Data;

@Data
public class RestaurantFilterRequest {
    private Long id;

    private Long userId;
    private String name;
    private String type;
}
