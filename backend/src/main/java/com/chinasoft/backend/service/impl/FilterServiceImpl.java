package com.chinasoft.backend.service.impl;

import cn.hutool.core.bean.BeanUtil;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.SubscribeMapper;
import com.chinasoft.backend.mapper.VisitMapper;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.request.facility.FacilityFilterRequest;
import com.chinasoft.backend.model.vo.facility.*;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import com.chinasoft.backend.service.facility.FilterService;
import com.chinasoft.backend.service.facility.RestaurantFacilityService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 设施筛选Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class FilterServiceImpl implements FilterService {

    @Autowired
    private VisitMapper visitMapper;

    @Autowired
    private SubscribeMapper subscribeMapper;

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private RestaurantFacilityService restaurantFacilityService;

    @Autowired
    private BaseFacilityService baseFacilityService;


    /**
     * 根据游乐设施筛选条件，获取游乐设施的信息列表。
     *
     * @param amusementFilterRequest 包含筛选条件的游乐设施请求对象
     * @return 游乐设施信息列表
     */
    @Override
    public List<AmusementFacilityInfoVO> getAmusementFacilityInfo(AmusementFilterRequest amusementFilterRequest) {
        // 获取用户 ID
        Long userId = amusementFilterRequest.getUserId();

        // 根据筛选条件获取设施基础信息列表
        List<AmusementFacilityInfoVO> amusementFacilityInfoVOList = new ArrayList<>();
        List<AmusementFacilityVO> amusementFacilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);

        // 将设施基础信息复制到设施信息视图中
        for (AmusementFacilityVO amusementFacility : amusementFacilities) {
            AmusementFacilityInfoVO amusementVisitSubscribeVO = new AmusementFacilityInfoVO();
            BeanUtil.copyProperties(amusementFacility, amusementVisitSubscribeVO);
            amusementFacilityInfoVOList.add(amusementVisitSubscribeVO);
        }

        // 获取用户的打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));

        // 根据打卡信息更新设施信息视图中的打卡状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.AMUSEMENT_TYPE) {
                for (AmusementFacilityInfoVO amusementFacilityInfoVO : amusementFacilityInfoVOList) {
                    if (amusementFacilityInfoVO.getId().equals(visit.getFacilityId())) {
                        amusementFacilityInfoVO.setIsVisited(1);
                        amusementFacilityInfoVO.setVisitId(visit.getId());
                        amusementFacilityInfoVO.setVisitTime(visit.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 获取用户的订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 根据订阅信息更新设施信息视图中的订阅状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == FacilityTypeConstant.AMUSEMENT_TYPE) {
                for (AmusementFacilityInfoVO amusementFacilityInfoVO : amusementFacilityInfoVOList) {
                    if (amusementFacilityInfoVO.getId().equals(subscribe.getFacilityId())) {
                        amusementFacilityInfoVO.setIsSubscribed(1);
                        amusementFacilityInfoVO.setSubscribeId(subscribe.getId());
                        amusementFacilityInfoVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡、未订阅的设施状态由null统一为0
        for (AmusementFacilityInfoVO amusementFacilityInfoVO : amusementFacilityInfoVOList) {
            if (amusementFacilityInfoVO.getIsVisited() == null) {
                amusementFacilityInfoVO.setIsVisited(0);
            }
            if (amusementFacilityInfoVO.getIsSubscribed() == null) {
                amusementFacilityInfoVO.setIsSubscribed(0);
            }
        }

        // 返回设施信息视图列表
        return amusementFacilityInfoVOList;
    }

    /**
     * 根据餐饮设施筛选条件，获取餐厅设施的信息列表。
     *
     * @param restaurantFilterRequest 包含筛选条件的餐饮设施请求对象
     * @return 餐饮设施信息列表
     */
    @Override
    public List<RestaurantFacilityInfoVO> getRestaurantFacilityInfo(RestaurantFilterRequest restaurantFilterRequest) {
        // 获取用户 ID
        Long userId = restaurantFilterRequest.getUserId();

        // 根据筛选条件获取设施基础信息列表
        List<RestaurantFacilityInfoVO> restaurantFacilityInfoVOList = new ArrayList<>();
        List<RestaurantFacilityVO> restaurantFacilities = restaurantFacilityService.getRestaurantFacility(restaurantFilterRequest);

        // 将设施基础信息复制到设施信息视图中
        for (RestaurantFacilityVO restaurantFacility : restaurantFacilities) {
            RestaurantFacilityInfoVO restaurantFacilityInfoVO = new RestaurantFacilityInfoVO();
            BeanUtil.copyProperties(restaurantFacility, restaurantFacilityInfoVO);
            restaurantFacilityInfoVOList.add(restaurantFacilityInfoVO);
        }

        // 获取用户的打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 根据打卡信息更新设施信息视图中的打卡状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.RESTAURANT_TYPE) {
                for (RestaurantFacilityInfoVO restaurantFacilityInfoVO : restaurantFacilityInfoVOList) {
                    if (restaurantFacilityInfoVO.getId().equals(visit.getFacilityId())) {
                        restaurantFacilityInfoVO.setIsVisited(1);
                        restaurantFacilityInfoVO.setVisitId(visit.getId());
                        restaurantFacilityInfoVO.setVisitTime(visit.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 获取用户的订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 根据订阅信息更新设施信息视图中的订阅状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == FacilityTypeConstant.RESTAURANT_TYPE) {
                for (RestaurantFacilityInfoVO restaurantFacilityInfoVO : restaurantFacilityInfoVOList) {
                    if (restaurantFacilityInfoVO.getId().equals(subscribe.getFacilityId())) {
                        restaurantFacilityInfoVO.setIsSubscribed(1);
                        restaurantFacilityInfoVO.setSubscribeId(subscribe.getId());
                        restaurantFacilityInfoVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡、未订阅的设施状态由null统一为0
        for (RestaurantFacilityInfoVO restaurantFacilityInfoVO : restaurantFacilityInfoVOList) {
            if (restaurantFacilityInfoVO.getIsVisited() == null) {
                restaurantFacilityInfoVO.setIsVisited(0);
            }
            if (restaurantFacilityInfoVO.getIsSubscribed() == null) {
                restaurantFacilityInfoVO.setIsSubscribed(0);
            }
        }

        // 返回设施信息视图列表
        return restaurantFacilityInfoVOList;
    }

    /**
     * 根据基础设施筛选条件，获取基础设施的信息列表。
     *
     * @param baseFilterRequest 包含筛选条件的基础设施请求对象
     * @return 基础设施信息列表
     */
    public List<BaseFacilityInfoVO> getBaseFacilityInfo(BaseFilterRequest baseFilterRequest) {
        // 获取用户 ID
        Long userId = baseFilterRequest.getUserId();

        // 根据筛选条件获取设施基础信息列表
        List<BaseFacilityInfoVO> baseFacilityInfoVOList = new ArrayList<>();
        List<BaseFacilityVO> baseFacilities = baseFacilityService.getBaseFacility(baseFilterRequest);

        // 将设施基础信息复制到设施信息视图中
        for (BaseFacilityVO baseFacility : baseFacilities) {
            BaseFacilityInfoVO baseFacilityInfoVO = new BaseFacilityInfoVO();
            BeanUtil.copyProperties(baseFacility, baseFacilityInfoVO);
            baseFacilityInfoVOList.add(baseFacilityInfoVO);
        }

        // 获取用户的打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 根据打卡信息更新设施信息视图中的打卡状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.BASE_TYPE) {
                for (BaseFacilityInfoVO baseFacilityInfoVO : baseFacilityInfoVOList) {
                    if (baseFacilityInfoVO.getId().equals(visit.getFacilityId())) {
                        baseFacilityInfoVO.setIsVisited(1);
                        baseFacilityInfoVO.setVisitId(visit.getId());
                        baseFacilityInfoVO.setVisitTime(visit.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 获取用户的订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 根据订阅信息更新设施信息视图中的订阅状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == FacilityTypeConstant.BASE_TYPE) {
                for (BaseFacilityInfoVO baseVisitSubscribeVO : baseFacilityInfoVOList) {
                    if (baseVisitSubscribeVO.getId().equals(subscribe.getFacilityId())) {
                        baseVisitSubscribeVO.setIsSubscribed(1);
                        baseVisitSubscribeVO.setSubscribeId(subscribe.getId());
                        baseVisitSubscribeVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡、未订阅的设施状态由null统一为0
        for (BaseFacilityInfoVO baseVisitSubscribeVO : baseFacilityInfoVOList) {
            if (baseVisitSubscribeVO.getIsVisited() == null) {
                baseVisitSubscribeVO.setIsVisited(0);
            }
            if (baseVisitSubscribeVO.getIsSubscribed() == null) {
                baseVisitSubscribeVO.setIsSubscribed(0);
            }
        }

        // 返回设施信息视图列表
        return baseFacilityInfoVOList;
    }

    /**
     * 获取所有类型的设施信息列表，根据传入的请求条件进行筛选。
     *
     * @param facilityFilterRequest 包含筛选条件的通用请求对象
     * @return 包含各类设施信息的对象列表
     */
    @Override
    public List<Object> getAllFacilityInfo(FacilityFilterRequest facilityFilterRequest) {
        // 获取用户ID
        Long userId = facilityFilterRequest.getUserId();

        // 调用游乐设施筛选方法
        AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
        amusementFilterRequest.setUserId(userId);
        List<AmusementFacilityInfoVO> amusementFacilityInfoVOList = getAmusementFacilityInfo(amusementFilterRequest);

        // 调用餐饮设施筛选方法
        RestaurantFilterRequest restaurantFilterRequest = new RestaurantFilterRequest();
        restaurantFilterRequest.setUserId(userId);
        List<RestaurantFacilityInfoVO> restaurantFacilityInfoVOList = getRestaurantFacilityInfo(restaurantFilterRequest);

        // 调用基础设施筛选方法
        BaseFilterRequest baseFilterRequest = new BaseFilterRequest();
        baseFilterRequest.setUserId(userId);
        List<BaseFacilityInfoVO> baseFacilityInfoVOList = getBaseFacilityInfo(baseFilterRequest);

        // 将游乐设施、餐饮设施、基础设施进行合并
        List<Object> data = new ArrayList<>();
        data.addAll(amusementFacilityInfoVOList);
        data.addAll(restaurantFacilityInfoVOList);
        data.addAll(baseFacilityInfoVOList);

        // 将所有设施按照拥挤度从低到高排序
        List<Object> sortedData = data.stream()
                .sorted(Comparator.comparingInt(o -> {
                    if (o instanceof AmusementFacilityInfoVO) {
                        return ((AmusementFacilityInfoVO) o).getExpectWaitTime();
                    } else if (o instanceof RestaurantFacilityInfoVO) {
                        return ((RestaurantFacilityInfoVO) o).getExpectWaitTime();
                    } else if (o instanceof BaseFacilityInfoVO) {
                        return ((BaseFacilityInfoVO) o).getExpectWaitTime();
                    } else {
                        throw new IllegalArgumentException("Unknown type in the list: " + o.getClass().getName());
                    }
                }))
                .collect(Collectors.toList());

        return sortedData;
    }
}




