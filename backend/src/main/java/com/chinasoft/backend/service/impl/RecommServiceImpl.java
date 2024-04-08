package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.mapper.RecommRouteMapper;
import com.chinasoft.backend.mapper.RouteMapper;
import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.RecommService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * @author 皎皎
 * @description 针对轮播图的数据库操作Service实现
 * @createDate 2024-04-07 16:24:00
 */
@Service
public class RecommServiceImpl implements RecommService {

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Autowired
    RouteMapper routeMapper;

    @Autowired
    RecommRouteMapper recommRouteMapper;

    @Override
    public RouteVO getRecommendation(Integer routeId) {

        // 查询推荐路线信息
        Route route = routeMapper.selectById(routeId);

        List<RecommRoute> recommRoutes = recommRouteMapper.selectList(Wrappers.<RecommRoute>query()
                .eq("route_id", routeId)
                .orderByDesc("priority"));

        List<Swiper> swiperList = new ArrayList<>();
        for (RecommRoute recommRoute : recommRoutes) {
            // 根据facility_id查询设施信息
            AmusementFacility facility = amusementFacilityMapper.selectById(recommRoute.getFacilityId());

            // 填入数据
            Swiper swiper = new Swiper();
            swiper.setName(facility.getName());
            swiper.setStartTime(facility.getStartTime());
            swiper.setCloseTime(facility.getCloseTime());

            // 随机选择一张设施图片
            Integer facilityType = 0;
            List<FacilityImage> images = facilityImageMapper.selectList(Wrappers.<FacilityImage>query().eq("facility_type", facilityType)
                    .eq("facility_id", recommRoute.getFacilityId()));

            Collections.shuffle(images);
            swiper.setImageUrl(images.get(0).getImageUrl());

            swiperList.add(swiper);
        }

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(route.getId());
        routeVO.setName(route.getName());
        routeVO.setImgurl(route.getImgUrl());
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

}




