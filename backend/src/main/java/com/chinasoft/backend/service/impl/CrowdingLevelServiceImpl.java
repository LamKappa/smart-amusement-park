package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.service.CrowdingLevelService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.util.CollectionUtils;

import java.util.List;

/**
 * @author 86178
 * @description 针对表【crowding_level(拥挤度表)】的数据库操作Service实现
 * @createDate 2024-04-07 16:11:50
 */
@Service
public class CrowdingLevelServiceImpl extends ServiceImpl<CrowdingLevelMapper, CrowdingLevel>
        implements CrowdingLevelService {

    @Autowired
    CrowdingLevelMapper crowdingLevelMapper;

    @Override
    public Integer getExpectWaitTimeByIdType(FacilityIdType facilityIdType) {
        Long facilityId = facilityIdType.getFacilityId();
        Integer facilityType = facilityIdType.getFacilityType();

        QueryWrapper<CrowdingLevel> crowdingLevelQuery = new QueryWrapper<CrowdingLevel>();
        crowdingLevelQuery.eq("facility_id", facilityId);
        crowdingLevelQuery.eq("facility_type", facilityType);
        crowdingLevelQuery.orderByDesc("create_time");
        List<CrowdingLevel> crowdingLevelList = crowdingLevelMapper.selectList(crowdingLevelQuery);
        if (!CollectionUtils.isEmpty(crowdingLevelList)) {
            return crowdingLevelList.get(0).getExpectWaitTime();
        }

        return 0;
    }
}




