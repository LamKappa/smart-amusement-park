package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeDeleteRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeGetRequest;
import com.chinasoft.backend.model.vo.AmusementVandSVO;
import com.chinasoft.backend.model.vo.BaseVandSVO;
import com.chinasoft.backend.model.vo.RestaurantVandSVO;
import com.chinasoft.backend.service.SubscribeService;
import com.chinasoft.backend.service.VisitAndSubscribeService;
import com.chinasoft.backend.service.VisitService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class VisitAndSubscribeController {

    @Autowired
    VisitService visitService;

    @Autowired
    SubscribeService subscribeService;

    @Autowired
    VisitAndSubscribeService visitAndSubscribeService;

    /**
     * 传入用户id、设施id、设施类别（0为游乐设施，1为餐厅设施，2为基础设施）进行打卡。
     */
    @PostMapping("/visit/add")
    public BaseResponse<Visit> addVisit(@RequestBody VisitAndSubscribeAddRequest visitAndSubscribeAddRequest) {
        if (visitAndSubscribeAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        Visit data = visitService.addVisit(visitAndSubscribeAddRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 传入打卡记录id取消打卡。
     */
    @PostMapping("/visit/delete")
    public BaseResponse<Boolean> deleteVisit(@RequestBody VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest) {
        if (visitAndSubscribeDeleteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        Boolean data = visitService.deleteVisit(visitAndSubscribeDeleteRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 传入用户手机号、设施id、设施类别（0为游乐设施，1为餐厅设施，2为基础设施）进行订阅。
     */
    @PostMapping("/subscribe/add")
    public BaseResponse<Subscribe> addSubscribe(@RequestBody VisitAndSubscribeAddRequest visitAndSubscribeAddRequest) {
        if (visitAndSubscribeAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        Subscribe data = subscribeService.addSubscribe(visitAndSubscribeAddRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 传入订阅记录id取消订阅。
     */
    @PostMapping("/subscribe/delete")
    public BaseResponse<Boolean> deleteSubscribe(@RequestBody VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest) {
        if (visitAndSubscribeDeleteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        Boolean data = subscribeService.deleteSubscribe(visitAndSubscribeDeleteRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

//    /**
//     * 传入用户id，展示游乐设施所有信息+打卡信息+订阅信息。
//     */
//    @PostMapping("/getVisitAndSubscribe/amusement")
//    public BaseResponse<List<AmusementVandSVO>> getAmusementVandS(@RequestBody VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
//        if (visitAndSubscribeGetRequest == null) {
//            throw new BusinessException(ErrorCode.PARAMS_ERROR);
//        }
//
//        // 查询数据库
//        List<AmusementVandSVO> data = visitAndSubscribeService.getAmusementVAndS(visitAndSubscribeGetRequest);
//
//        // 返回响应
//        return ResultUtils.success(data);
//    }
//
//    /**
//     * 传入用户id，展示餐厅设施所有信息+打卡信息+订阅信息。
//     */
//    @PostMapping("/getVisitAndSubscribe/restaurant")
//    public BaseResponse<List<RestaurantVandSVO>> getRestaurantVandS(@RequestBody VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
//        if (visitAndSubscribeGetRequest == null) {
//            throw new BusinessException(ErrorCode.PARAMS_ERROR);
//        }
//
//        // 查询数据库
//        List<RestaurantVandSVO> data = visitAndSubscribeService.getRestaurantVAndS(visitAndSubscribeGetRequest);
//
//        // 返回响应
//        return ResultUtils.success(data);
//    }
//
//    /**
//     * 传入用户id，展示基础设施所有信息+打卡信息+订阅信息。
//     */
//    @PostMapping("/getVisitAndSubscribe/base")
//    public BaseResponse<List<BaseVandSVO>> getBaseVandS(@RequestBody VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
//        if (visitAndSubscribeGetRequest == null) {
//            throw new BusinessException(ErrorCode.PARAMS_ERROR);
//        }
//
//        // 查询数据库
//        List<BaseVandSVO> data = visitAndSubscribeService.getBaseVAndS(visitAndSubscribeGetRequest);
//
//        // 返回响应
//        return ResultUtils.success(data);
//    }

    /**
     * 传入用户id，展示所有设施的信息+打卡信息+订阅信息（按拥挤度从低到高排序）。
     */
    @PostMapping("/getAllVisitAndSubscribe")
    public BaseResponse<List<Object>> getAllVandS(@RequestBody VisitAndSubscribeGetRequest visitAndSubscribeGetRequest) {
        if (visitAndSubscribeGetRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<Object> data = visitAndSubscribeService.getAllVAndS(visitAndSubscribeGetRequest);

        // 返回响应
        return ResultUtils.success(data);
    }

}
