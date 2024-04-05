package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.baomidou.mybatisplus.extension.service.IService;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【restaurant_facility(餐饮设施表)】的数据库操作Service
* @createDate 2024-04-05 09:53:39
*/
public interface RestaurantFacilityService extends IService<RestaurantFacility> {

    List<RestaurantFacility> getRestaurantName(String name);

    List<RestaurantFacility> getRestaurantType(String type);
}
