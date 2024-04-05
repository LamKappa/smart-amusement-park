package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import org.springframework.stereotype.Service;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【amusement_facility】的数据库操作Service实现
* @createDate 2024-04-05 09:47:45
*/
@Service
public class AmusementFacilityServiceImpl extends ServiceImpl<AmusementFacilityMapper, AmusementFacility>
    implements AmusementFacilityService{

    @Override
    public List<AmusementFacility> getAmusementName(String name) {
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();

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
    public List<AmusementFacility> getAmusementType(String type) {
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();

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

    @Override
    public List<AmusementFacility> getAmusementHeight(Integer height) {
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();

        if (height == null) {
            // 如果height为null，则不添加查询条件，返回所有数据
            return this.baseMapper.selectList(queryWrapper);
        }

        queryWrapper.ge("height_low", height) // height >= height_low
                .le("height_up", height);  // height <= height_up

        // 执行查询并返回结果
        return this.baseMapper.selectList(queryWrapper);
    }

    @Override
    public List<AmusementFacility> getAmusementCrowd(String crowd) {
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();

        // 检查crowd是否为空或空字符串
        if (crowd == null || crowd.isEmpty()) {
            // 如果为空，则返回所有数据，不添加查询条件
            return this.baseMapper.selectList(queryWrapper);
        } else {
            // 如果crowd非空，则添加查询条件
            queryWrapper.eq("crowd", crowd);
            return this.baseMapper.selectList(queryWrapper);
        }
    }
}




