package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.route.Route;
import com.chinasoft.backend.model.request.route.AddRouteRequest;
import com.chinasoft.backend.model.request.route.DeleteRouteRequest;
import com.chinasoft.backend.model.request.route.RouteRecommendationRequest;
import com.chinasoft.backend.model.request.route.UpdateRouteRequest;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.service.route.RecommendationRouteService;
import com.chinasoft.backend.service.route.RouteService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 路线接口
 *
 * @author 孟祥硕 姜堂蕴之
 */
@RestController
public class RouteController {

    @Autowired
    RecommendationRouteService recommendationRouteService;

    @Autowired
    RouteService routeService;

    /**
     * 查询路线
     *
     * @param routeRecommendationRequest 包含查询条件的设施推荐请求对象，用户不输入查询条件时，返回所有推荐路线
     * @return 包含推荐路线的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @PostMapping("/recommendation")
    public BaseResponse<List<RouteVO>> getRecommendation(@RequestBody RouteRecommendationRequest routeRecommendationRequest) {
        // 检查请求体是否为空
        if (routeRecommendationRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用推荐设施服务查询数据库，获取符合查询条件的设施列表
        List<RouteVO> data = recommendationRouteService.getRecommendation(routeRecommendationRequest);

        // 使用工具类构建成功响应，并返回包含推荐设施列表的BaseResponse对象
        return ResultUtils.success(data);
    }

    /**
     * 推荐路线导航使用次数统计
     *
     * @author 孟祥硕
     */
    @GetMapping("/recommendation/countPlus")
    public BaseResponse<Boolean> recommendationRouteNav(@Param("id") Integer routeId) {

        if (routeId == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 统计次数+1
        Route route = routeService.getById(routeId);
        route.setUseCount(route.getUseCount() + 1);
        boolean b = routeService.updateById(route);

        return ResultUtils.success(b);
    }

    /**
     * 根据设施打卡次数排序，返回由打卡最多的四个设施组成的游玩路线
     *
     * @return 包含排序后游玩路线的BaseResponse对象
     * @author 姜堂蕴之
     */
    @GetMapping("/recommendation/sortByVisit")
    public BaseResponse<RouteVO> sortByVisit() {
        // 调用推荐设施服务查询数据库，获取根据打卡次数排序后的游玩路线，通常包含打卡最多的四个设施
        RouteVO data = recommendationRouteService.sortByVisit();

        // 使用工具类构建成功响应，并返回包含排序后游玩路线的BaseResponse对象
        return ResultUtils.success(data);
    }

    /**
     * 根据设施订阅数量排序，返回由订阅最多的四个设施组成的游玩路线
     *
     * @return 包含排序后游玩路线的BaseResponse对象
     * @author 姜堂蕴之
     */
    @GetMapping("/recommendation/sortBySubscribe")
    public BaseResponse<RouteVO> sortBySubscribe() {
        // 调用推荐设施服务查询数据库，根据设施的订阅数量进行排序，并获取订阅最多的四个设施组成的游玩路线
        RouteVO data = recommendationRouteService.sortBySubscribe();

        // 使用工具类构建成功响应，并返回包含排序后游玩路线的BaseResponse对象
        return ResultUtils.success(data);
    }

    /**
     * 返回由订拥挤度排行的四个设施组成的游玩路线
     *
     * @author 孟祥硕
     */
    @GetMapping("/recommendation/sortCrowingLevel")
    public BaseResponse<RouteVO> sortCrowingLevel() {
        // 查询数据库
        RouteVO data = recommendationRouteService.sortCrowingLevel();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 增加路线
     *
     * @param addRouteRequest 包含新路线信息的请求对象
     * @return 包含新添加的路线列表的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @PostMapping("/recommendation/addRoute")
    public BaseResponse<List<RouteVO>> addRoute(@RequestBody AddRouteRequest addRouteRequest) {
        // 检查请求体是否为空
        if (addRouteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用推荐设施服务，将新路线添加到数据库中，并获取新添加的路线列表
        List<RouteVO> data = recommendationRouteService.addRoute(addRouteRequest);

        // 使用工具类构建成功响应，并返回包含新添加的路线列表的BaseResponse对象
        return ResultUtils.success(data);
    }

    /**
     * 删除路线
     *
     * @param deleteRouteRequest 包含要删除路线信息的请求对象
     * @return 包含删除操作结果的BaseResponse对象，其中data字段表示删除是否成功
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @PostMapping("/recommendation/deleteRoute")
    public BaseResponse<Boolean> deleteRoute(@RequestBody DeleteRouteRequest deleteRouteRequest) {
        // 检查请求体是否为空
        if (deleteRouteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 调用推荐设施服务删除指定路线，并获取删除操作的结果
        Boolean data = recommendationRouteService.deleteRoute(deleteRouteRequest);

        // 使用工具类构建成功响应，并返回包含删除操作结果的BaseResponse对象
        return ResultUtils.success(data);
    }

    /**
     * 更新路线
     *
     * @param updateRouteRequest 包含更新后路线信息的请求对象
     * @return 包含更新后路线列表的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @PostMapping("/recommendation/updateRoute")
    public BaseResponse<List<RouteVO>> updateRoute(@RequestBody UpdateRouteRequest updateRouteRequest) {
        // 检查请求体是否为空
        if (updateRouteRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 调用推荐设施服务更新路线信息，并获取更新后的路线列表
        List<RouteVO> updatedRoutes = recommendationRouteService.updateRoute(updateRouteRequest);

        // 使用工具类构建成功响应，并返回包含更新后路线列表的BaseResponse对象
        return ResultUtils.success(updatedRoutes);
    }
}
