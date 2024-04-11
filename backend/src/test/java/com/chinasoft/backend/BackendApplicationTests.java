package com.chinasoft.backend;

import cn.hutool.core.collection.CollectionUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.VisitMapper;
import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.model.vo.CrowingTimeCountVO;
import com.chinasoft.backend.service.*;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.data.redis.core.RedisTemplate;

import java.text.SimpleDateFormat;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.*;
import java.util.stream.Collectors;

@SpringBootTest
class BackendApplicationTests {

    @Autowired
    UserService userService;

    @Autowired
    AmusementFacilityService amusementFacilityService;

    @Autowired
    BaseFacilityService baseFacilityService;

    @Autowired
    RestaurantFacilityService restaurantFacilityService;

    @Autowired
    FacilityImageService facilityImageService;

    @Autowired
    RedisTemplate redisTemplate;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Autowired
    RouteService routeService;

    @Test
    void contextLoads() {

    }

    @Test
    void testUser() {
        System.out.println(userService.list());
    }

    @Test
    void testAmusementFacility() {
        List<AmusementFacility> list = amusementFacilityService.list();
        System.out.println(list);

    }

    @Test
    void testBaseFacility() {
        List<BaseFacility> list = baseFacilityService.list();
        System.out.println(list);
    }

    @Test
    void testRestaurantFacility() {
        List<RestaurantFacility> list = restaurantFacilityService.list();
        System.out.println(list);
    }

    @Test
    void testFacilityImage() {
        List<FacilityImage> list = facilityImageService.list();
        System.out.println(list);
    }

    @Test
    void testRedis() {
        HashMap<String, IoTData> map = new HashMap<>();
        List<IoTData> list = new ArrayList<>();

        IoTData ioTData = new IoTData();
        ioTData.setDeviceId(1);
        ioTData.setFacilityId(2);
        ioTData.setFacilityType(3);
        ioTData.setDetection(4);
        list.add(ioTData);

        map.put("hello", ioTData);

        // HashOperations hashOperations = redisTemplate.opsForHash();
        // hashOperations.putAll("map", map);
        // System.out.println(hashOperations.get("map", "hello"));
        // ListOperations valueOperations = redisTemplate.opsForHash();
        // valueOperations
        // System.out.println(valueOperations.get("hello"));
    }

    @Test
    void testMySQLTime() {
        CrowdingLevel crowdingLevel = new CrowdingLevel();
        crowdingLevel.setFacilityId(2L);
        crowdingLevel.setFacilityType(0);
        crowdingLevel.setExpectWaitTime(20);

        crowdingLevelService.save(crowdingLevel);

    }


    @Test
    public void testTimeStream() {
        List<CrowdingLevel> list = crowdingLevelService.list();

        for (int i = 23; i >= 0; i--) {
            // 使用Stream流和时间表达式来进行根据时间分组
            Map<String, List<CrowdingLevel>> groupMap = list.stream().collect(Collectors.groupingBy(item -> new SimpleDateFormat("yyyy-MM-dd HH").format(item.getCreateTime())));
            List<CrowdingLevel> crowdingLevelList = new ArrayList<>();
            for (String s : groupMap.keySet()) {
                if (Integer.parseInt(s.substring(s.length() - 1, s.length())) == i) {
                    crowdingLevelList.addAll(groupMap.get(s));
                }
            }
            System.out.println(crowdingLevelList);
        }
    }

    @Test
    public void testRoute() {
        System.out.println(routeService.list());
    }

    @Autowired
    VisitService visitService;

    @Autowired
    VisitMapper visitMapper;

    @Test
    public void testVisitCount() {
        // QueryWrapper queryWrapper = new QueryWrapper();
        // queryWrapper.groupBy("facility_id");
        // long count = visitService.count(queryWrapper);
        Map<Long, Map<String, Long>> integerIntegerMap = visitMapper.visitCount();
        System.out.println(integerIntegerMap);
    }

    @Autowired
    CrowdingLevelMapper crowdingLevelMapper;

    @Test
    public void testCrowingTimeCount() {
        List<AmusementFacility> facilityList = amusementFacilityService.list();


        List<CrowingTimeCountVO> crowingTimeCountVOList = new ArrayList<>();

        LocalDate todayDate = LocalDate.now();

        for (AmusementFacility facility : facilityList) {
            CrowingTimeCountVO crowingTimeCountVO = new CrowingTimeCountVO();
            crowingTimeCountVO.setFacilityId(facility.getId());
            crowingTimeCountVO.setFacilityName(facility.getName());
            Date startTime = facility.getStartTime();
            Date closeTime = facility.getCloseTime();
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
            // System.out.println("crowdingLevelList = " + crowdingLevelList);

            int a = 1;
            // List<CrowdingLevel> list = crowdingLevelService.list();

            if (CollectionUtil.isNotEmpty(crowdingLevelList)) {
                for (int i = closeTime.getHours(); i >= startTime.getHours(); i--) {
                    // 使用Stream流和时间表达式来进行根据时间分组
                    Map<String, List<CrowdingLevel>> groupMap = crowdingLevelList.stream()
                            .collect(Collectors.groupingBy(item -> new SimpleDateFormat("yyyy-MM-dd HH")
                                    .format(item.getCreateTime())));
                    List<CrowdingLevel> crowdingLevelGroupList = new ArrayList<>();
                    for (String s : groupMap.keySet()) {
                        Integer groupHour = Integer.parseInt(s.substring(s.length() - 2, s.length()));
                        if (groupHour == i) {
                            crowdingLevelGroupList.addAll(groupMap.get(s));
                        }
                    }
                    if (CollectionUtil.isNotEmpty(crowdingLevelGroupList)) {
                        System.out.println("crowdingLevelGroupList = " + crowdingLevelGroupList);
                    }
                }
            }

        }
    }

    @Test
    public void testVisit() {
        List<Visit> list = visitService.list();
        System.out.println(list);
    }
}
