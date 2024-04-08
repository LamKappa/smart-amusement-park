package com.chinasoft.backend.service.impl;

import cn.hutool.http.HttpUtil;
import cn.hutool.json.JSONArray;
import cn.hutool.json.JSONObject;
import cn.hutool.json.JSONUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.model.request.EENavigationRequest;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.PositionPoint;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.BaseFacilityService;
import com.chinasoft.backend.service.MapService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
@Slf4j
public class MapServiceImpl implements MapService {

    @Autowired
    private CrowdingLevelMapper crowdingLevelMapper;

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

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

        // 根据设施id查询经纬度和预期等待时间


        // QueryWrapper queryWrapper = new QueryWrapper<>();
        // queryWrapper.in("id", amuseFacilityIdList);

        // amusementFacilityService.getMap(queryWrapper)

        return null;
    }

    @Override
    public List<PositionPoint> getTwoPointNav(String userLongitude, String userLatitude, Integer facilityId, Integer facilityType) {

        // 获取设施位置
        String facilityLongitude = null;
        String facilityLatitude = null;

        if(facilityType == 0){
            AmusementFacility facility = amusementFacilityMapper.selectOne(Wrappers.<AmusementFacility>query().eq("id", facilityId));
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        }else if(facilityType == 1){
            RestaurantFacility facility = restaurantFacilityMapper.selectOne(Wrappers.<RestaurantFacility>query().eq("id", facilityId));
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        }else if(facilityType == 2){
            BaseFacility facility = baseFacilityMapper.selectOne(Wrappers.<BaseFacility>query().eq("id", facilityId));
            facilityLongitude = facility.getLongitude();
            facilityLatitude = facility.getLatitude();
        }

        // 调用高德地图api
        HashMap<String, Object> paramMap = new HashMap<>();
        paramMap.put("origin", userLongitude + "," + userLatitude);
        paramMap.put("destination", facilityLongitude + "," + facilityLatitude);
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
