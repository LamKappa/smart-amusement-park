package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.mapper.*;
import com.chinasoft.backend.model.entity.RecommRoute;
import com.chinasoft.backend.model.entity.Route;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.MapService;
import com.chinasoft.backend.service.RecommService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.*;

/**
 * @author 皎皎
 * @description 针对轮播图的数据库操作Service实现
 * @createDate 2024-04-07 16:24:00
 */
@Service
public class RecommServiceImpl implements RecommService {

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Autowired
    VisitMapper visitMapper;

    @Autowired
    SubscribeMapper subscribeMapper;

    @Autowired
    RouteMapper routeMapper;

    @Autowired
    RecommRouteMapper recommRouteMapper;

    @Autowired
    MapService mapService;

    @Override
    public RouteVO getRecommendation(Integer routeId) {

        // 查询推荐路线信息
        Route route = routeMapper.selectById(routeId);

        List<RecommRoute> recommRoutes = recommRouteMapper.selectList(Wrappers.<RecommRoute>query()
                .eq("route_id", routeId)
                .orderByDesc("priority"));

        List<AmusementFacilityVO> swiperList = new ArrayList<>();
        for (RecommRoute recommRoute : recommRoutes) {
            // 根据facility_id查询设施信息
//            AmusementFacility facility = amusementFacilityMapper.selectById(recommRoute.getFacilityId());
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(recommRoute.getFacilityId());

            List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            AmusementFacilityVO facility = facilities.get(0);

            swiperList.add(facility);


//            // 填入数据
//            Swiper swiper = new Swiper();
//            swiper.setName(facility.getName());
//            swiper.setStartTime(facility.getStartTime());
//            swiper.setCloseTime(facility.getCloseTime());

            // 随机选择一张设施图片
//            Integer facilityType = 0;
//            List<FacilityImage> images = facilityImageMapper.selectList(Wrappers.<FacilityImage>query().eq("facility_type", facilityType)
//                    .eq("facility_id", recommRoute.getFacilityId()));
//
//            Collections.shuffle(images);
//            swiper.setImageUrl(images.get(0).getImageUrl());
//
//            swiperList.add(swiper);
        }

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(route.getId());
        routeVO.setName(route.getName());
        routeVO.setImgUrl(route.getImgUrl());
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

    @Override
    public RouteVO sortByVisit() {
        // 使用自定义的SQL查询前4个设施
        List<Map> topFourFacilities = visitMapper.getTopFourFacilitiesByVisitCount();

        List<AmusementFacilityVO> swiperList = new ArrayList<>();

        for (Map facility : topFourFacilities) {
            Set keys = facility.keySet(); // 获取当前Map中的所有键，此处键数为1
            Long facilityId = null;
            for (Object key : keys) {
                facilityId = (Long) facility.get(key);
            }

            // 获取设施信息
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);

            List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            AmusementFacilityVO amusementFacilityVO = facilities.get(0);

            swiperList.add(amusementFacilityVO);
        }

        // 选取第一个设施的第一个图片作为封面
        String imgUrl = swiperList.get(0).getImageUrls().get(0);

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(null);
        routeVO.setName("热门打卡路线");
        routeVO.setImgUrl(imgUrl);
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

    @Override
    public RouteVO sortBySubscribe() {
        // 使用自定义的SQL查询前4个设施
        List<Map> topFourFacilities = subscribeMapper.getTopFourFacilitiesBySubscribeCount();

        List<AmusementFacilityVO> swiperList = new ArrayList<>();

        for (Map facility : topFourFacilities) {
            Set keys = facility.keySet(); // 获取当前Map中的所有键，此处键数为1
            Long facilityId = null;
            for (Object key : keys) {
                facilityId = (Long) facility.get(key);
            }

            // 获取设施信息
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);

            List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            AmusementFacilityVO amusementFacilityVO = facilities.get(0);

            swiperList.add(amusementFacilityVO);
        }

        // 选取第一个设施的第一个图片作为封面
        String imgUrl = swiperList.get(0).getImageUrls().get(0);

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(null);
        routeVO.setName("人气订阅路线");
        routeVO.setImgUrl(imgUrl);
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

    /**
     * 返回由订拥挤度排行的四个设施组成的游玩路线
     */
    @Override
    public RouteVO sortCrowingLevel() {

        List<AmusementFacilityVO> swiperList = new ArrayList<>();
        // 获取所有设施
        AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
        List<AmusementFacilityVO> allFacilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);

        // 从小到大进行排序
        Collections.sort(allFacilities, (a, b) -> {
            return a.getExpectWaitTime() - b.getExpectWaitTime();
        });

        // 取前四个
        for (int i = 0; i < 4; i++) {
            swiperList.add(allFacilities.get(i));
        }

        // 选取第一个设施的第一个图片作为封面
        String imgUrl = swiperList.get(0).getImageUrls().get(0);

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(null);
        routeVO.setName("畅通无阻路线");
        routeVO.setImgUrl(imgUrl);
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }


}




