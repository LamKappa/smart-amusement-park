package com.chinasoft.backend;

import com.chinasoft.backend.mapper.VisitMapper;
import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.service.*;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.data.redis.core.RedisTemplate;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
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
            // 使用Stream流和时间表达式来优雅地进行根据时间分组
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
}
