package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.Swiper;
import com.chinasoft.backend.service.SearchService;
import com.chinasoft.backend.service.SwiperService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class SwiperController {

    @Autowired
    SwiperService swiperService;

    /**
     * @param
     * @return
     */
    @GetMapping("/swiper")
    public BaseResponse<List<Swiper>> getSwiper() {
        // 查询数据库
        List<Swiper> data = swiperService.getSwiper();

        // 返回响应
        return ResultUtils.success(data);
    }

}
