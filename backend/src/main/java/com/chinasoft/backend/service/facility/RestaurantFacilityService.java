package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.facility.RestaurantFacility;
import com.chinasoft.backend.model.request.facility.RestaurantFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.facility.RestaurantFacilityVO;

import java.util.List;

/**
 * 针对表【restaurant_facility】的数据库操作Service
 *
 * @author 姜堂蕴之 孟祥硕
 */
public interface RestaurantFacilityService extends IService<RestaurantFacility> {
    /**
     * 餐饮设施筛选
     *
     * @author 姜堂蕴之
     */
    List<RestaurantFacilityVO> getRestaurantFacility(RestaurantFilterRequest restaurantFilterRequest);

    /**
     * 餐饮设施查询
     *
     * @author 姜堂蕴之
     */
    List<RestaurantFacilityVO> searchRestaurantFacility(String keyword);

    /**
     * 增加
     *
     * @author 孟祥硕
     */
    long add(RestaurantFacilityAddRequest restaurantFacilityAddRequest);

    /**
     * 修改
     *
     * @author 孟祥硕
     */
    Boolean update(RestaurantFacilityUpdateRequest restaurantFacilityUpdateRequest);

    /**
     * 参数校验
     *
     * @author 孟祥硕
     */
    void validParams(RestaurantFacility restaurantFacility, boolean add);


}
