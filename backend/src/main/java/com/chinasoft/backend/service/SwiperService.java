package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.Swiper;

import java.util.List;

/**
* @author 皎皎
* @description 针对轮播图的数据库操作Service
* @createDate 2024-04-07 16:20:10
*/
public interface SwiperService {

    List<Swiper> getSwiper();

}
