package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.service.BaseFacilityService;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import org.springframework.stereotype.Service;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【base_facility(基础设施表)】的数据库操作Service实现
* @createDate 2024-04-05 09:51:44
*/
@Service
public class BaseFacilityServiceImpl extends ServiceImpl<BaseFacilityMapper, BaseFacility>
    implements BaseFacilityService{

    @Override
    public List<BaseFacility> getBaseName(String name) {
        QueryWrapper<BaseFacility> queryWrapper = new QueryWrapper<>();

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
}




