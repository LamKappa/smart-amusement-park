package com.chinasoft.backend.service.impl;

import cn.hutool.core.bean.BeanUtil;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.*;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.*;
import com.chinasoft.backend.model.vo.*;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.BaseFacilityService;
import com.chinasoft.backend.service.RestaurantFacilityService;
import com.chinasoft.backend.service.VisitAndSubscribeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class VisitAndSubscribeServiceImpl implements VisitAndSubscribeService{

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

    @Override
    public List<AmusementVandSVO> getAmusementVAndS(AmusementFilterRequest amusementFilterRequest) {
        Long userId = amusementFilterRequest.getUserId();

        // 获取设施基础信息
        List<AmusementVandSVO> amusementVandSVOList = new ArrayList<>();
        List<AmusementFacilityVO> amusementFacilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
        for(AmusementFacilityVO amusementFacility : amusementFacilities){
            AmusementVandSVO amusementVandSVO = new AmusementVandSVO();
            BeanUtil.copyProperties(amusementFacility, amusementVandSVO);
            amusementVandSVOList.add(amusementVandSVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.AMUSEMENT_TYPE) {
                for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
                    if (amusementVandSVO.getId().equals(visit.getFacilityId())) {
                        amusementVandSVO.setIsVisited(1);
                        amusementVandSVO.setVisitId(visit.getId());
                        amusementVandSVO.setVisitTime(visit.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 获取订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_subscribed状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == FacilityTypeConstant.AMUSEMENT_TYPE) {
                for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
                    if (amusementVandSVO.getId().equals(subscribe.getFacilityId())) {
                        amusementVandSVO.setIsSubscribed(1);
                        amusementVandSVO.setSubscribeId(subscribe.getId());
                        amusementVandSVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡未订阅的部分由null统一为0
        for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
            if (amusementVandSVO.getIsVisited() == null) {
                amusementVandSVO.setIsVisited(0);
            }
            if (amusementVandSVO.getIsSubscribed() == null) {
                amusementVandSVO.setIsSubscribed(0);
            }
        }


        // 返回结果
        return amusementVandSVOList;

    }

    @Override
    public List<RestaurantVandSVO> getRestaurantVAndS(RestaurantFilterRequest restaurantFilterRequest) {
        Long userId = restaurantFilterRequest.getUserId();

        // 获取设施基础信息
        List<RestaurantVandSVO> restaurantVandSVOList = new ArrayList<>();
        List<RestaurantFacilityVO> restaurantFacilities = restaurantFacilityService.getRestaurantFacility(restaurantFilterRequest);
        for(RestaurantFacilityVO restaurantFacility : restaurantFacilities){
            RestaurantVandSVO restaurantVandSVO = new RestaurantVandSVO();
            BeanUtil.copyProperties(restaurantFacility, restaurantVandSVO);
            restaurantVandSVOList.add(restaurantVandSVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.RESTAURANT_TYPE) {
                for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
                    if (restaurantVandSVO.getId().equals(visit.getFacilityId())) {
                        restaurantVandSVO.setIsVisited(1);
                        restaurantVandSVO.setVisitId(visit.getId());
                        restaurantVandSVO.setVisitTime(visit.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 获取订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_subscribed状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == FacilityTypeConstant.RESTAURANT_TYPE) {
                for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
                    if (restaurantVandSVO.getId().equals(subscribe.getFacilityId())) {
                        restaurantVandSVO.setIsSubscribed(1);
                        restaurantVandSVO.setSubscribeId(subscribe.getId());
                        restaurantVandSVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡未订阅的部分由null统一为0
        for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
            if (restaurantVandSVO.getIsVisited() == null) {
                restaurantVandSVO.setIsVisited(0);
            }
            if (restaurantVandSVO.getIsSubscribed() == null) {
                restaurantVandSVO.setIsSubscribed(0);
            }
        }

        // 返回结果
        return restaurantVandSVOList;
    }

    public List<BaseVandSVO> getBaseVAndS(BaseFilterRequest baseFilterRequest) {
        Long userId = baseFilterRequest.getUserId();

        // 获取设施基础信息
        List<BaseVandSVO> baseVandSVOList = new ArrayList<>();
        List<BaseFacilityVO> baseFacilities = baseFacilityService.getBaseFacility(baseFilterRequest);
        for(BaseFacilityVO baseFacility : baseFacilities){
            BaseVandSVO baseVandSVO = new BaseVandSVO();
            BeanUtil.copyProperties(baseFacility, baseVandSVO);
            baseVandSVOList.add(baseVandSVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.BASE_TYPE) {
                for (BaseVandSVO baseVandSVO : baseVandSVOList) {
                    if (baseVandSVO.getId().equals(visit.getFacilityId())) {
                        baseVandSVO.setIsVisited(1);
                        baseVandSVO.setVisitId(visit.getId());
                        baseVandSVO.setVisitTime(visit.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 获取订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_subscribed状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == FacilityTypeConstant.BASE_TYPE) {
                for (BaseVandSVO baseVandSVO : baseVandSVOList) {
                    if (baseVandSVO.getId().equals(subscribe.getFacilityId())) {
                        baseVandSVO.setIsSubscribed(1);
                        baseVandSVO.setSubscribeId(subscribe.getId());
                        baseVandSVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡未订阅的部分由null统一为0
        for (BaseVandSVO baseVandSVO : baseVandSVOList) {
            if (baseVandSVO.getIsVisited() == null) {
                baseVandSVO.setIsVisited(0);
            }
            if (baseVandSVO.getIsSubscribed() == null) {
                baseVandSVO.setIsSubscribed(0);
            }
        }

        // 返回结果
        return baseVandSVOList;
    }

    @Override
    public List<Object> getAllVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        Long userId = visitAndSubscribeGetRequest.getUserId();

        // 获取游乐设施基础信息
        AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
        amusementFilterRequest.setUserId(userId);
        List<AmusementVandSVO> amusementVandSVOList = getAmusementVAndS(amusementFilterRequest);

        // 获取餐厅设施基础信息
        RestaurantFilterRequest restaurantFilterRequest = new RestaurantFilterRequest();
        restaurantFilterRequest.setUserId(userId);
        List<RestaurantVandSVO> restaurantVandSVOList = getRestaurantVAndS(restaurantFilterRequest);

        // 获取基础设施基础信息
        BaseFilterRequest baseFilterRequest = new BaseFilterRequest();
        baseFilterRequest.setUserId(userId);
        List<BaseVandSVO> baseVandSVOList = getBaseVAndS(baseFilterRequest);

        List<Object> data = new ArrayList<>();
        data.addAll(amusementVandSVOList);
        data.addAll(restaurantVandSVOList);
        data.addAll(baseVandSVOList);

        // 按拥挤度从低到高排序
        List<Object> sortedData = data.stream()
                .sorted(Comparator.comparingInt(o -> {
                    if (o instanceof AmusementVandSVO) {
                        return ((AmusementVandSVO) o).getExpectWaitTime();
                    }
                    else if (o instanceof RestaurantVandSVO) {
                        return ((RestaurantVandSVO) o).getExpectWaitTime();
                    } else if (o instanceof BaseVandSVO) {
                        return ((BaseVandSVO) o).getExpectWaitTime();
                    }
                    else {
                        throw new IllegalArgumentException("Unknown type in the list: " + o.getClass().getName());
                    }
                }))
                .collect(Collectors.toList());

        return sortedData;
    }
}




