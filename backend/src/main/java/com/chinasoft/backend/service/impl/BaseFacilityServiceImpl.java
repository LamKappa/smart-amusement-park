package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.vo.BaseFacilityVO;
import com.chinasoft.backend.service.BaseFacilityService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * @author 皎皎
 * @description 针对表【base_facility(基础设施表)】的数据库操作Service实现
 * @createDate 2024-04-05 09:51:44
 */
@Service
public class BaseFacilityServiceImpl extends ServiceImpl<BaseFacilityMapper, BaseFacility>
        implements BaseFacilityService {

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Override
    public List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest) {
        QueryWrapper<BaseFacility> queryWrapper = new QueryWrapper<>();

        // 检查name是否非空
        if (baseFilterRequest.getName() != null && !baseFilterRequest.getName().isEmpty()) {
            queryWrapper.eq("name", baseFilterRequest.getName());
        }

        // 搜索图片
        List<BaseFacility> facilities = this.baseMapper.selectList(queryWrapper);

        List<BaseFacilityVO> facilityVOList = new ArrayList<>();

        for (BaseFacility facility : facilities) {

            BaseFacilityVO facilityVO = new BaseFacilityVO();

            Integer facilityType = 2;

            // 将facility的信息复制到VO对象
            BeanUtils.copyProperties(facility, facilityVO);

            // 创建 QueryWrapper 实例
            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();

            // 设置查询条件
            queryWrapper2.eq("facility_type", FacilityTypeConstant.BASE_TYPE)
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




