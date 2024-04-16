package com.chinasoft.backend.service.visitsubscribe;

import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeGetRequest;
import com.chinasoft.backend.model.vo.visitsubscribe.AmusementVisitSubscribeVO;
import com.chinasoft.backend.model.vo.visitsubscribe.BaseVisitSubscribeVO;
import com.chinasoft.backend.model.vo.visitsubscribe.RestaurantVisitSubscribeVO;

import java.util.List;

public interface VisitAndSubscribeService {
    List<AmusementVisitSubscribeVO> getAmusementVisitSubscribe(AmusementFilterRequest amusementFilterRequest);

    List<RestaurantVisitSubscribeVO> getRestaurantVisitSubscribe(RestaurantFilterRequest restaurantFilterRequest);

    List<BaseVisitSubscribeVO> getBaseVisitSubscribe(BaseFilterRequest baseFilterRequest);

    List<Object> getAllVisitSubscribe(VisitAndSubscribeGetRequest visitAndSubscribeGetRequest);
}
