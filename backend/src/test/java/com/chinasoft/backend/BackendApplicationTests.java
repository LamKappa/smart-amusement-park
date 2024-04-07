package com.chinasoft.backend;

import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.service.*;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.data.redis.core.RedisTemplate;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

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
}
