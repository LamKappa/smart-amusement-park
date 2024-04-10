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
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.FacilityPositionAndExpectWaitTime;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;
import com.chinasoft.backend.model.vo.PositionPoint;
import com.chinasoft.backend.service.*;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

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

        // 需要所有导航的设施的经纬度和预期等待时间
        List<FacilityPositionAndExpectWaitTime> facilityInfoList = new ArrayList<>();

        // 根据设施id和设施type查询所有设施的经纬度和预期等待时间
        for (FacilityIdType facilityIdType : facilityIdTypeList) {
            Integer facilityType = facilityIdType.getFacilityType();
            Long facilityId = facilityIdType.getFacilityId();
            // 获取预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(facilityIdType);
            totalExpectWaitTime += expectWaitTime;
            FacilityPositionAndExpectWaitTime facilityInfo = new FacilityPositionAndExpectWaitTime();
            facilityInfo.setExpectWaitTime(expectWaitTime);
            // 获取设施的经纬度信息
            if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
                AmusementFacility facility = amusementFacilityService.getById(facilityId);
                facilityInfo.setLongitude(facility.getLongitude());
                facilityInfo.setLatitude(facility.getLatitude());
            } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
                RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
                facilityInfo.setLongitude(facility.getLongitude());
                facilityInfo.setLatitude(facility.getLatitude());
            } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
                BaseFacility facility = baseFacilityService.getById(facilityId);
                facilityInfo.setLongitude(facility.getLongitude());
                facilityInfo.setLatitude(facility.getLatitude());
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

        // 调用高德API
        for (FacilityPositionAndExpectWaitTime facilityInfo : facilityInfoList) {
            PositionPoint targetPositionPoint = new PositionPoint(facilityInfo.getLongitude(), facilityInfo.getLatitude());
            resPositionPointList.addAll(getTwoPointNav(currPositionPoint, targetPositionPoint));
            Integer expectWalkTime = getTwoPointExpectWalkTime(currPositionPoint, targetPositionPoint);
            totalExpectWalkTime += expectWalkTime;
            currPositionPoint = targetPositionPoint;
        }

        // 将数据封装称NavVO
        NavVO navVO = new NavVO();
        navVO.setPaths(resPositionPointList);
        navVO.setExpectWalkTime(totalExpectWalkTime);
        navVO.setTotalWaitTime(totalExpectWaitTime);

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
        // 预期走路时间
        Integer expectWalkTime = 0;

        expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(facilityIdType);

        if (facilityType == 0) {
            // 获取设施的经纬度
            AmusementFacility facility = amusementFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == 1) {
            RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == 2) {
            BaseFacility facility = baseFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        }

        // 调用API获取路径坐标点列表
        PositionPoint sourcePositionPoint = new PositionPoint(userLongitude, userLatitude);
        PositionPoint targetPositionPoint = new PositionPoint(facilityLongitude, facilityLatitude);
        List<PositionPoint> resList = getTwoPointNav(sourcePositionPoint, targetPositionPoint);

        // 获取预计走路时间
        expectWalkTime = getTwoPointExpectWalkTime(sourcePositionPoint, targetPositionPoint);

        // 将数据封装称NavVO
        NavVO navVO = new NavVO();
        navVO.setTotalWaitTime(expectWaitTime);
        navVO.setPaths(resList);
        navVO.setExpectWalkTime(expectWalkTime);

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
     * 获得两个点之间的预期走路时间
     */
    private Integer getTwoPointExpectWalkTime(PositionPoint sourcePoint, PositionPoint targetPoint) {
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

        String result = HttpUtil.get(DISTANCE_REQUEST_URL, paramMap);

        JSONObject resObj = JSONUtil.parseObj(result);
        List<Map> results = (List<Map>) resObj.get("results");

        for (Map map : results) {
            Integer duration = Integer.parseInt((String) map.get("duration"));
            expectWalkTime += duration / 60;
        }

        return expectWalkTime;
    }


}
