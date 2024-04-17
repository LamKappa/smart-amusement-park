package com.chinasoft.backend.service.impl;

import cn.hutool.http.HttpUtil;
import cn.hutool.json.JSONObject;
import cn.hutool.json.JSONUtil;
import com.chinasoft.backend.config.AmapProperties;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Walk;
import com.chinasoft.backend.model.entity.facility.*;
import com.chinasoft.backend.model.request.map.MultipleNavigationRequest;
import com.chinasoft.backend.model.request.map.SingleNavigationRequest;
import com.chinasoft.backend.model.vo.NavVO;
import com.chinasoft.backend.model.vo.PositionPoint;
import com.chinasoft.backend.service.MapService;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import com.chinasoft.backend.service.facility.CrowdingLevelService;
import com.chinasoft.backend.service.facility.RestaurantFacilityService;
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
    private CrowdingLevelService crowdingLevelService;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private RestaurantFacilityService restaurantFacilityService;

    @Autowired
    private BaseFacilityService baseFacilityService;

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
     *
     * @author 孟祥硕
     */
    @Override
    public NavVO mulFacilityNav(MultipleNavigationRequest navigationRequest) {
        String userLongitude = navigationRequest.getUserLongitude();
        String userLatitude = navigationRequest.getUserLatitude();

        // 所有设施的id和type列表
        List<FacilityIdType> facilityIdTypeList = navigationRequest.getFacilities();

        // 总的预计等待时间
        Integer totalExpectWaitTime = 0;

        // 需要所有导航的设施的id和姓名，以及经纬度和预期等待时间
        List<Facility> facilityInfoList = new ArrayList<>();

        // 当前用户位置
        PositionPoint currPositionPoint = new PositionPoint(userLongitude, userLatitude);


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
            facilityInfo.setFacilityType(facilityType);

            String longitude = "";
            String latitude = "";
            String name = "";
            // 获取设施的经纬度信息
            if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
                AmusementFacility facility = amusementFacilityService.getById(facilityId);
                longitude = facility.getLongitude();
                latitude = facility.getLatitude();
                name = facility.getName();
            } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
                RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
                longitude = facility.getLongitude();
                latitude = facility.getLatitude();
                name = facility.getName();
            } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
                BaseFacility facility = baseFacilityService.getById(facilityId);
                longitude = facility.getLongitude();
                latitude = facility.getLatitude();
                name = facility.getName();
            }
            facilityInfo.setLongitude(longitude);
            facilityInfo.setLatitude(latitude);
            facilityInfo.setName(name);

            // 获取预计行走时间
            Walk walkInfo = getTwoPointExpectWalkInfo(currPositionPoint, new PositionPoint(longitude, latitude));
            facilityInfo.setExpectWalkTime(walkInfo.getExpectWalkTime());

            facilityInfoList.add(facilityInfo);
        }


        // 通过预计行走时间和预计等待时间进行从小到大排序
        Collections.sort(facilityInfoList, (a, b) -> {
            return (a.getExpectWaitTime() + a.getExpectWalkTime()) - (b.getExpectWaitTime() + b.getExpectWalkTime());
        });

        // 计算出最终的路径坐标点
        List<PositionPoint> resPositionPointList = new ArrayList<>();


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
     * 单个设施最优路径导航
     *
     * @param singleNavigationRequest 包含导航请求的参数的请求体对象
     * @return 包含最优路径导航信息的BaseResponse对象
     * @throws BusinessException 当请求体为null时，抛出参数错误异常
     * @author 姜堂蕴之
     */
    @Override
    public NavVO sinFacilityNav(SingleNavigationRequest singleNavigationRequest) {
        // 获取请求参数
        String userLongitude = singleNavigationRequest.getUserLongitude();
        String userLatitude = singleNavigationRequest.getUserLatitude();
        Long facilityId = singleNavigationRequest.getFacilityId();
        Integer facilityType = singleNavigationRequest.getFacilityType();

        // 构建设施ID和类型对象
        FacilityIdType facilityIdType = new FacilityIdType(facilityId, facilityType);

        // 初始化设施经纬度和预期等待时间
        String facilityLongitude = null;
        String facilityLatitude = null;
        Integer expectWaitTime = 0;

        // 获取设施的预期等待时间
        expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(facilityIdType);

        // 获取设施位置信息
        if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
            // 获取游乐设施的经纬度
            AmusementFacility facility = amusementFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
            // 获取餐饮设施的经纬度
            RestaurantFacility facility = restaurantFacilityService.getById(facilityId);
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
            // 获取基础设施的经纬度
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

        // 格式化预计到达时间
        LocalDateTime expectArriveTime = walkInfo.getExpectArriveTime();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("HH:mm");
        String formattedExpectArriveTime = expectArriveTime.toLocalTime().format(formatter);

        // 封装导航信息到NavVO对象
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
     *
     * @author 姜堂蕴之
     */
    private List<PositionPoint> getTwoPointNav(PositionPoint sourcePoint, PositionPoint targetPoint) {
        // 提取起点和终点的经纬度信息
        String sourceLongitude = sourcePoint.getLongitude();
        String sourceLatitude = sourcePoint.getLatitude();
        String targetLongitude = targetPoint.getLongitude();
        String targetLatitude = targetPoint.getLatitude();

        // 创建参数Map，用于构建高德地图API请求
        HashMap<String, Object> paramMap = new HashMap<>();
        paramMap.put("origin", sourceLongitude + "," + sourceLatitude);  // 设置起点坐标
        paramMap.put("destination", targetLongitude + "," + targetLatitude); // 设置终点坐标
        paramMap.put("key", amapProperties.getKey()); // 设置高德地图API密钥

        // 调用HttpUtil的get方法发起请求，获取高德地图API返回的导航结果
        String result = HttpUtil.get(WALKING_NAV_REQUEST_URL, paramMap);

        // 解析高德地图API返回的JSON数据
        JSONObject resObj = JSONUtil.parseObj(result);

        // 提取导航步骤信息
        List<Map> steps = (List<Map>) resObj.getByPath("route.paths.0.steps");

        // 创建用于存储导航路径点的列表
        List<PositionPoint> dataList = new ArrayList<>();

        // 遍历导航步骤中的每个点
        for (Map map : steps) {
            // 获取步骤中的折线点字符串
            String polyline = (String) map.get("polyline");

            // 将折线点字符串按分号分割成多个点
            String[] points = polyline.split(";");

            // 遍历每个点
            for (String point : points) {
                // 将点按逗号分割成经纬度
                String[] position = point.split(",");
                String longitude = position[0];
                String latitude = position[1];

                // 创建位置点对象
                PositionPoint positionPoint = new PositionPoint();
                positionPoint.setLatitude(latitude);
                positionPoint.setLongitude(longitude);

                // 将位置点添加到列表中
                dataList.add(positionPoint);
            }
        }

        // 返回导航路径点列表
        return dataList;
    }


    /**
     * 获得两个点之间的预期行走信息
     *
     * @author 姜堂蕴之
     */
    private Walk getTwoPointExpectWalkInfo(PositionPoint sourcePoint, PositionPoint targetPoint) {
        // 提取起点和终点的经纬度信息
        String sourceLongitude = sourcePoint.getLongitude();
        String sourceLatitude = sourcePoint.getLatitude();
        String targetLongitude = targetPoint.getLongitude();
        String targetLatitude = targetPoint.getLatitude();

        // 创建参数Map，用于构建高德地图API请求
        HashMap<String, Object> paramMap = new HashMap<>();
        paramMap.put("origins", sourceLongitude + "," + sourceLatitude); // 设置起点坐标
        paramMap.put("destination", targetLongitude + "," + targetLatitude); // 设置终点坐标
        paramMap.put("key", amapProperties.getKey()); // 设置高德地图API密钥
        paramMap.put("type", 3); // 设置类型为步行

        // 初始化预计步行时间和预计步行距离
        Integer expectWalkTime = 0;
        Integer expectWalkDistance = 0;

        // 获取当前时间
        LocalDateTime currentTime = LocalDateTime.now();

        // 调用HttpUtil的get方法发起请求，获取高德地图API返回的步行距离和时间信息
        String result = HttpUtil.get(DISTANCE_REQUEST_URL, paramMap);

        // 解析高德地图API返回的JSON数据
        JSONObject resObj = JSONUtil.parseObj(result);
        List<Map> results = (List<Map>) resObj.get("results");

        // 遍历结果列表，累加步行时间和步行距离
        for (Map map : results) {
            Integer duration = Integer.parseInt((String) map.get("duration")); // 提取步行时间（单位：秒）
            Integer distance = Integer.parseInt((String) map.get("distance")); // 提取步行距离（单位：米）
            expectWalkTime += duration / 60; // 将步行时间转换为分钟并累加
            expectWalkDistance += distance; // 累加步行距离
        }

        // 计算预计到达时间（当前时间加上预计步行时间）
        LocalDateTime expectArriveTime = currentTime.plusMinutes(expectWalkTime);

        // 创建Walk对象并设置预计步行信息
        Walk walk = new Walk();
        walk.setExpectWalkTime(expectWalkTime); // 设置预计步行时间（单位：分钟）
        walk.setExpectWalkDistance(expectWalkDistance); // 设置预计步行距离（单位：米）
        walk.setExpectArriveTime(expectArriveTime); // 设置预计到达时间

        // 返回包含期望步行信息的Walk对象
        return walk;
    }
}
