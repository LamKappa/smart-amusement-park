package com.chinasoft.backend.model.request;

import lombok.Data;

@Data
public class AmusementFilterRequest {
    private Long id;

    private Long userId;
    private String name;
    private String type;
    private Integer height;
    private String crowd;
}
