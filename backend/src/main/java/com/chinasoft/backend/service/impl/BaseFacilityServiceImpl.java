package com.chinasoft.backend.service.impl;

import cn.hutool.core.collection.CollectionUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.BaseFacilityMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.request.BaseFacilityAddRequest;
import com.chinasoft.backend.model.request.BaseFacilityUpdateRequest;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.request.FacilityIdType;
import com.chinasoft.backend.model.vo.BaseFacilityVO;
import com.chinasoft.backend.service.BaseFacilityService;
import com.chinasoft.backend.service.CrowdingLevelService;
import com.chinasoft.backend.service.FacilityImageService;
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
 * @author 皎皎
 * @description 针对表【base_facility(基础设施表)】的数据库操作Service实现
 * @createDate 2024-04-05 09:51:44
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


    @Override
    public List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest) {
        QueryWrapper<BaseFacility> queryWrapper = new QueryWrapper<>();

        // 检查id是否非空
        if (baseFilterRequest.getId() != null) {
            queryWrapper.eq("id", baseFilterRequest.getId());
        }

        // 检查name是否非空
        if (baseFilterRequest.getName() != null && !baseFilterRequest.getName().isEmpty()) {
            queryWrapper.like("name", baseFilterRequest.getName());
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

            // 查询预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(new FacilityIdType(facility.getId(), FacilityTypeConstant.BASE_TYPE));
            facilityVO.setExpectWaitTime(expectWaitTime);

            // 将VO对象加入列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }

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




