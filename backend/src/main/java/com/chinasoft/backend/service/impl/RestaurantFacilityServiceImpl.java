package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.service.RestaurantFacilityService;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import org.springframework.stereotype.Service;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【restaurant_facility(餐饮设施表)】的数据库操作Service实现
* @createDate 2024-04-05 09:53:39
*/
@Service
public class RestaurantFacilityServiceImpl extends ServiceImpl<RestaurantFacilityMapper, RestaurantFacility>
    implements RestaurantFacilityService{

    @Override
    public List<RestaurantFacility> getRestaurantName(String name) {
        QueryWrapper<RestaurantFacility> queryWrapper = new QueryWrapper<>();

        // 检查name是否为空或空字符串
        if (name == null || name.isEmpty()) {
            // 如果为空，则返回所有数据，不添加查询条件
            return this.baseMapper.selectList(queryWrapper);
        } else {
            // 如果name非空，则添加查询条件
            queryWrapper.eq("name", name);
            return this.baseMapper.selectList(queryWrapper);
        }
    }

    @Override
    public List<RestaurantFacility> getRestaurantType(String type) {
        QueryWrapper<RestaurantFacility> queryWrapper = new QueryWrapper<>();

        // 检查type是否为空或空字符串
        if (type == null || type.isEmpty()) {
            // 如果为空，则返回所有数据，不添加查询条件
            return this.baseMapper.selectList(queryWrapper);
        } else {
            // 如果type非空，则添加查询条件
            queryWrapper.eq("type", type);
            return this.baseMapper.selectList(queryWrapper);
        }
    }
}




