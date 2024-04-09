package com.chinasoft.backend.service;

import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.request.RestaurantFilterRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeGetRequest;
import com.chinasoft.backend.model.vo.AmusementVandSVO;
import com.chinasoft.backend.model.vo.BaseVandSVO;
import com.chinasoft.backend.model.vo.RestaurantVandSVO;

import java.util.List;

public interface VisitAndSubscribeService {
    List<AmusementVandSVO> getAmusementVAndS(AmusementFilterRequest amusementFilterRequest);

    List<RestaurantVandSVO> getRestaurantVAndS(RestaurantFilterRequest restaurantFilterRequest);

    List<BaseVandSVO> getBaseVAndS(BaseFilterRequest baseFilterRequest);

    List<Object> getAllVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest);
}
