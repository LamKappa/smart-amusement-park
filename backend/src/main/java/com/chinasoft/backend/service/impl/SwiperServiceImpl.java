package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
import com.chinasoft.backend.service.SearchService;
import com.chinasoft.backend.service.SwiperService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

/**
* @author 皎皎
* @description 针对轮播图的数据库操作Service实现
* @createDate 2024-04-07 16:24:00
*/
@Service
public class SwiperServiceImpl implements SwiperService {

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    FacilityImageMapper facilityImageMapper;


    @Override
    public List<Swiper> getSwiper() {

        // 获取所有游乐设施
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();
        List<AmusementFacility> facilities = amusementFacilityMapper.selectList(queryWrapper);

        // 获取游乐设施的名称与图片
        List<Swiper> swiperList = new ArrayList<>();

        for (AmusementFacility facility : facilities) {

            Swiper swiper = new Swiper();

            // 预设设施类型为游乐设施
            Integer facilityType = 0;

            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();

            // 设置查询条件
            queryWrapper2.eq("facility_type", facilityType)
                    .eq("facility_id", facility.getId());

            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);

            // 提取 image_url 列表
            List<String> imageUrls = facilityImages.stream()
                    .map(FacilityImage::getImageUrl)
                    .collect(Collectors.toList());

            // 在列表中随机抽取一张图片
            Collections.shuffle(imageUrls);
            String imageUrl = imageUrls.get(0);

            // 填充swiper数据
            swiper.setName(facility.getName());
            swiper.setImageUrl(imageUrl);

            // 将swiper对象加入列表
            swiperList.add(swiper);
        }

        return swiperList;
    }
}




