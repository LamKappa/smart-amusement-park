package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.model.entity.Swiper;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.entity.facility.FacilityImage;
import com.chinasoft.backend.service.SwiperService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 针对轮播图的数据库操作Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class SwiperServiceImpl implements SwiperService {

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    FacilityImageMapper facilityImageMapper;


    /**
     * 获取所有游乐设施的轮播信息
     *
     * @return 包含轮播信息的BaseResponse对象
     */
    @Override
    public List<Swiper> getSwiper() {

        // 获取所有游乐设施
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();
        List<AmusementFacility> facilities = amusementFacilityMapper.selectList(queryWrapper);

        // 用于存放游乐设施轮播信息
        List<Swiper> swiperList = new ArrayList<>();

        for (AmusementFacility facility : facilities) {

            Swiper swiper = new Swiper();

            // 查询设施图片
            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();
            queryWrapper2.eq("facility_type", FacilityTypeConstant.AMUSEMENT_TYPE)
                    .eq("facility_id", facility.getId());
            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);
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




