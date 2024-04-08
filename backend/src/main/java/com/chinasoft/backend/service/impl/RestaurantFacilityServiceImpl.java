package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.entity.RestaurantFacility;
import com.chinasoft.backend.model.request.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.RestaurantFacilityVO;
import com.chinasoft.backend.service.RestaurantFacilityService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * @author 皎皎
 * @description 针对表【restaurant_facility(餐饮设施表)】的数据库操作Service实现
 * @createDate 2024-04-05 09:53:39
 */
@Service
public class RestaurantFacilityServiceImpl extends ServiceImpl<RestaurantFacilityMapper, RestaurantFacility>
        implements RestaurantFacilityService {

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Override
    public List<RestaurantFacilityVO> getRestaurantFacility(RestaurantFilterRequest restaurantFilterRequest) {
        QueryWrapper<RestaurantFacility> queryWrapper = new QueryWrapper<>();

        // 检查name是否非空
        if (restaurantFilterRequest.getName() != null && !restaurantFilterRequest.getName().isEmpty()) {
            queryWrapper.eq("name", restaurantFilterRequest.getName());
        }

        // 检查type是否非空
        if (restaurantFilterRequest.getType() != null && !restaurantFilterRequest.getType().isEmpty()) {
            queryWrapper.like("type", restaurantFilterRequest.getType());
        }

        // 搜索图片
        List<RestaurantFacility> facilities = this.baseMapper.selectList(queryWrapper);

        List<RestaurantFacilityVO> facilityVOList = new ArrayList<>();

        for (RestaurantFacility facility : facilities) {

            RestaurantFacilityVO facilityVO = new RestaurantFacilityVO();

            Integer facilityType = 1;

            // 将facility的信息复制到VO对象
            BeanUtils.copyProperties(facility, facilityVO);

            // 创建 QueryWrapper 实例
            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();

            // 设置查询条件
            queryWrapper2.eq("facility_type", FacilityTypeConstant.RESTAURANT_TYPE)
                    .eq("facility_id", facility.getId());

            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);

            // 提取 image_url 列表
            List<String> imageUrls = facilityImages.stream()
                    .map(FacilityImage::getImageUrl)
                    .collect(Collectors.toList());

            // 将imageUrls放入VO对象
            facilityVO.setImageUrls(imageUrls);

            // 将VO对象加入列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }
}




