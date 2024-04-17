package com.chinasoft.backend.model.entity;

import lombok.Data;

import java.io.Serializable;

/**
 * 轮播信息
 *
 * @author 姜堂蕴之
 */
@Data
public class Swiper implements Serializable {
    /**
     * 设施名称
     */
    private String name;

    /**
     * 设施图片
     */
    private String imageUrl;

}