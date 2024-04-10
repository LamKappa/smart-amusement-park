package com.chinasoft.backend.model.entity;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

@Data
public class Swiper implements Serializable {
    /**
     * 名称
     */
    private String name;

    /**
     * 设施图片
     */
    private String imageUrl;

}