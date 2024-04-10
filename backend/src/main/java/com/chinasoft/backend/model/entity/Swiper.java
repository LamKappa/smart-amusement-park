package com.chinasoft.backend.model.entity;

import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class Swiper implements Serializable {
    /**
     * 名称
     */
    private String name;

    /**
     * 开放时间
     */
    private Date startTime;

    /**
     * 关闭时间
     */
    private Date closeTime;

    /**
     * 设施图片
     */
    private String imageUrl;

}