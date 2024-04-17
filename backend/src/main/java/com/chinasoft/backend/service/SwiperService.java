package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.Swiper;

import java.util.List;

/**
 * 针对轮播图的数据库操作Service
 *
 * @author 姜堂蕴之
 */
public interface SwiperService {

    /**
     * 获取所有游乐设施的轮播信息
     *
     * @return 包含轮播信息的BaseResponse对象
     */
    List<Swiper> getSwiper();

}
