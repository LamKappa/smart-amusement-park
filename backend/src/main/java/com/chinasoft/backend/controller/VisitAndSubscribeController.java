package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.facility.FacilityFilterRequest;
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeDeleteRequest;
import com.chinasoft.backend.service.facility.FilterService;
import com.chinasoft.backend.service.visitsubscribe.SubscribeService;
import com.chinasoft.backend.service.visitsubscribe.VisitService;
import org.apache.commons.lang3.ObjectUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 打卡、订阅接口
 *
 * @author 姜堂蕴之
 */
@RestController
public class VisitAndSubscribeController {

    @Autowired
    VisitService visitService;

    @Autowired
    SubscribeService subscribeService;

    @Autowired
    FilterService filterService;

    /**
     * 添加用户打卡记录
     *
     * @param visitAndSubscribeAddRequest 包含用户id、设施id和设施类别的打卡请求对象
     * @return 包含新增打卡记录的Visit对象的BaseResponse响应
     * @throws BusinessException 如果请求对象为null，则抛出参数错误的业务异常
     */
    @PostMapping("/visit/add")
    public BaseResponse<Visit> addVisit(@RequestBody VisitAndSubscribeAddRequest visitAndSubscribeAddRequest) {
        // 检查请求对象是否为空
        if (visitAndSubscribeAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用visitService的addVisit方法，传入请求对象，执行添加打卡记录操作，并返回新增的打卡记录
        Visit data = visitService.addVisit(visitAndSubscribeAddRequest);

        // 使用ResultUtils工具类将新增的打卡记录封装为成功的BaseResponse响应对象，并返回
        return ResultUtils.success(data);
    }

    /**
     * 根据打卡记录ID取消打卡
     *
     * @param visitAndSubscribeDeleteRequest 包含打卡记录ID的取消打卡请求对象
     * @return 包含操作结果的Boolean值的BaseResponse响应
     * @throws BusinessException 如果请求对象或打卡记录ID为空，或待取消的打卡记录不存在，则抛出相应的业务异常
     */
    @PostMapping("/visit/delete")
    public BaseResponse<Boolean> deleteVisit(@RequestBody VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest) {
        // 检查请求对象和打卡记录ID是否为空
        if (ObjectUtils.anyNull(visitAndSubscribeDeleteRequest, visitAndSubscribeDeleteRequest.getId())) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 初始化操作结果为false
        boolean res = false;

        // 根据打卡记录ID查询数据库，获取对应的打卡记录
        Visit visit = visitService.getById(visitAndSubscribeDeleteRequest.getId());

        // 校验待取消的打卡记录是否存在
        if (visit == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待取消的打卡记录不存在");
        }

        // 调用visitService的removeById方法，根据打卡记录ID删除打卡记录，并获取操作结果
        res = visitService.removeById(visitAndSubscribeDeleteRequest.getId());

        // 使用ResultUtils工具类将操作结果封装为成功的BaseResponse响应对象，并返回
        return ResultUtils.success(res);
    }

    /**
     * 根据用户手机号、设施id和设施类别进行订阅操作
     *
     * @param visitAndSubscribeAddRequest 包含用户手机号、设施id和设施类别的订阅请求对象
     * @return 包含新增订阅信息的Subscribe对象的BaseResponse响应
     * @throws BusinessException 如果请求对象为null，则抛出参数错误的业务异常
     */
    @PostMapping("/subscribe/add")
    public BaseResponse<Subscribe> addSubscribe(@RequestBody VisitAndSubscribeAddRequest visitAndSubscribeAddRequest) {
        // 检查请求对象是否为空
        if (visitAndSubscribeAddRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用subscribeService的addSubscribe方法，传入请求对象，执行订阅操作，并返回新增的订阅信息
        Subscribe data = subscribeService.addSubscribe(visitAndSubscribeAddRequest);

        // 使用ResultUtils工具类将新增的订阅信息封装为成功的BaseResponse响应对象，并返回
        return ResultUtils.success(data);
    }

    /**
     * 根据订阅记录ID取消订阅
     *
     * @param visitAndSubscribeDeleteRequest 包含订阅记录ID的取消订阅请求对象
     * @return 包含操作结果的Boolean值的BaseResponse响应
     * @throws BusinessException 如果请求对象或订阅记录ID为空，或待取消的订阅记录不存在，则抛出相应的业务异常
     */
    @PostMapping("/subscribe/delete")
    public BaseResponse<Boolean> deleteSubscribe(@RequestBody VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest) {
        // 检查请求对象和订阅记录ID是否为空
        if (ObjectUtils.anyNull(visitAndSubscribeDeleteRequest, visitAndSubscribeDeleteRequest.getId())) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 初始化操作结果为false
        boolean res = false;

        // 根据订阅记录ID查询数据库，获取对应的订阅记录
        Subscribe subscribe = subscribeService.getById(visitAndSubscribeDeleteRequest.getId());

        // 校验待取消的订阅记录是否存在
        if (subscribe == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待取消的订阅不存在");
        }

        // 调用subscribeService的removeById方法，根据订阅记录ID删除订阅记录，并获取操作结果
        res = subscribeService.removeById(visitAndSubscribeDeleteRequest.getId());

        // 使用ResultUtils工具类将操作结果封装为成功的BaseResponse响应对象，并返回
        return ResultUtils.success(res);
    }

    /**
     * 传入用户ID，展示所有设施的信息以及用户的打卡信息和订阅信息，按照拥挤度从低到高排序。
     *
     * @param facilityFilterRequest 设施筛选请求对象，包含用户ID等信息
     * @return 包含所有设施信息、用户打卡信息和订阅信息的响应结果
     * @throws BusinessException 参数错误异常，当传入的请求对象为空时抛出
     */
    @PostMapping("/getAllVisitAndSubscribe")
    public BaseResponse<List<Object>> getAllFacility(@RequestBody FacilityFilterRequest facilityFilterRequest) {
        // 检查参数是否为空
        if (facilityFilterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库，获取所有设施信息、用户打卡信息和订阅信息
        List<Object> data = filterService.getAllFacilityInfo(facilityFilterRequest);

        // 返回响应
        return ResultUtils.success(data);
    }


}
