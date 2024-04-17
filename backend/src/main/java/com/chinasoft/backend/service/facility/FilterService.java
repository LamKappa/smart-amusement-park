package com.chinasoft.backend.service.facility;

import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.request.visitsubscribe.FacilityFilterRequest;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityInfoVO;
import com.chinasoft.backend.model.vo.facility.BaseFacilityInfoVO;
import com.chinasoft.backend.model.vo.facility.RestaurantFacilityInfoVO;

import java.util.List;

/**
 * 设施筛选Service
 *
 * @author 姜堂蕴之
 */
public interface FilterService {

    /**
     * 根据游乐设施筛选条件，获取游乐设施的信息列表。
     *
     * @param amusementFilterRequest 包含筛选条件的游乐设施请求对象
     * @return 游乐设施信息列表
     */
    List<AmusementFacilityInfoVO> getAmusementFacilityInfo(AmusementFilterRequest amusementFilterRequest);

    /**
     * 根据餐饮设施筛选条件，获取餐厅设施的信息列表。
     *
     * @param restaurantFilterRequest 包含筛选条件的餐饮设施请求对象
     * @return 餐饮设施信息列表
     */
    List<RestaurantFacilityInfoVO> getRestaurantFacilityInfo(RestaurantFilterRequest restaurantFilterRequest);

    /**
     * 根据基础设施筛选条件，获取基础设施的信息列表。
     *
     * @param baseFilterRequest 包含筛选条件的基础设施请求对象
     * @return 基础设施信息列表
     */
    List<BaseFacilityInfoVO> getBaseFacilityInfo(BaseFilterRequest baseFilterRequest);

    /**
     * 获取所有类型的设施信息列表，根据传入的请求条件进行筛选。
     *
     * @param facilityFilterRequest 包含筛选条件的通用请求对象
     * @return 包含各类设施信息的对象列表
     */
    List<Object> getAllFacilityInfo(FacilityFilterRequest facilityFilterRequest);

}
