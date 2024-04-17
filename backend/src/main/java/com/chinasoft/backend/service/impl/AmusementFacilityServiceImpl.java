package com.chinasoft.backend.service.impl;

import cn.hutool.core.collection.CollectionUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.constant.AmusementFacilityTypeEnum;
import com.chinasoft.backend.constant.CrowdTypeEnum;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.FacilityImageMapper;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import com.chinasoft.backend.model.entity.facility.FacilityImage;
import com.chinasoft.backend.model.request.facility.AmusementFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.AmusementFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
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
 * @author 姜堂蕴之
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

    @Autowired
    CrowdingLevelMapper crowdingLevelMapper;

    @Autowired
    FacilityImageService facilityImageService;

    private static final String LONGITUDE_REG_EXPRESS = "^(([1-9]\\d?)|(1[0-7]\\d))(\\.\\d{1,6})|180|0(\\.\\d{1,6})?$";
    private static final String LATITUDE_REG_EXPRESS = "^(([1-8]\\d?)|([1-8]\\d))(\\.\\d{1,6})|90|0(\\.\\d{1,6})?$";


    @Override
    public List<AmusementFacilityVO> getAmusementFacility(AmusementFilterRequest amusementFilterRequest) {
        QueryWrapper<AmusementFacility> queryWrapper = new QueryWrapper<>();

        if (amusementFilterRequest.getId() != null) {
            queryWrapper.eq("id", amusementFilterRequest.getId());
        }

        // 检查name是否非空
        if (amusementFilterRequest.getName() != null && !amusementFilterRequest.getName().isEmpty()) {
            queryWrapper.like("name", amusementFilterRequest.getName());
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
            queryWrapper.like("crowd_type", amusementFilterRequest.getCrowd());
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
            queryWrapper2.eq("facility_type", FacilityTypeConstant.AMUSEMENT_TYPE)
                    .eq("facility_id", facility.getId());

            List<FacilityImage> facilityImages = facilityImageMapper.selectList(queryWrapper2);

            // 提取 image_url 列表
            List<String> imageUrls = facilityImages.stream()
                    .map(FacilityImage::getImageUrl)
                    .collect(Collectors.toList());

            // 将imageUrls放入VO对象
            facilityVO.setImageUrls(imageUrls);

            // 查询预计等待时间
            Integer expectWaitTime = crowdingLevelService.getExpectWaitTimeByIdType(new FacilityIdType(facility.getId(), FacilityTypeConstant.AMUSEMENT_TYPE));
            if (expectWaitTime != 0) {
                facilityVO.setExpectWaitTime(expectWaitTime);
            } else {
                // 默认值
                facilityVO.setExpectWaitTime(facilityVO.getExpectTime());
            }
            // 将VO对象加入列表
            facilityVOList.add(facilityVO);
        }

        return facilityVOList;
    }

    /**
     * 增加
     */
    @Override
    public long add(AmusementFacilityAddRequest amusementFacilityAddRequest) {

        // 添加默认值
        if (amusementFacilityAddRequest.getHeightLow() == null) {
            amusementFacilityAddRequest.setHeightLow(0);
        }
        if (amusementFacilityAddRequest.getHeightUp() == null) {
            amusementFacilityAddRequest.setHeightUp(300);
        }


        AmusementFacility amusementFacility = new AmusementFacility();
        BeanUtils.copyProperties(amusementFacilityAddRequest, amusementFacility);


        // 校验
        validParams(amusementFacility, true);

        if (amusementFacility.getStatus() == null) {
            amusementFacility.setStatus(0);
        }
        boolean result = this.save(amusementFacility);
        if (!result) {
            throw new BusinessException(ErrorCode.OPERATION_ERROR, "添加失败");
        }
        long newFacilityId = amusementFacility.getId();

        List<String> imageUrls = amusementFacilityAddRequest.getImageUrls();
        if (CollectionUtil.isNotEmpty(imageUrls)) {
            List<FacilityImage> imageList = new ArrayList<>();
            for (String url : imageUrls) {
                FacilityImage image = new FacilityImage();
                image.setFacilityId(newFacilityId);
                image.setImageUrl(url);
                image.setFacilityType(FacilityTypeConstant.AMUSEMENT_TYPE);
                imageList.add(image);
            }
            facilityImageService.saveBatch(imageList);
        }
        StringBuffer text = new StringBuffer();
        // 超过120个字符的情况下，换行缩进4个空格，点号和方法名称一起换行



        return newFacilityId;
    }


    /**
     * 修改
     */
    @Override
    public Boolean update(AmusementFacilityUpdateRequest amusementFacilityUpdateRequest) {

        AmusementFacility amusementFacility = new AmusementFacility();
        BeanUtils.copyProperties(amusementFacilityUpdateRequest, amusementFacility);

        // 判断是否存在
        Long facilityId = amusementFacility.getId();
        AmusementFacility oldFacility = this.getById(facilityId);
        if (oldFacility == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "修改设施不存在");
        }

        // 校验
        validParams(amusementFacility, false);
        // 更新
        boolean result = this.updateById(amusementFacility);

        // 更新图片
        List<String> imageUrls = amusementFacilityUpdateRequest.getImageUrls();
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
    public void validParams(AmusementFacility amusementFacility, boolean add) {

        String name = amusementFacility.getName();
        String introduction = amusementFacility.getIntroduction();
        String longitude = amusementFacility.getLongitude();
        String latitude = amusementFacility.getLatitude();
        Integer perUserCount = amusementFacility.getPerUserCount();
        Integer expectTime = amusementFacility.getExpectTime();
        String type = amusementFacility.getType();
        String crowdType = amusementFacility.getCrowdType();
        Time startTime = amusementFacility.getStartTime();
        Time closeTime = amusementFacility.getCloseTime();
        String instruction = amusementFacility.getInstruction();
        Integer heightLow = amusementFacility.getHeightLow();
        Integer heightUp = amusementFacility.getHeightUp();

        if (amusementFacility == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 创建时，所有参数必须非空
        if (add) {
            if (StringUtils.isAnyBlank(name, introduction, longitude, latitude, type, crowdType, instruction) ||
                    ObjectUtils.anyNull(perUserCount, expectTime, perUserCount, heightLow, heightUp, startTime, closeTime)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
            }
        }

        // 经度校验
        if (latitude != null && longitude != null && (!Pattern.matches(LONGITUDE_REG_EXPRESS, longitude) || !Pattern.matches(LATITUDE_REG_EXPRESS, latitude))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "经纬度格式错误");
        }

        // 一次游玩人数和预计游玩时间校验
        if (perUserCount != null && (perUserCount <= 0 || perUserCount > 200)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "一次游玩人数错误");
        }
        if (expectTime != null && (expectTime <= 0 || expectTime > 100)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "预计游玩时间错误");
        }

        // type校验
        if (type != null) {
            String[] split = type.split("/");
            for (String s : split) {
                if (!AmusementFacilityTypeEnum.existValidate(s)) {
                    throw new BusinessException(ErrorCode.PARAMS_ERROR, "设施类型填写错误");
                }
            }
        }


        // crowingType校验
        if (crowdType != null) {
            String[] split = crowdType.split("/");
            for (String s : split) {
                if (!CrowdTypeEnum.existValidate(s)) {
                    throw new BusinessException(ErrorCode.PARAMS_ERROR, "适合人群填写错误");
                }
            }
        }

        // 身高校验
        if (heightLow != null && heightUp != null) {
            if (heightLow < 0 || heightUp > 300 || heightLow >= heightUp) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "要求身高填写错误");
            }
        }
    }
}