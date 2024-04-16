package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.*;
import com.chinasoft.backend.model.entity.route.RecommRoute;
import com.chinasoft.backend.model.entity.route.Route;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.route.AddRouteRequest;
import com.chinasoft.backend.model.request.route.DeleteRouteRequest;
import com.chinasoft.backend.model.request.route.RouteRecommendationRequest;
import com.chinasoft.backend.model.request.route.UpdateRouteRequest;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;
import com.chinasoft.backend.service.MapService;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.route.RecommendationRouteService;
import com.chinasoft.backend.service.route.RouteService;
import org.apache.commons.lang3.ObjectUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.net.HttpURLConnection;
import java.net.URL;
import java.util.*;

/**
 * @author 姜堂蕴之
 * @description 针对轮播图的数据库操作Service实现
 * @createDate 2024-04-07 16:24:00
 */
@Service
public class RecommendationRouteServiceImpl implements RecommendationRouteService {

    private static final int MAX_ROUTE_NAME_LENGTH = 50;
    private static final int MIN_ROUTE_NAME_LENGTH = 0;
    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

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

    @Autowired
    RouteService routeService;

    @Override
    public List<RouteVO> getRecommendation(RouteRecommendationRequest routeRecommendationRequest) {
        QueryWrapper<Route> queryWrapper = new QueryWrapper<>();

        if (routeRecommendationRequest.getId() != null) {
            queryWrapper.eq("id", routeRecommendationRequest.getId());
        }

        if (routeRecommendationRequest.getName() != null) {
            queryWrapper.like("name", routeRecommendationRequest.getName());
        }

        List<RouteVO> routeVOList = new ArrayList<>();
        // 查询推荐路线信息
        List<Route> routes = routeMapper.selectList(queryWrapper);

        for (Route route : routes) {
            List<RecommRoute> recommRoutes = recommRouteMapper.selectList(Wrappers.<RecommRoute>query()
                    .eq("route_id", route.getId())
                    .orderByAsc("priority"));
            List<AmusementFacilityVO> swiperList = new ArrayList<>();
            for (RecommRoute recommRoute : recommRoutes) {
                // 根据facility_id查询设施信息
                AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
                amusementFilterRequest.setId(recommRoute.getFacilityId());

                List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
                AmusementFacilityVO facility = facilities.get(0);

                swiperList.add(facility);
            }
            RouteVO routeVO = new RouteVO();
            routeVO.setId(route.getId());
            routeVO.setName(route.getName());
            routeVO.setImgUrl(route.getImgUrl());
            routeVO.setSwiperList(swiperList);

            routeVOList.add(routeVO);

        }
        return routeVOList;
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

    @Override
    public List<RouteVO> addRoute(AddRouteRequest addRouteRequest) {
        String name = addRouteRequest.getName();
        String imgUrl = addRouteRequest.getImgUrl();
        List<Integer> facilityIdList = addRouteRequest.getFacilityIdList();

        if (ObjectUtils.anyNull(name, imgUrl, facilityIdList)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 校验路线名称有效性
        if (name.length() > MAX_ROUTE_NAME_LENGTH || name.length() < MIN_ROUTE_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "路线名称长度不在允许范围内");
        }

        // 判断url格式是否合法
        if (imgUrl != null) {
            if (!(imgUrl.matches("^(http|https)://.*$"))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL格式不正确");
            }
        }


        // 发起HTTP请求检查url是否可访问
        try {
            URL obj = new URL(imgUrl);
            HttpURLConnection con = (HttpURLConnection) obj.openConnection();
            con.setRequestMethod("GET");
            int responseCode = con.getResponseCode();
            if (responseCode != HttpURLConnection.HTTP_OK) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
            }
        } catch (Exception e) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
        }

