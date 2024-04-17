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
 * 轮播信息接口
 *
 * @author 姜堂蕴之
 */
@RestController
public class SwiperController {

    @Autowired
    SwiperService swiperService;

    /**
     * 获取所有游乐设施的轮播信息
     *
     * @return 包含轮播信息的BaseResponse对象
     */
    @GetMapping("/swiper")
    public BaseResponse<List<Swiper>> getSwiper() {
        // 调用swiperService的getSwiper方法查询数据库，获取轮播信息列表
        List<Swiper> data = swiperService.getSwiper();

        // 使用ResultUtils工具类将查询结果封装为成功的BaseResponse对象，并返回
        return ResultUtils.success(data);
    }

}
