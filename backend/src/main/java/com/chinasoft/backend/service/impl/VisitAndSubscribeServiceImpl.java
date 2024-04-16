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
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeGetRequest;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;
import com.chinasoft.backend.model.vo.facility.BaseFacilityVO;
import com.chinasoft.backend.model.vo.facility.RestaurantFacilityVO;
import com.chinasoft.backend.model.vo.visitsubscribe.AmusementVisitSubscribeVO;
import com.chinasoft.backend.model.vo.visitsubscribe.BaseVisitSubscribeVO;
import com.chinasoft.backend.model.vo.visitsubscribe.RestaurantVisitSubscribeVO;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import com.chinasoft.backend.service.facility.RestaurantFacilityService;
import com.chinasoft.backend.service.visitsubscribe.VisitAndSubscribeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class VisitAndSubscribeServiceImpl implements VisitAndSubscribeService {

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
    public List<AmusementVisitSubscribeVO> getAmusementVisitSubscribe(AmusementFilterRequest amusementFilterRequest) {
        Long userId = amusementFilterRequest.getUserId();

        // 获取设施基础信息
        List<AmusementVisitSubscribeVO> amusementVisitSubscribeVOList = new ArrayList<>();
        List<AmusementFacilityVO> amusementFacilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
        for (AmusementFacilityVO amusementFacility : amusementFacilities) {
            AmusementVisitSubscribeVO amusementVisitSubscribeVO = new AmusementVisitSubscribeVO();
            BeanUtil.copyProperties(amusementFacility, amusementVisitSubscribeVO);
            amusementVisitSubscribeVOList.add(amusementVisitSubscribeVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.AMUSEMENT_TYPE) {
                for (AmusementVisitSubscribeVO amusementVisitSubscribeVO : amusementVisitSubscribeVOList) {
                    if (amusementVisitSubscribeVO.getId().equals(visit.getFacilityId())) {
                        amusementVisitSubscribeVO.setIsVisited(1);
                        amusementVisitSubscribeVO.setVisitId(visit.getId());
                        amusementVisitSubscribeVO.setVisitTime(visit.getCreateTime());
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
                for (AmusementVisitSubscribeVO amusementVisitSubscribeVO : amusementVisitSubscribeVOList) {
                    if (amusementVisitSubscribeVO.getId().equals(subscribe.getFacilityId())) {
                        amusementVisitSubscribeVO.setIsSubscribed(1);
                        amusementVisitSubscribeVO.setSubscribeId(subscribe.getId());
                        amusementVisitSubscribeVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡未订阅的部分由null统一为0
        for (AmusementVisitSubscribeVO amusementVisitSubscribeVO : amusementVisitSubscribeVOList) {
            if (amusementVisitSubscribeVO.getIsVisited() == null) {
                amusementVisitSubscribeVO.setIsVisited(0);
            }
            if (amusementVisitSubscribeVO.getIsSubscribed() == null) {
                amusementVisitSubscribeVO.setIsSubscribed(0);
            }
        }


        // 返回结果
        return amusementVisitSubscribeVOList;

    }

    @Override
    public List<RestaurantVisitSubscribeVO> getRestaurantVisitSubscribe(RestaurantFilterRequest restaurantFilterRequest) {
        Long userId = restaurantFilterRequest.getUserId();

        // 获取设施基础信息
        List<RestaurantVisitSubscribeVO> restaurantVisitSubscribeVOList = new ArrayList<>();
        List<RestaurantFacilityVO> restaurantFacilities = restaurantFacilityService.getRestaurantFacility(restaurantFilterRequest);
        for (RestaurantFacilityVO restaurantFacility : restaurantFacilities) {
            RestaurantVisitSubscribeVO restaurantVisitSubscribeVO = new RestaurantVisitSubscribeVO();
            BeanUtil.copyProperties(restaurantFacility, restaurantVisitSubscribeVO);
            restaurantVisitSubscribeVOList.add(restaurantVisitSubscribeVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.RESTAURANT_TYPE) {
                for (RestaurantVisitSubscribeVO restaurantVisitSubscribeVO : restaurantVisitSubscribeVOList) {
                    if (restaurantVisitSubscribeVO.getId().equals(visit.getFacilityId())) {
                        restaurantVisitSubscribeVO.setIsVisited(1);
                        restaurantVisitSubscribeVO.setVisitId(visit.getId());
                        restaurantVisitSubscribeVO.setVisitTime(visit.getCreateTime());
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
                for (RestaurantVisitSubscribeVO restaurantVisitSubscribeVO : restaurantVisitSubscribeVOList) {
                    if (restaurantVisitSubscribeVO.getId().equals(subscribe.getFacilityId())) {
                        restaurantVisitSubscribeVO.setIsSubscribed(1);
                        restaurantVisitSubscribeVO.setSubscribeId(subscribe.getId());
                        restaurantVisitSubscribeVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡未订阅的部分由null统一为0
        for (RestaurantVisitSubscribeVO restaurantVisitSubscribeVO : restaurantVisitSubscribeVOList) {
            if (restaurantVisitSubscribeVO.getIsVisited() == null) {
                restaurantVisitSubscribeVO.setIsVisited(0);
            }
            if (restaurantVisitSubscribeVO.getIsSubscribed() == null) {
                restaurantVisitSubscribeVO.setIsSubscribed(0);
            }
        }

        // 返回结果
        return restaurantVisitSubscribeVOList;
    }

    public List<BaseVisitSubscribeVO> getBaseVisitSubscribe(BaseFilterRequest baseFilterRequest) {
        Long userId = baseFilterRequest.getUserId();

        // 获取设施基础信息
        List<BaseVisitSubscribeVO> baseVisitSubscribeVOList = new ArrayList<>();
        List<BaseFacilityVO> baseFacilities = baseFacilityService.getBaseFacility(baseFilterRequest);
        for (BaseFacilityVO baseFacility : baseFacilities) {
            BaseVisitSubscribeVO baseVisitSubscribeVO = new BaseVisitSubscribeVO();
            BeanUtil.copyProperties(baseFacility, baseVisitSubscribeVO);
            baseVisitSubscribeVOList.add(baseVisitSubscribeVO);
        }

        // 获取打卡信息
        List<Visit> visits = visitMapper.selectList(Wrappers.<Visit>query().eq("user_id", userId));


        // 更新设施列表的is_visited状态
        for (Visit visit : visits) {
            if (visit.getFacilityType() == FacilityTypeConstant.BASE_TYPE) {
                for (BaseVisitSubscribeVO baseVisitSubscribeVO : baseVisitSubscribeVOList) {
                    if (baseVisitSubscribeVO.getId().equals(visit.getFacilityId())) {
                        baseVisitSubscribeVO.setIsVisited(1);
                        baseVisitSubscribeVO.setVisitId(visit.getId());
                        baseVisitSubscribeVO.setVisitTime(visit.getCreateTime());
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
                for (BaseVisitSubscribeVO baseVisitSubscribeVO : baseVisitSubscribeVOList) {
                    if (baseVisitSubscribeVO.getId().equals(subscribe.getFacilityId())) {
                        baseVisitSubscribeVO.setIsSubscribed(1);
                        baseVisitSubscribeVO.setSubscribeId(subscribe.getId());
                        baseVisitSubscribeVO.setSubscribeTime(subscribe.getCreateTime());
                        break;
                    }
                }
            }
        }

        // 将未打卡未订阅的部分由null统一为0
        for (BaseVisitSubscribeVO baseVisitSubscribeVO : baseVisitSubscribeVOList) {
            if (baseVisitSubscribeVO.getIsVisited() == null) {
                baseVisitSubscribeVO.setIsVisited(0);
            }
            if (baseVisitSubscribeVO.getIsSubscribed() == null) {
                baseVisitSubscribeVO.setIsSubscribed(0);
            }
        }

        // 返回结果
        return baseVisitSubscribeVOList;
    }

    @Override
    public List<Object> getAllVisitSubscribe(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        Long userId = visitAndSubscribeGetRequest.getUserId();

        // 获取游乐设施基础信息
        AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
        amusementFilterRequest.setUserId(userId);
        List<AmusementVisitSubscribeVO> amusementVisitSubscribeVOList = getAmusementVisitSubscribe(amusementFilterRequest);

        // 获取餐厅设施基础信息
        RestaurantFilterRequest restaurantFilterRequest = new RestaurantFilterRequest();
        restaurantFilterRequest.setUserId(userId);
        List<RestaurantVisitSubscribeVO> restaurantVisitSubscribeVOList = getRestaurantVisitSubscribe(restaurantFilterRequest);

        // 获取基础设施基础信息
        BaseFilterRequest baseFilterRequest = new BaseFilterRequest();
        baseFilterRequest.setUserId(userId);
        List<BaseVisitSubscribeVO> baseVisitSubscribeVOList = getBaseVisitSubscribe(baseFilterRequest);

        List<Object> data = new ArrayList<>();
        data.addAll(amusementVisitSubscribeVOList);
        data.addAll(restaurantVisitSubscribeVOList);
        data.addAll(baseVisitSubscribeVOList);

        // 按拥挤度从低到高排序
        List<Object> sortedData = data.stream()
                .sorted(Comparator.comparingInt(o -> {
                    if (o instanceof AmusementVisitSubscribeVO) {
                        return ((AmusementVisitSubscribeVO) o).getExpectWaitTime();
                    } else if (o instanceof RestaurantVisitSubscribeVO) {
                        return ((RestaurantVisitSubscribeVO) o).getExpectWaitTime();
                    } else if (o instanceof BaseVisitSubscribeVO) {
                        return ((BaseVisitSubscribeVO) o).getExpectWaitTime();
                    } else {
                        throw new IllegalArgumentException("Unknown type in the list: " + o.getClass().getName());
                    }
                }))
                .collect(Collectors.toList());

        return sortedData;
    }
}




