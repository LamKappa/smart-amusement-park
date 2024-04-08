package com.chinasoft.backend.model.vo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class PositionPoint {


    /**
     * 经度
     */
    private String longitude;

    /**
     * 纬度
     */
    private String latitude;

}
