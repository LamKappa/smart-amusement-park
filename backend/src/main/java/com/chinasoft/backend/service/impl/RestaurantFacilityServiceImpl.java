package com.chinasoft.backend.service.impl;

import cn.hutool.core.collection.CollectionUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.constant.RestaurantFacilityTypeEnum;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.mapper.RestaurantFacilityMapper;
import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import com.chinasoft.backend.model.entity.facility.FacilityImage;
import com.chinasoft.backend.model.entity.facility.RestaurantFacility;
import com.chinasoft.backend.model.request.facility.RestaurantFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.facility.RestaurantFacilityVO;
import com.chinasoft.backend.service.facility.CrowdingLevelService;
import com.chinasoft.backend.service.facility.FacilityImageService;
import com.chinasoft.backend.service.facility.RestaurantFacilityService;
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
 * @author 姜堂蕴之
 * @description 针对表【restaurant_facility(餐饮设施表)】的数据库操作Service实现
 * @createDate 2024-04-05 09:53:39
 */
@Service
public class RestaurantFacilityServiceImpl extends ServiceImpl<RestaurantFacilityMapper, RestaurantFacility>
        implements RestaurantFacilityService {

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Autowired
    FacilityImageService facilityImageService;

    private static final String LONGITUDE_REG_EXPRESS = "^(([1-9]\\d?)|(1[0-7]\\d))(\\.\\d{1,6})|180|0(\\.\\d{1,6})?$";
    private static final String LATITUDE_REG_EXPRESS = "^(([1-8]\\d?)|([1-8]\\d))(\\.\\d{1,6})|90|0(\\.\\d{1,6})?$";


    @Override
    public List<RestaurantFacilityVO> getRestaurantFacility(RestaurantFilterRequest restaurantFilterRequest) {
        QueryWrapper<RestaurantFacility> queryWrapper = new QueryWrapper<>();

        // 检查id是否非空
        if (restaurantFilterRequest.getId() != null) {
            queryWrapper.eq("id", restaurantFilterRequest.getId());
        }

        // 检查name是否非空
        if (restaurantFilterRequest.getName() != null && !restaurantFilterRequest.getName().isEmpty()) {
            queryWrapper.like("name", restaurantFilterRequest.getName());
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

            // 查询预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(new FacilityIdType(facility.getId(), FacilityTypeConstant.RESTAURANT_TYPE));
            facilityVO.setExpectWaitTime(expectWaitTime);

            // 将VO对象加入列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }

    @Override
    public long add(RestaurantFacilityAddRequest restaurantFacilityAddRequest) {

        RestaurantFacility restaurantFacility = new RestaurantFacility();
        BeanUtils.copyProperties(restaurantFacilityAddRequest, restaurantFacility);

        // 校验
        validParams(restaurantFacility, true);

        if (restaurantFacility.getStatus() == null) {
            restaurantFacility.setStatus(0);
        }
        boolean result = this.save(restaurantFacility);
        if (!result) {
            throw new BusinessException(ErrorCode.OPERATION_ERROR, "添加失败");
        }
        long newFacilityId = restaurantFacility.getId();

        List<String> imageUrls = restaurantFacilityAddRequest.getImageUrls();
        if (CollectionUtil.isNotEmpty(imageUrls)) {
            List<FacilityImage> imageList = new ArrayList<>();
            for (String url : imageUrls) {
                FacilityImage image = new FacilityImage();
                image.setFacilityId(newFacilityId);
                image.setImageUrl(url);
                image.setFacilityType(FacilityTypeConstant.RESTAURANT_TYPE);
                imageList.add(image);
            }
            facilityImageService.saveBatch(imageList);
        }

        return newFacilityId;
    }

    @Override
    public Boolean update(RestaurantFacilityUpdateRequest restaurantFacilityUpdateRequest) {

        RestaurantFacility restaurantFacility = new RestaurantFacility();
        BeanUtils.copyProperties(restaurantFacilityUpdateRequest, restaurantFacility);

        // 判断是否存在
        Long facilityId = restaurantFacility.getId();
        RestaurantFacility oldFacility = this.getById(facilityId);
        if (oldFacility == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "修改设施不存在");
        }

        // 校验
        validParams(restaurantFacility, false);
        // 更新
        boolean result = this.updateById(restaurantFacility);

        // 更新图片
        List<String> imageUrls = restaurantFacilityUpdateRequest.getImageUrls();
        if (CollectionUtil.isNotEmpty(imageUrls)) {
            // 先批量删除
            QueryWrapper<FacilityImage> deleteWrapper = new QueryWrapper<>();
            deleteWrapper.eq("facility_id", facilityId)
                    .eq("facility_type", FacilityTypeConstant.AMUSEMENT_TYPE);
            facilityImageService.remove(deleteWrapper);

            // 再批量添加图片
            List<FacilityImage> imageList = new ArrayList<>();
            for (String url : imageUrls) {
                FacilityImage image = new FacilityImage();
                image.setFacilityId(facilityId);
                image.setImageUrl(url);
                image.setFacilityType(FacilityTypeConstant.AMUSEMENT_TYPE);
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
    public void validParams(RestaurantFacility restaurantFacility, boolean add) {
        String name = restaurantFacility.getName();
        String introduction = restaurantFacility.getIntroduction();
        String longitude = restaurantFacility.getLongitude();
        String latitude = restaurantFacility.getLatitude();
        String type = restaurantFacility.getType();
        Time startTime = restaurantFacility.getStartTime();
        Time closeTime = restaurantFacility.getCloseTime();
        Integer expectTime = restaurantFacility.getExpectTime();
        Integer maxCapacity = restaurantFacility.getMaxCapacity();


        if (restaurantFacility == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 创建时，所有参数必须非空
        if (add) {
            if (StringUtils.isAnyBlank(name, introduction, longitude, latitude, type) ||
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

        // type校验
        if (type != null) {
            String[] split = type.split("/");
            for (String s : split) {
                if (!RestaurantFacilityTypeEnum.existValidate(s)) {
                    throw new BusinessException(ErrorCode.PARAMS_ERROR, "设施类型填写错误");
                }
            }
        }

    }
}




