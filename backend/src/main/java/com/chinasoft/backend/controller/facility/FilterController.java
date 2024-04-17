package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityInfoVO;
import com.chinasoft.backend.model.vo.facility.BaseFacilityInfoVO;
import com.chinasoft.backend.model.vo.facility.RestaurantFacilityInfoVO;
import com.chinasoft.backend.service.facility.FilterService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 设施筛选接口
 *
 * @author 姜堂蕴之
 */
@RestController
@RequestMapping("/filter")
public class FilterController {

    @Autowired
    FilterService filterService;

    /**
     * 展示游乐设施基本信息与相关打卡信息和订阅信息
     * 支持的筛选条件包括：用户ID、游乐设施ID、设施名称、项目类型、用户身高
     * 若筛选条件为空，返回所有游乐设施的信息列表
     *
     * @param amusementFilterRequest 游乐设施的筛选条件请求体
     * @return 包含游乐设施信息列表的响应体
     * @throws BusinessException 如果请求体为空，则抛出业务异常
     */
    @PostMapping("/amusement")
    public BaseResponse<List<AmusementFacilityInfoVO>> getAmusementFacility(@RequestBody AmusementFilterRequest amusementFilterRequest) {

        // 检查请求体是否为空
        if (amusementFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库获取游乐设施信息
        List<AmusementFacilityInfoVO> data = filterService.getAmusementFacilityInfo(amusementFilterRequest);

        // 返回包含游乐设施信息的响应体
        return ResultUtils.success(data);
    }

    /**
     * 展示餐饮设施基本信息与相关打卡信息和订阅信息
     * 支持的筛选条件包括：用户ID、餐饮设施ID、设施名称、餐饮类型
     * 若筛选条件为空，返回所有餐厅设施的信息列表
     *
     * @param restaurantFilterRequest 餐厅设施的筛选条件请求体
     * @return 包含餐厅设施信息列表的响应体
     * @throws BusinessException 如果请求体为空，则抛出业务异常
     */
    @PostMapping("/restaurant")
    public BaseResponse<List<RestaurantFacilityInfoVO>> getRestaurantFacility(@RequestBody RestaurantFilterRequest restaurantFilterRequest) {

        // 检查请求体是否为空
        if (restaurantFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库获取餐厅设施信息
        List<RestaurantFacilityInfoVO> data = filterService.getRestaurantFacilityInfo(restaurantFilterRequest);

        // 返回包含餐厅设施信息的响应体
        return ResultUtils.success(data);
    }

    /**
     * 展示基础设施基本信息与相关打卡信息和订阅信息
     * 支持的筛选条件包括：用户ID、基础设施ID、设施名称
     * 若筛选条件为空，返回所有基础设施的信息列表
     *
     * @param baseFilterRequest 基础设施的筛选条件请求体
     * @return 包含基础设施信息列表的响应体
     * @throws BusinessException 如果请求体为空，则抛出业务异常
     */
    @PostMapping("/base")
    public BaseResponse<List<BaseFacilityInfoVO>> getBaseFacility(@RequestBody BaseFilterRequest baseFilterRequest) {

        // 检查请求体是否为空
        if (baseFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库获取基础设施信息
        List<BaseFacilityInfoVO> data = filterService.getBaseFacilityInfo(baseFilterRequest);

        // 返回包含基础设施信息的响应体
        return ResultUtils.success(data);
    }

}
