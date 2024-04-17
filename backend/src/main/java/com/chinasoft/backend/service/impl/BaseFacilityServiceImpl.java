package com.chinasoft.backend.service.impl;

import cn.hutool.core.collection.CollectionUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.model.entity.facility.BaseFacility;
import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import com.chinasoft.backend.model.entity.facility.FacilityImage;
import com.chinasoft.backend.model.entity.facility.RestaurantFacility;
import com.chinasoft.backend.model.request.facility.BaseFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.BaseFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.vo.facility.BaseFacilityVO;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import com.chinasoft.backend.service.facility.CrowdingLevelService;
import com.chinasoft.backend.service.facility.FacilityImageService;
import org.apache.commons.lang3.ObjectUtils;
import org.apache.commons.lang3.StringUtils;
import org.springframework.beans.BeanUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.sql.Time;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

/**
 * 针对表【base_facility】的数据库操作Service实现
 *
 * @author 姜堂蕴之 孟祥硕
 */
@Service
public class BaseFacilityServiceImpl extends ServiceImpl<BaseFacilityMapper, BaseFacility>
        implements BaseFacilityService {

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Autowired
    FacilityImageService facilityImageService;

    private static final String LONGITUDE_REG_EXPRESS = "^(([1-9]\\d?)|(1[0-7]\\d))(\\.\\d{1,6})|180|0(\\.\\d{1,6})?$";
    private static final String LATITUDE_REG_EXPRESS = "^(([1-8]\\d?)|([1-8]\\d))(\\.\\d{1,6})|90|0(\\.\\d{1,6})?$";


    /**
     * 基础设施筛选
     *
     * @author 姜堂蕴之
     */
    @Override
    public List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest) {
        QueryWrapper<BaseFacility> queryWrapper = new QueryWrapper<>();

        // 加入设施id筛选条件
        if (baseFilterRequest.getId() != null) {
            queryWrapper.eq("id", baseFilterRequest.getId());
        }

        // 加入设施名称筛选条件
        if (baseFilterRequest.getName() != null && !baseFilterRequest.getName().isEmpty()) {
            queryWrapper.like("name", baseFilterRequest.getName());
        }

        // 获取设施基本信息
        List<BaseFacility> facilities = this.baseMapper.selectList(queryWrapper);

        // 创建设施信息视图，为每个设施加入设施图片和预计等待时间
        List<BaseFacilityVO> facilityVOList = new ArrayList<>();

        for (BaseFacility facility : facilities) {
            // 将设施基础信息复制进入设施信息视图
            BaseFacilityVO facilityVO = new BaseFacilityVO();
            BeanUtils.copyProperties(facility, facilityVO);

            // 查询设施图片
            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();
            queryWrapper2.eq("facility_type", FacilityTypeConstant.BASE_TYPE)
                    .eq("facility_id", facility.getId());
            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);
            List<String> imageUrls = facilityImages.stream()
                    .map(FacilityImage::getImageUrl)
                    .collect(Collectors.toList());

            // 将设施图片放入设施信息视图中
            facilityVO.setImageUrls(imageUrls);

            // 查询预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(new FacilityIdType(facility.getId(), FacilityTypeConstant.BASE_TYPE));
            facilityVO.setExpectWaitTime(expectWaitTime);

            // 将设施信息视图加入设施信息视图列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }

    /**
     * 基础设施查询
     *
     * @author 姜堂蕴之
     */
    @Override
    public List<BaseFacilityVO> searchBaseFacility(String keyword) {
        // 创建QueryWrapper对象用于构建查询条件
        QueryWrapper<BaseFacility> queryWrapper = new QueryWrapper<>();

        // 对关键字进行模糊查询
        queryWrapper.like("name", keyword);

        // 获取设施基本信息
        List<BaseFacility> facilities = this.baseMapper.selectList(queryWrapper);

        // 创建设施信息视图，为每个设施加入设施图片和预计等待时间
        List<BaseFacilityVO> facilityVOList = new ArrayList<>();

        for (BaseFacility facility : facilities) {
            // 将设施基础信息复制进入设施信息视图
            BaseFacilityVO facilityVO = new BaseFacilityVO();
            BeanUtils.copyProperties(facility, facilityVO);

            // 查询设施图片
            QueryWrapper<FacilityImage> queryWrapper2 = new QueryWrapper<>();
            queryWrapper2.eq("facility_type", FacilityTypeConstant.BASE_TYPE)
                    .eq("facility_id", facility.getId());
            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);
            List<String> imageUrls = facilityImages.stream()
                    .map(FacilityImage::getImageUrl)
                    .collect(Collectors.toList());

            // 将设施图片放入设施信息视图中
            facilityVO.setImageUrls(imageUrls);

            // 查询预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(new FacilityIdType(facility.getId(), FacilityTypeConstant.BASE_TYPE));
            facilityVO.setExpectWaitTime(expectWaitTime);

            // 将设施信息视图加入设施信息视图列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }

    /**
     * 增加
     *
     * @author 孟祥硕
     */
    @Override
    public long add(BaseFacilityAddRequest baseFacilityAddRequest) {

        BaseFacility baseFacility = new BaseFacility();
        BeanUtils.copyProperties(baseFacilityAddRequest, baseFacility);

        // 校验
        validParams(baseFacility, true);

        if (baseFacility.getStatus() == null) {
            baseFacility.setStatus(0);
        }
        boolean result = this.save(baseFacility);
        if (!result) {
            throw new BusinessException(ErrorCode.OPERATION_ERROR, "添加失败");
        }
        long newFacilityId = baseFacility.getId();

        List<String> imageUrls = baseFacilityAddRequest.getImageUrls();
        if (CollectionUtil.isNotEmpty(imageUrls)) {
            List<FacilityImage> imageList = new ArrayList<>();
            for (String url : imageUrls) {
                FacilityImage image = new FacilityImage();
                image.setFacilityId(newFacilityId);
                image.setImageUrl(url);
                image.setFacilityType(FacilityTypeConstant.BASE_TYPE);
                imageList.add(image);
            }
            facilityImageService.saveBatch(imageList);
        }

        return newFacilityId;
    }

    /**
     * 修改
     *
     * @author 孟祥硕
     */
    @Override
    public Boolean update(BaseFacilityUpdateRequest baseFacilityUpdateRequest) {

        BaseFacility baseFacility = new BaseFacility();
        BeanUtils.copyProperties(baseFacilityUpdateRequest, baseFacility);

        // 判断是否存在
        Long facilityId = baseFacility.getId();
        BaseFacility oldFacility = this.getById(facilityId);
        if (oldFacility == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "修改设施不存在");
        }

        // 校验
        validParams(baseFacility, false);
        // 更新
        boolean result = this.updateById(baseFacility);

        // 更新图片
        List<String> imageUrls = baseFacilityUpdateRequest.getImageUrls();
        if (CollectionUtil.isNotEmpty(imageUrls)) {
            // 先批量删除
            QueryWrapper<FacilityImage> deleteWrapper = new QueryWrapper<>();
            deleteWrapper.eq("facility_id", facilityId)
                    .eq("facility_type", FacilityTypeConstant.BASE_TYPE);
            facilityImageService.remove(deleteWrapper);

            // 再批量添加图片
            List<FacilityImage> imageList = new ArrayList<>();
            for (String url : imageUrls) {
                FacilityImage image = new FacilityImage();
                image.setFacilityId(facilityId);
                image.setImageUrl(url);
                image.setFacilityType(FacilityTypeConstant.BASE_TYPE);
                imageList.add(image);
            }
            facilityImageService.saveBatch(imageList);
        }

        return result;
    }

    /**
     * 参数校验
     *
     * @author 孟祥硕
     */
    @Override
    public void validParams(BaseFacility baseFacility, boolean add) {
        String name = baseFacility.getName();
        String longitude = baseFacility.getLongitude();
        String latitude = baseFacility.getLatitude();
        Time startTime = baseFacility.getStartTime();
        Time closeTime = baseFacility.getCloseTime();
        Integer expectTime = baseFacility.getExpectTime();
        Integer maxCapacity = baseFacility.getMaxCapacity();


        if (baseFacility == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 创建时，所有参数必须非空
        if (add) {
            if (StringUtils.isAnyBlank(name, longitude, latitude) ||
                    ObjectUtils.anyNull(maxCapacity, expectTime, startTime, closeTime)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
            }
        }

        // 经度校验
        if (latitude != null && longitude != null && (!Pattern.matches(LONGITUDE_REG_EXPRESS, longitude) || !Pattern.matches(LATITUDE_REG_EXPRESS, latitude))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "经纬度格式错误");
        }

        // 预计游玩时间校验
        if (expectTime != null && (expectTime <= 0 || expectTime > 100)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "预计时间错误");
        }
    }
}