        // 校验途径设施列表中的设施是否存在  
        for (Integer facilityId : facilityIdList) {
            if (amusementFacilityMapper.selectById(facilityId) == null) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "添加设施不存在");
            }
        }

        // 校验插入数据是否重复
        List<RouteVO> allRouteVOList = getRecommendation(new RouteRecommendationRequest());

        for (RouteVO existingRouteVO : allRouteVOList) {
            String existingName = existingRouteVO.getName();
            String existingImgUrl = existingRouteVO.getImgUrl();
            List<AmusementFacilityVO> existingSwiperList = existingRouteVO.getSwiperList();
            List<Integer> existingFacilityIdList = new ArrayList<>();

            for (AmusementFacilityVO amusementFacilityVO : existingSwiperList) {
                existingFacilityIdList.add(Math.toIntExact(amusementFacilityVO.getId()));
            }

            if (name.equals(existingName)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "名称重复");
            }

            if (imgUrl.equals(existingImgUrl)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "图片重复");
            }

            if (facilityIdList.equals(existingFacilityIdList)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "途径设施重复");
            }
        }

        Route route = new Route();
        route.setName(name);
        route.setImgUrl(imgUrl);
        routeMapper.insert(route);

        Long routeId = route.getId();

        for (Integer facilityId : facilityIdList) {
            RecommRoute recommRoute = new RecommRoute();
            recommRoute.setRouteId(routeId);
            recommRoute.setFacilityId(Long.valueOf(facilityId));
            recommRoute.setPriority(facilityIdList.indexOf(facilityId) + 1);
            recommRouteMapper.insert(recommRoute);
        }

        RouteRecommendationRequest routeRecommendationRequest = new RouteRecommendationRequest();
        routeRecommendationRequest.setId(routeId);

        List<RouteVO> routeVOList = getRecommendation(routeRecommendationRequest);

        return routeVOList;

    }

    @Override
    public Boolean deleteRoute(DeleteRouteRequest deleteRouteRequest) {
        Long routeId = deleteRouteRequest.getId();

        if (routeId == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        Route route = routeService.getById(routeId);

        // 校验待删除的路线是否存在
        if (route == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待删除的路线不存在");
        }

        QueryWrapper<RecommRoute> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("route_id", routeId);

        int deletedCount = recommRouteMapper.delete(queryWrapper);

        Boolean res = routeService.removeById(routeId);

        if (deletedCount > 0 && res) {
            return true;
        } else {
            return false;
        }

    }

    @Override
    public List<RouteVO> updateRoute(UpdateRouteRequest updateRouteRequest) {

        Long routeId = updateRouteRequest.getId();
        String name = updateRouteRequest.getName();
        String imgUrl = updateRouteRequest.getImgUrl();
        List<Integer> facilityIdList = updateRouteRequest.getFacilityIdList();


        if (ObjectUtils.anyNull(routeId, name, imgUrl, facilityIdList)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        Route originalRoute = routeService.getById(routeId);

        // 校验待更新的路线是否存在
        if (originalRoute == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待更新的路线不存在");
        }

        // 校验路线名称有效性
        if (name.length() > MAX_ROUTE_NAME_LENGTH || name.length() < MIN_ROUTE_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "路线名称长度不在允许范围内");
        }

        // 判断url格式是否合法
        if (!(imgUrl.matches("^(http|https)://.*$"))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL格式不正确");
        }

        // 发起HTTP请求检查url是否可访问
        try {
            URL obj = new URL(imgUrl);
            HttpURLConnection con = (HttpURLConnection) obj.openConnection();
            con.setRequestMethod("GET");
            int responseCode = con.getResponseCode();
            if (responseCode != HttpURLConnection.HTTP_OK) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
            }
        } catch (Exception e) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
        }

        // 校验途径设施列表中的设施是否存在
        for (Integer facilityId : facilityIdList) {
            if (amusementFacilityMapper.selectById(facilityId) == null) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "添加设施不存在");
            }
        }

        // 校验插入数据是否重复
        List<RouteVO> allRouteVOList = getRecommendation(new RouteRecommendationRequest());

        for (RouteVO existingRouteVO : allRouteVOList) {
            String existingName = existingRouteVO.getName();
            String existingImgUrl = existingRouteVO.getImgUrl();
            List<AmusementFacilityVO> existingSwiperList = existingRouteVO.getSwiperList();
            List<Integer> existingFacilityIdList = new ArrayList<>();

            for (AmusementFacilityVO amusementFacilityVO : existingSwiperList) {
                existingFacilityIdList.add(Math.toIntExact(amusementFacilityVO.getId()));
            }

            if (name.equals(existingName) && !(name.equals(originalRoute.getName()))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "名称重复");
            }

            if (imgUrl.equals(existingImgUrl) && !(imgUrl.equals(originalRoute.getImgUrl()))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "图片重复");
            }

            if (facilityIdList.equals(existingFacilityIdList) && !(facilityIdList.equals(updateRouteRequest.getFacilityIdList()))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "途径设施重复");
            }
        }

        Route updateRoute = new Route();
        updateRoute.setId(routeId);
        updateRoute.setName(name);
        updateRoute.setImgUrl(imgUrl);
        routeMapper.updateById(updateRoute);

        // 删除原有的路线途径设施记录
        QueryWrapper<RecommRoute> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("route_id", routeId);
        recommRouteMapper.delete(queryWrapper);


        // 增加现在的路线途径设施记录
        for (Integer facilityId : facilityIdList) {
            RecommRoute recommRoute = new RecommRoute();
            recommRoute.setRouteId(routeId);
            recommRoute.setFacilityId(Long.valueOf(facilityId));
            recommRoute.setPriority(facilityIdList.indexOf(facilityId) + 1);
            recommRouteMapper.insert(recommRoute);
        }

        RouteRecommendationRequest routeRecommendationRequest = new RouteRecommendationRequest();
        routeRecommendationRequest.setId(routeId);

        List<RouteVO> routeVOList = getRecommendation(routeRecommendationRequest);

        return routeVOList;
    }

}



