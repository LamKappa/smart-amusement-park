package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.request.facility.RestaurantFilterRequest;
import com.chinasoft.backend.model.vo.visitsubscribe.AmusementVisitSubscribeVO;
import com.chinasoft.backend.model.vo.visitsubscribe.BaseVisitSubscribeVO;
import com.chinasoft.backend.model.vo.visitsubscribe.RestaurantVisitSubscribeVO;
import com.chinasoft.backend.service.visitsubscribe.VisitAndSubscribeService;
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
    VisitAndSubscribeService visitAndSubscribeService;

    /**
     * 展示游乐设施所有信息+打卡信息+订阅信息。
     */
    @PostMapping("/amusement")
    public BaseResponse<List<AmusementVisitSubscribeVO>> getAmusementVandS(@RequestBody AmusementFilterRequest amusementFilterRequest) {
        if (amusementFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<AmusementVisitSubscribeVO> data = visitAndSubscribeService.getAmusementVisitSubscribe(amusementFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 展示餐厅设施所有信息+打卡信息+订阅信息。
     */
    @PostMapping("/restaurant")
    public BaseResponse<List<RestaurantVisitSubscribeVO>> getRestaurantVandS(@RequestBody RestaurantFilterRequest restaurantFilterRequest) {
        if (restaurantFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<RestaurantVisitSubscribeVO> data = visitAndSubscribeService.getRestaurantVisitSubscribe(restaurantFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 展示基础设施所有信息+打卡信息+订阅信息。
     */
    @PostMapping("/base")
    public BaseResponse<List<BaseVisitSubscribeVO>> getBaseVandS(@RequestBody BaseFilterRequest baseFilterRequest) {
        if (baseFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<BaseVisitSubscribeVO> data = visitAndSubscribeService.getBaseVisitSubscribe(baseFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

}
