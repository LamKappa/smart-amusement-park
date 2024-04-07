package com.chinasoft.backend;

import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.service.*;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;

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
}
