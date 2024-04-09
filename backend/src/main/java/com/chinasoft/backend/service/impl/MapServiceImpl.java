package com.chinasoft.backend.service.impl;

import cn.hutool.http.HttpUtil;
import cn.hutool.json.JSONObject;
import cn.hutool.json.JSONUtil;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.config.AmapProperties;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
import com.chinasoft.backend.model.vo.NavVO;
import com.chinasoft.backend.model.vo.PositionPoint;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.MapService;
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
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

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
        List<FacilityIdType> facilityIdTypeList = navigationRequest.getFacilities();

        // 获得所有设施id
        List<Integer> amuseFacilityIdList = new ArrayList<>();
        for (FacilityIdType facilityIdType : facilityIdTypeList) {
            if (facilityIdType.getFacilityType() == FacilityTypeConstant.AMUSEMENT_TYPE) {
                amuseFacilityIdList.add(facilityIdType.getFacilityId());
            }
        }

        // 总的预计等待时间
        Integer totalExpectWaitTime = 0;

        // 根据设施id查询所有设施的经纬度和预期等待时间
        List<AmusementFacilityVO> facilityVOList = new ArrayList<>();
        for (Integer facilityId : amuseFacilityIdList) {
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);
            List<AmusementFacilityVO> amusementFacility = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            facilityVOList.add(amusementFacility.get(0));
            totalExpectWaitTime += amusementFacility.get(0).getExpectWaitTime();
        }

        // 通过预计等待时间进行从小到大排序
        Collections.sort(facilityVOList, (a, b) -> {
            return a.getExpectWaitTime() - b.getExpectWaitTime();
        });

        // 计算出最终的路径坐标点
        List<PositionPoint> resPositionPointList = new ArrayList<>();

        PositionPoint currPositionPoint = new PositionPoint(userLongitude, userLatitude);

        // 总的预期走路时间
        Integer totalExpectWalkTime = 0;

        // 调用高德API
        for (AmusementFacilityVO amusementFacilityVO : facilityVOList) {
            PositionPoint targetPositionPoint = new PositionPoint(amusementFacilityVO.getLongitude(), amusementFacilityVO.getLatitude());
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
        Integer facilityId = eeNavigationRequest.getFacilityId();
        Integer facilityType = eeNavigationRequest.getFacilityType();


        // 获取设施位置
        String facilityLongitude = null;
        String facilityLatitude = null;

        // 预期等待时间
        Integer expectWaitTime = 0;
        // 预期走路时间
        Integer expectWalkTime = 0;

        if (facilityType == 0) {
            // AmusementFacility facility = amusementFacilityMapper.selectOne(Wrappers.<AmusementFacility>query().eq("id", facilityId));
            // 复用Service方法，根据id获取预期等待时间以及设施的经纬度
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);
            List<AmusementFacilityVO> amusementFacility = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            AmusementFacilityVO facility = amusementFacility.get(0);
            expectWaitTime = facility.getExpectWaitTime();
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == 1) {
            RestaurantFacility facility = restaurantFacilityMapper.selectOne(Wrappers.<RestaurantFacility>query().eq("id", facilityId));
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == 2) {
            BaseFacility facility = baseFacilityMapper.selectOne(Wrappers.<BaseFacility>query().eq("id", facilityId));
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
