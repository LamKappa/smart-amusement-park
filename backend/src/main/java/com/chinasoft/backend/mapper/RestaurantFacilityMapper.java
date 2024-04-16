package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.facility.RestaurantFacility;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 姜堂蕴之
 * @description 针对表【restaurant_facility(餐饮设施表)】的数据库操作Mapper
 * @createDate 2024-04-05 09:53:39
 * @Entity com.chinasoft.backend.model.entity.facility.RestaurantFacility
 */
@Mapper
public interface RestaurantFacilityMapper extends BaseMapper<RestaurantFacility> {

}




