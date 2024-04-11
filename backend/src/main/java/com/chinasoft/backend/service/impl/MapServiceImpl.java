package com.chinasoft.backend.service.impl;

import cn.hutool.http.HttpUtil;
import cn.hutool.json.JSONObject;
import cn.hutool.json.JSONUtil;
import com.chinasoft.backend.config.AmapProperties;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;
import com.chinasoft.backend.model.vo.PositionPoint;
import com.chinasoft.backend.service.*;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;

@Service
@Slf4j
public class MapServiceImpl implements MapService {

    @Autowired
    private AmapProperties amapProperties;

    @Autowired
    private CrowdingLevelMapper crowdingLevelMapper;

    @Autowired
    private CrowdingLevelService crowdingLevelService;

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private RestaurantFacilityService restaurantFacilityService;

    @Autowired
    private BaseFacilityService baseFacilityService;

    @Autowired
    private RestaurantFacilityMapper restaurantFacilityMapper;

    @Autowired
    private BaseFacilityMapper baseFacilityMapper;

    /**
     * 两点之间导航的请求URL
     */
    private static final String WALKING_NAV_REQUEST_URL = "https://restapi.amap.com/v3/direction/walking";

    /**
     * 距离和预期等待时间的请求URL
     */
    private static final String DISTANCE_REQUEST_URL = "https://restapi.amap.com/v3/distance";

