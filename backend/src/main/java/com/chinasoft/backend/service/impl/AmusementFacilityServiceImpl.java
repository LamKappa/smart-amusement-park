package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.CrowdingLevelService;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * @author 皎皎
 * @description 针对表【amusement_facility】的数据库操作Service实现
 * @createDate 2024-04-05 09:47:45
 */
@Service
public class AmusementFacilityServiceImpl extends ServiceImpl<AmusementFacilityMapper, AmusementFacility>
        implements AmusementFacilityService {

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Override
    public List<AmusementFacilityVO> getAmusementFacility(AmusementFilterRequest amusementFilterRequest) {
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();

        // 检查name是否非空
        if (amusementFilterRequest.getName() != null && !amusementFilterRequest.getName().isEmpty()) {
            queryWrapper.eq("name", amusementFilterRequest.getName());
        }

        // 检查type是否非空
        if (amusementFilterRequest.getType() != null && !amusementFilterRequest.getType().isEmpty()) {
            queryWrapper.like("type", amusementFilterRequest.getType());
        }

        // 检查height是否非空
        if (amusementFilterRequest.getHeight() != null) {
            queryWrapper.le("height_low", amusementFilterRequest.getHeight()) // height_low <= height
                    .ge("height_up", amusementFilterRequest.getHeight()); // height_up >= height
        }

        // 检查crowd是否非空
        if (amusementFilterRequest.getCrowd() != null && !amusementFilterRequest.getCrowd().isEmpty()) {
            queryWrapper.eq("crowd", amusementFilterRequest.getCrowd());
        }

        // 搜索图片
        List<AmusementFacility> facilities = this.baseMapper.selectList(queryWrapper);

        List<AmusementFacilityVO> facilityVOList = new ArrayList<>();

        for (AmusementFacility facility : facilities) {

            AmusementFacilityVO facilityVO = new AmusementFacilityVO();

            Integer facilityType = 0;

            // 将facility的信息复制到VO对象
            BeanUtils.copyProperties(facility, facilityVO);

            // 创建 QueryWrapper 实例
            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();

            // 设置查询条件
            queryWrapper2.eq("facility_type", facilityType)
                    .eq("facility_id", facility.getId());

            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);

            // 提取 image_url 列表
            List<String> imageUrls = facilityImages.stream()
                    .map(FacilityImage::getImageUrl)
                    .collect(Collectors.toList());

            // 将imageUrls放入VO对象
            facilityVO.setImageUrls(imageUrls);

            // 查询预计等待时间
            // QueryWrapper<CrowdingLevel> crowdingQueryWrapper = new QueryWrapper<>();
            // crowdingQueryWrapper.eq("")
            CrowdingLevel crowdingLevelQuery = new CrowdingLevel();
            crowdingLevelQuery.setFacilityId(facility.getId());
            CrowdingLevel crowdingLevel = crowdingLevelService.getById(crowdingLevelQuery);
            facilityVO.setExpectWaitTime(crowdingLevel.getExpectWaitTime());
            // 将VO对象加入列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }

}




