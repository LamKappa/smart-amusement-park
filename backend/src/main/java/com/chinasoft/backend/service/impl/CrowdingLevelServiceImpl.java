package com.chinasoft.backend.service.impl;

import cn.hutool.core.collection.CollectionUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import com.chinasoft.backend.model.vo.statistic.CrowingTimeCountVO;
import com.chinasoft.backend.model.vo.statistic.CrowingTimeItemVO;
import com.chinasoft.backend.service.facility.CrowdingLevelService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.util.CollectionUtils;

import java.text.SimpleDateFormat;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * @author 孟祥硕
 * @description 针对表【crowding_level(拥挤度表)】的数据库操作Service实现
 * @createDate 2024-04-07 16:11:50
 */
@Service
public class CrowdingLevelServiceImpl extends ServiceImpl<CrowdingLevelMapper, CrowdingLevel>
        implements CrowdingLevelService {

    @Autowired
    CrowdingLevelMapper crowdingLevelMapper;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Autowired
    AmusementFacilityServiceImpl amusementFacilityService;

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

    /**
     * 以小时为单位统计当日的拥挤度（预期等待时间）
     */
    @Override
    public List<CrowingTimeCountVO> crowingTimeCount() {
        /**
         * 设施列表
         */
        List<AmusementFacility> facilityList = amusementFacilityService.list();

        List<CrowingTimeCountVO> crowingTimeCountVOList = new ArrayList<>();


        LocalDate todayDate = LocalDate.now();

        for (AmusementFacility facility : facilityList) {
            // 该设施的设施信息和时间拥挤度列表
            CrowingTimeCountVO crowingTimeCountVO = new CrowingTimeCountVO();
            crowingTimeCountVO.setFacilityId(facility.getId());
            crowingTimeCountVO.setFacilityName(facility.getName());
            Date startTime = facility.getStartTime();
            Date closeTime = facility.getCloseTime();
            // 时间拥挤度列表
            List<CrowingTimeItemVO> crowingTimeItemVOList = new ArrayList<>();
            crowingTimeCountVO.setCrowingTimeItemVOList(crowingTimeItemVOList);

            // 查询开始时间和结束时间
            LocalTime localStartTime = LocalTime.of(startTime.getHours(), startTime.getMinutes());
            LocalDateTime startDateTime = LocalDateTime.of(todayDate, localStartTime);
            LocalTime localCloseTime = LocalTime.of(closeTime.getHours(), closeTime.getMinutes());
            LocalDateTime closeDateTime = LocalDateTime.of(todayDate, localCloseTime);

            // 查询该设施今天这个时间段的所有的拥挤度信息
            QueryWrapper<CrowdingLevel> queryWrapper = new QueryWrapper<>();
            queryWrapper.eq("facility_id", facility.getId());
            queryWrapper.ge("create_time", startDateTime);
            queryWrapper.le("create_time", closeDateTime);
            List<CrowdingLevel> crowdingLevelList = crowdingLevelMapper.selectList(queryWrapper);


            // 进行根据时间分组
            if (CollectionUtil.isNotEmpty(crowdingLevelList)) {
                for (int i = startTime.getHours(); i <= closeTime.getHours(); i++) {
                    // 使用Stream流和时间表达式来进行根据时间分组
                    Map<String, List<CrowdingLevel>> groupMap = crowdingLevelList.stream()
                            .collect(Collectors.groupingBy(item -> new SimpleDateFormat("yyyy-MM-dd HH")
                                    .format(item.getCreateTime())));
                    List<CrowdingLevel> crowdingLevelGroupList = new ArrayList<>();
                    // 按小时进行分类
                    for (String s : groupMap.keySet()) {
                        int groupHour = Integer.parseInt(s.substring(s.length() - 2, s.length()));
                        if (groupHour == i) {
                            crowdingLevelGroupList.addAll(groupMap.get(s));
                            // 获得该时间段的预计等待时间
                            Integer avgExpectWaitTime = getAvgExpectWaitTime(crowdingLevelGroupList);
                            // 向该设施的时间拥挤度列表中添加数据
                            CrowingTimeItemVO crowingTimeItemVO = new CrowingTimeItemVO();
                            crowingTimeItemVO.setTime(groupHour);
                            crowingTimeItemVO.setExpectWaitTime(avgExpectWaitTime);
                            crowingTimeItemVOList.add(crowingTimeItemVO);
                        }
                    }
                }
            }

            crowingTimeCountVOList.add(crowingTimeCountVO);
        }

        return crowingTimeCountVOList;
    }

    /**
     * 获取某个时间段的平均预计等待时间
     */
    private Integer getAvgExpectWaitTime(List<CrowdingLevel> crowdingLevelList) {
        Integer count = 0;
        Integer sum = 0;
        for (CrowdingLevel crowdingLevel : crowdingLevelList) {
            count++;
            sum += crowdingLevel.getExpectWaitTime();
        }
        return sum / count;
    }
}