    /**
     * 多个设施进行最优路径导航
     */
    @Override
    public NavVO mulFacilityNav(NavigationRequest navigationRequest) {
        String userLongitude = navigationRequest.getUserLongitude();
        String userLatitude = navigationRequest.getUserLatitude();

        // 所有设施的id和type列表
        List<FacilityIdType> facilityIdTypeList = navigationRequest.getFacilities();

        // 总的预计等待时间
        Integer totalExpectWaitTime = 0;

        // 需要所有导航的设施的id和姓名，以及经纬度和预期等待时间
        List<Facility> facilityInfoList = new ArrayList<>();


        // 根据设施id和设施type查询所有设施的经纬度和预期等待时间
        for (FacilityIdType facilityIdType : facilityIdTypeList) {
            Integer facilityType = facilityIdType.getFacilityType();
            Long facilityId = facilityIdType.getFacilityId();
            // 获取预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(facilityIdType);
            totalExpectWaitTime += expectWaitTime;
            Facility facilityInfo = new Facility();
            facilityInfo.setId(facilityId);
            facilityInfo.setExpectWaitTime(expectWaitTime);
            // 获取设施的经纬度信息
            if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
                AmusementFacility facility = amusementFacilityService.getById(facilityId);
                facilityInfo.setLongitude(facility.getLongitude());
                facilityInfo.setLatitude(facility.getLatitude());
                facilityInfo.setName(facilityInfo.getName());
            } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
                RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
                facilityInfo.setLongitude(facility.getLongitude());
                facilityInfo.setLatitude(facility.getLatitude());
                facilityInfo.setName(facilityInfo.getName());
            } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
                BaseFacility facility = baseFacilityService.getById(facilityId);
                facilityInfo.setLongitude(facility.getLongitude());
                facilityInfo.setLatitude(facility.getLatitude());
                facilityInfo.setName(facilityInfo.getName());
            }
            facilityInfoList.add(facilityInfo);
        }


        // 通过预计等待时间进行从小到大排序
        Collections.sort(facilityInfoList, (a, b) -> {
            return a.getExpectWaitTime() - b.getExpectWaitTime();
        });

        // 计算出最终的路径坐标点
        List<PositionPoint> resPositionPointList = new ArrayList<>();

        PositionPoint currPositionPoint = new PositionPoint(userLongitude, userLatitude);

        // 总的预期走路时间
        Integer totalExpectWalkTime = 0;
        // 总的预期走路路程
        Integer totalExpectWalkDistance = 0;


        // 调用高德API
        for (Facility facilityInfo : facilityInfoList) {
            PositionPoint targetPositionPoint = new PositionPoint(facilityInfo.getLongitude(), facilityInfo.getLatitude());
            resPositionPointList.addAll(getTwoPointNav(currPositionPoint, targetPositionPoint));
            Walk walkInfo = getTwoPointExpectWalkInfo(currPositionPoint, targetPositionPoint);
            totalExpectWalkTime += walkInfo.getExpectWalkTime();
            totalExpectWalkDistance += walkInfo.getExpectWalkDistance();
            currPositionPoint = targetPositionPoint;
        }

        // 预计到达时间
        LocalDateTime currentTime = LocalDateTime.now();
        LocalDateTime totalExpectArriveTime = currentTime.plusMinutes(totalExpectWalkTime);
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("HH:mm");
        String formattedTotalExpectArriveTime = totalExpectArriveTime.toLocalTime().format(formatter);

        // 将数据封装称NavVO
        NavVO navVO = new NavVO();
        navVO.setPaths(resPositionPointList);
        navVO.setExpectWalkTime(totalExpectWalkTime);
        navVO.setExpectWalkDistance(totalExpectWalkDistance);
        // 多个设施导航不需要预计到达时间
        // navVO.setExpectArriveTime(formattedTotalExpectArriveTime);
        navVO.setTotalWaitTime(totalExpectWaitTime);
        navVO.setFacilities(facilityInfoList);

        return navVO;
    }


    /**
     * 单个设施进行导航
     */
    @Override
    public NavVO sinFacilityNav(EENavigationRequest eeNavigationRequest) {
        String userLongitude = eeNavigationRequest.getUserLongitude();
        String userLatitude = eeNavigationRequest.getUserLatitude();
        Long facilityId = eeNavigationRequest.getFacilityId();
        Integer facilityType = eeNavigationRequest.getFacilityType();

        FacilityIdType facilityIdType = new FacilityIdType(facilityId, facilityType);


        // 获取设施位置
        String facilityLongitude = null;
        String facilityLatitude = null;

        // 预期等待时间
        Integer expectWaitTime = 0;

        expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(facilityIdType);

        if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
            // 获取设施的经纬度
            AmusementFacility facility = amusementFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
            RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
            BaseFacility facility = baseFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        }

        // 调用API获取路径坐标点列表
        PositionPoint sourcePositionPoint = new PositionPoint(userLongitude, userLatitude);
        PositionPoint targetPositionPoint = new PositionPoint(facilityLongitude, facilityLatitude);
        List<PositionPoint> resList = getTwoPointNav(sourcePositionPoint, targetPositionPoint);

        // 获取预计行走信息
        Walk walkInfo = getTwoPointExpectWalkInfo(sourcePositionPoint, targetPositionPoint);

        LocalDateTime expectArriveTime = walkInfo.getExpectArriveTime();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("HH:mm");
        String formattedExpectArriveTime = expectArriveTime.toLocalTime().format(formatter);

        // 将数据封装称NavVO
        NavVO navVO = new NavVO();
        navVO.setTotalWaitTime(expectWaitTime);
        navVO.setExpectWalkTime(walkInfo.getExpectWalkTime());
        navVO.setExpectWalkDistance(walkInfo.getExpectWalkDistance());
        navVO.setExpectArriveTime(formattedExpectArriveTime);
        navVO.setPaths(resList);


        return navVO;
    }

    /**
     * 获得两个点之间的路径坐标点列表
     */
    private List<PositionPoint> getTwoPointNav(PositionPoint sourcePoint, PositionPoint targetPoint) {
        // 调用高德地图api
        String sourceLongitude = sourcePoint.getLongitude();
        String sourceLatitude = sourcePoint.getLatitude();
        String targetLongitude = targetPoint.getLongitude();
        String targetLatitude = targetPoint.getLatitude();
        HashMap<String, Object> paramMap = new HashMap<>();
        paramMap.put("origin", sourceLongitude + "," + sourceLatitude);
        paramMap.put("destination", targetLongitude + "," + targetLatitude);
        paramMap.put("key", amapProperties.getKey());

        String result = HttpUtil.get(WALKING_NAV_REQUEST_URL, paramMap);

        // 处理高德api数据
        JSONObject resObj = JSONUtil.parseObj(result);
        List<Map> steps = (List<Map>) resObj.getByPath("route.paths.0.steps");
        List<PositionPoint> dataList = new ArrayList<>();


        // 遍历steps
        for (Map map : steps) {
            String polyline = (String) map.get("polyline");

            String[] points = polyline.split(";");
            for (String point : points) {
                String[] position = point.split(",");
                String longitude = position[0];
                String latitude = position[1];

                PositionPoint positionPoint = new PositionPoint();
                positionPoint.setLatitude(latitude);
                positionPoint.setLongitude(longitude);

                // 将坐标点添加到列表中
                dataList.add(positionPoint);
            }
        }

        return dataList;
    }


    /**
     * 获得两个点之间的预期行走信息
     */
    private Walk getTwoPointExpectWalkInfo(PositionPoint sourcePoint, PositionPoint targetPoint) {
        // 调用高德地图api
        String sourceLongitude = sourcePoint.getLongitude();
        String sourceLatitude = sourcePoint.getLatitude();
        String targetLongitude = targetPoint.getLongitude();
        String targetLatitude = targetPoint.getLatitude();
        HashMap<String, Object> paramMap = new HashMap<>();
        paramMap.put("origins", sourceLongitude + "," + sourceLatitude);
        paramMap.put("destination", targetLongitude + "," + targetLatitude);
        paramMap.put("key", amapProperties.getKey());
        paramMap.put("type", 3);

        Integer expectWalkTime = 0;
        Integer expectWalkDistance = 0;
        LocalDateTime currentTime = LocalDateTime.now();

        String result = HttpUtil.get(DISTANCE_REQUEST_URL, paramMap);

        JSONObject resObj = JSONUtil.parseObj(result);
        List<Map> results = (List<Map>) resObj.get("results");

        for (Map map : results) {
            Integer duration = Integer.parseInt((String) map.get("duration"));
            Integer distance = Integer.parseInt((String) map.get("distance"));
            expectWalkTime += duration / 60;
            expectWalkDistance += distance;
        }

        LocalDateTime expectArriveTime = currentTime.plusMinutes(expectWalkTime);

        Walk walk = new Walk();
        walk.setExpectWalkTime(expectWalkTime);
        walk.setExpectWalkDistance(expectWalkDistance);
        walk.setExpectArriveTime(expectArriveTime);

        return walk;
    }
}
