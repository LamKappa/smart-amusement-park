package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.model.request.RestaurantFacilityAddRequest;
import com.chinasoft.backend.model.request.RestaurantFacilityUpdateRequest;
import com.chinasoft.backend.model.request.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.RestaurantFacilityVO;

import java.util.List;

/**
 * @author 皎皎
 * @description 针对表【restaurant_facility(餐饮设施表)】的数据库操作Service
 * @createDate 2024-04-05 09:53:39
 */
public interface RestaurantFacilityService extends IService<RestaurantFacility> {

    List<RestaurantFacilityVO> getRestaurantFacility(RestaurantFilterRequest restaurantFilterRequest);

    long add(RestaurantFacilityAddRequest restaurantFacilityAddRequest);

    Boolean update(RestaurantFacilityUpdateRequest restaurantFacilityUpdateRequest);

    void validParams(RestaurantFacility restaurantFacility, boolean add);
}
