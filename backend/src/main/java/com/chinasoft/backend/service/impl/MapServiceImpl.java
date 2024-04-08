package com.chinasoft.backend.service.impl;

import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.request.NavigationRequest;
import com.chinasoft.backend.model.vo.PositionPoint;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.MapService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

@Service
@Slf4j
public class MapServiceImpl implements MapService {

    @Autowired
    private CrowdingLevelMapper crowdingLevelMapper;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

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
}
