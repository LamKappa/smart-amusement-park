package com.chinasoft.backend.service.impl;

import cn.hutool.core.bean.BeanUtil;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
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

    public List<Object> getVisitAndSubscribe(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        Long userId = visitAndSubscribeGetRequest.getUserId();

        // 获取amusementFacilities, restaurantFacilities, baseFacilities
        List<AmusementVandSVO> amusementVandSVOList = new ArrayList<>();
        List<AmusementFacilityVO> amusementFacilities = amusementFacilityService.getAmusementFacility(new AmusementFilterRequest());
        for(AmusementFacilityVO amusementFacility : amusementFacilities){
            AmusementVandSVO amusementVandSVO = new AmusementVandSVO();
            BeanUtil.copyProperties(amusementFacility, amusementVandSVO);
            amusementVandSVOList.add(amusementVandSVO);
        }

        List<RestaurantVandSVO> restaurantVandSVOList = new ArrayList<>();
        List<RestaurantFacilityVO> restaurantFacilities = restaurantFacilityService.getRestaurantFacility(new RestaurantFilterRequest());
        for(RestaurantFacilityVO restaurantFacility : restaurantFacilities){
            RestaurantVandSVO restaurantVandSVO = new RestaurantVandSVO();
            BeanUtil.copyProperties(restaurantFacility, restaurantVandSVO);
            restaurantVandSVOList.add(restaurantVandSVO);
        }

        List<BaseVandSVO> baseVandSVOList = new ArrayList<>();
        List<BaseFacilityVO> baseFacilities = baseFacilityService.getBaseFacility(new BaseFilterRequest());
        for(BaseFacilityVO baseFacility : baseFacilities){
            BaseVandSVO baseVandSVO = new BaseVandSVO();
            BeanUtil.copyProperties(baseFacility, baseVandSVO);
            baseVandSVOList.add(baseVandSVO);
        }

        // 获取visitedFacilities
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            switch (visit.getFacilityType()) {
                case 0: // Amusement Facility
                    for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
                        if (amusementVandSVO.getId().equals(visit.getFacilityId())) {
                            amusementVandSVO.setIsVisited(1);
                            amusementVandSVO.setVisitId(visit.getId());
                            break;
                        }
                    }
                    break;
                case 1: // Restaurant Facility
                    for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
                        if (restaurantVandSVO.getId().equals(visit.getFacilityId())) {
                            restaurantVandSVO.setIsVisited(1);
                            restaurantVandSVO.setVisitId(visit.getId());
                            break;
                        }
                    }
                    break;
                case 2: // Base Facility
                    for (BaseVandSVO baseVandSVO : baseVandSVOList) {
                        if (baseVandSVO.getId().equals(visit.getFacilityId())) {
                            baseVandSVO.setIsVisited(1);
                            baseVandSVO.setVisitId(visit.getId());
                            break;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // 获取subscribedFacilities
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_visited状态
        for (Subscribe subscribe : subscribes) {
            switch (subscribe.getFacilityType()) {
                case 0: // Amusement Facility
                    for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
                        if (amusementVandSVO.getId().equals(subscribe.getFacilityId())) {
                            amusementVandSVO.setIsSubscribed(1);
                            amusementVandSVO.setSubscribeId(subscribe.getId());
                            break;
                        }
                    }
                    break;
                case 1: // Restaurant Facility
                    for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
                        if (restaurantVandSVO.getId().equals(subscribe.getFacilityId())) {
                            restaurantVandSVO.setIsSubscribed(1);
                            restaurantVandSVO.setSubscribeId(subscribe.getId());
                            break;
                        }
                    }
                    break;
                case 2: // Base Facility
                    for (BaseVandSVO baseVandSVO : baseVandSVOList) {
                        if (baseVandSVO.getId().equals(subscribe.getFacilityId())) {
                            baseVandSVO.setIsSubscribed(1);
                            baseVandSVO.setSubscribeId(subscribe.getId());
                            break;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // 构建VisitAndSubscribeVO对象并返回，这里假设VO对象包含上述设施列表
        List<Object> data = new ArrayList<>();
        data.addAll(amusementVandSVOList);
        data.addAll(restaurantVandSVOList);
        data.addAll(baseVandSVOList);

        return data;

    }


    @Override
    public List<AmusementVandSVO> getAmusementVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        Long userId = visitAndSubscribeGetRequest.getUserId();

        // 获取游乐设施基础信息
        List<AmusementVandSVO> amusementVandSVOList = new ArrayList<>();
        List<AmusementFacilityVO> amusementFacilities = amusementFacilityService.getAmusementFacility(new AmusementFilterRequest());
        for(AmusementFacilityVO amusementFacility : amusementFacilities){
            AmusementVandSVO amusementVandSVO = new AmusementVandSVO();
            BeanUtil.copyProperties(amusementFacility, amusementVandSVO);
            amusementVandSVOList.add(amusementVandSVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == 0) {
                for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
                    if (amusementVandSVO.getId().equals(visit.getFacilityId())) {
                        amusementVandSVO.setIsVisited(1);
                        amusementVandSVO.setVisitId(visit.getId());
                        break;
                    }
                }
            }
        }

        // 获取订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_visited状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == 0) {
                for (AmusementVandSVO amusementVandSVO : amusementVandSVOList) {
                    if (amusementVandSVO.getId().equals(subscribe.getFacilityId())) {
                        amusementVandSVO.setIsSubscribed(1);
                        amusementVandSVO.setSubscribeId(subscribe.getId());
                        break;
                    }
                }
            }
        }

        // 返回结果
        return amusementVandSVOList;

    }

    @Override
    public List<RestaurantVandSVO> getRestaurantVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        Long userId = visitAndSubscribeGetRequest.getUserId();

        // 获取游乐设施基础信息
        List<RestaurantVandSVO> restaurantVandSVOList = new ArrayList<>();
        List<RestaurantFacilityVO> restaurantFacilities = restaurantFacilityService.getRestaurantFacility(new RestaurantFilterRequest());
        for(RestaurantFacilityVO restaurantFacility : restaurantFacilities){
            RestaurantVandSVO restaurantVandSVO = new RestaurantVandSVO();
            BeanUtil.copyProperties(restaurantFacility, restaurantVandSVO);
            restaurantVandSVOList.add(restaurantVandSVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == 1) {
                for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
                    if (restaurantVandSVO.getId().equals(visit.getFacilityId())) {
                        restaurantVandSVO.setIsVisited(1);
                        restaurantVandSVO.setVisitId(visit.getId());
                        break;
                    }
                }
            }
        }

        // 获取订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_visited状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == 0) {
                for (RestaurantVandSVO restaurantVandSVO : restaurantVandSVOList) {
                    if (restaurantVandSVO.getId().equals(subscribe.getFacilityId())) {
                        restaurantVandSVO.setIsSubscribed(1);
                        restaurantVandSVO.setSubscribeId(subscribe.getId());
                        break;
                    }
                }
            }
        }

        // 返回结果
        return restaurantVandSVOList;
    }

    public List<BaseVandSVO> getBaseVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        Long userId = visitAndSubscribeGetRequest.getUserId();

        // 获取游乐设施基础信息
        List<BaseVandSVO> baseVandSVOList = new ArrayList<>();
        List<BaseFacilityVO> baseFacilities = baseFacilityService.getBaseFacility(new BaseFilterRequest());
        for(BaseFacilityVO baseFacility : baseFacilities){
            BaseVandSVO baseVandSVO = new BaseVandSVO();
            BeanUtil.copyProperties(baseFacility, baseVandSVO);
            baseVandSVOList.add(baseVandSVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == 2) {
                for (BaseVandSVO baseVandSVO : baseVandSVOList) {
                    if (baseVandSVO.getId().equals(visit.getFacilityId())) {
                        baseVandSVO.setIsVisited(1);
                        baseVandSVO.setVisitId(visit.getId());
                        break;
                    }
                }
            }
        }

        // 获取订阅信息
        List<Subscribe> subscribes = subscribeMapper.selectList(Wrappers.<Subscribe>query().eq("user_id", userId));

        // 更新设施列表的is_visited状态
        for (Subscribe subscribe : subscribes) {
            if (subscribe.getFacilityType() == 0) {
                for (BaseVandSVO baseVandSVO : baseVandSVOList) {
                    if (baseVandSVO.getId().equals(subscribe.getFacilityId())) {
                        baseVandSVO.setIsSubscribed(1);
                        baseVandSVO.setSubscribeId(subscribe.getId());
                        break;
                    }
                }
            }
        }

        // 返回结果
        return baseVandSVOList;
    }
}




