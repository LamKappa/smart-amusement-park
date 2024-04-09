package com.chinasoft.backend.model.request;

import lombok.Data;

@Data
public class BaseFilterRequest {
    private Long id;

    private Long userId;
    private String name;
}
