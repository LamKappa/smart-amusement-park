package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.Swiper;
import com.chinasoft.backend.service.SwiperService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 轮播图接口
 *
 * @author 姜堂蕴之
 */
@RestController
public class SwiperController {

    @Autowired
    SwiperService swiperService;

    /**
     * 根据名称 简介 类型 查询各种设施
     */
    @GetMapping("/swiper")
    public BaseResponse<List<Swiper>> getSwiper() {
        // 查询数据库
        List<Swiper> data = swiperService.getSwiper();

        // 返回响应
        return ResultUtils.success(data);
    }

}
