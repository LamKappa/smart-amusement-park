package com.chinasoft.backend.service;

import com.chinasoft.backend.model.request.VisitAndSubscribeGetRequest;
import com.chinasoft.backend.model.vo.AmusementVandSVO;
import com.chinasoft.backend.model.vo.BaseVandSVO;
import com.chinasoft.backend.model.vo.RestaurantVandSVO;
import com.chinasoft.backend.model.vo.VisitAndSubscribeVO;

import java.util.List;

public interface VisitAndSubscribeService {
    List<AmusementVandSVO> getAmusementVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest);

    List<RestaurantVandSVO> getRestaurantVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest);

    List<BaseVandSVO> getBaseVAndS(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest);
}
