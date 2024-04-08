package com.chinasoft.backend.service.impl;

import cn.hutool.http.HttpUtil;
import cn.hutool.json.JSONObject;
import cn.hutool.json.JSONUtil;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
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
    private CrowdingLevelMapper crowdingLevelMapper;

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private RestaurantFacilityMapper restaurantFacilityMapper;

    @Autowired
    private BaseFacilityMapper baseFacilityMapper;

    @Override
    public List<PositionPoint> mulFacilityNav(NavigationRequest navigationRequest) {
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

        // 根据设施id查询所有设施的经纬度和预期等待时间

        List<AmusementFacilityVO> facilityVOList = new ArrayList<>();
        for (Integer facilityId : amuseFacilityIdList) {
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);
            List<AmusementFacilityVO> amusementFacility = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            facilityVOList.add(amusementFacility.get(0));
        }

        // 通过预计等待时间进行从小到大排序
        Collections.sort(facilityVOList, (a, b) -> {
            return a.getExpectWaitTime() - b.getExpectWaitTime();
        });

        // 计算出最终的路径坐标点
        List<PositionPoint> resPositionPointList = new ArrayList<>();

        PositionPoint currPositionPoint = new PositionPoint(userLongitude, userLatitude);

        for (AmusementFacilityVO amusementFacilityVO : facilityVOList) {
            PositionPoint targetPositionPoint = new PositionPoint(amusementFacilityVO.getLongitude(), amusementFacilityVO.getLatitude());
            resPositionPointList.addAll(getTwoPointNav(currPositionPoint, targetPositionPoint));
            currPositionPoint = targetPositionPoint;
        }

        return resPositionPointList;
    }

    @Override
    public List<PositionPoint> sinFacilityNav(EENavigationRequest eeNavigationRequest) {
        String userLongitude = eeNavigationRequest.getUserLongitude();
        String userLatitude = eeNavigationRequest.getUserLatitude();
        Integer facilityId = eeNavigationRequest.getFacilityId();
        Integer facilityType = eeNavigationRequest.getFacilityType();


        // 获取设施位置
        String facilityLongitude = null;
        String facilityLatitude = null;

        if (facilityType == 0) {
            AmusementFacility facility = amusementFacilityMapper.selectOne(Wrappers.<AmusementFacility>query().eq("id", facilityId));
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

        List<PositionPoint> resList = getTwoPointNav(new PositionPoint(userLongitude, userLatitude), new PositionPoint(facilityLongitude, facilityLatitude));

        return resList;
    }

    private List<PositionPoint> getTwoPointNav(PositionPoint sourcePoint, PositionPoint targetPoint) {


        // 调用高德地图api
        String sourceLongitude = sourcePoint.getLongitude();
        String sourceLatitude = sourcePoint.getLatitude();
        String targetLongitude = targetPoint.getLongitude();
        String targetLatitude = targetPoint.getLatitude();
        HashMap<String, Object> paramMap = new HashMap<>();
        paramMap.put("origin", sourceLongitude + "," + sourceLatitude);
        paramMap.put("destination", targetLongitude + "," + targetLatitude);
        paramMap.put("key", "a78b0915a81134b501c379379e908f12");

        String result = HttpUtil.get("https://restapi.amap.com/v3/direction/walking", paramMap);

        // 处理高德api数据
        JSONObject resObj = JSONUtil.parseObj(result);
        List steps = (List) resObj.getByPath("route.paths.0.steps");
        List<PositionPoint> dataList = new ArrayList<>();

        // 遍历steps
        for (int i = 0; i < steps.size(); i++) {
            Map map = (Map) steps.get(i);
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
}
