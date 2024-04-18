package com.chinasoft.backend.service.route;

import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.request.route.AddRouteRequest;
import com.chinasoft.backend.model.request.route.DeleteRouteRequest;
import com.chinasoft.backend.model.request.route.RouteRecommendationRequest;
import com.chinasoft.backend.model.request.route.UpdateRouteRequest;
import com.chinasoft.backend.model.vo.RouteVO;

import java.util.List;

/**
 * 针对搜索的数据库操作Service
 *
 * @author 姜堂蕴之
 */
public interface RecommendationRouteService {

    /**
     * 查询路线
     *
     * @param routeRecommendationRequest 包含查询条件的设施推荐请求对象，用户不输入查询条件时，返回所有推荐路线
     * @return 包含推荐路线的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    List<RouteVO> getRecommendation(RouteRecommendationRequest routeRecommendationRequest);

    /**
     * 根据设施打卡次数排序，返回由打卡最多的四个设施组成的游玩路线
     *
     * @return 包含排序后游玩路线的BaseResponse对象
     * @author 姜堂蕴之
     */
    RouteVO sortByVisit();

    /**
     * 根据设施订阅数量排序，返回由订阅最多的四个设施组成的游玩路线
     *
     * @return 包含排序后游玩路线的BaseResponse对象
     * @author 姜堂蕴之
     */
    RouteVO sortBySubscribe();

    /**
     * 返回由拥挤度排行的四个设施组成的游玩路线
     *
     * @author 孟祥硕
     */
    RouteVO sortCrowingLevel();

    /**
     * 增加路线
     *
     * @param addRouteRequest 包含新路线信息的请求对象
     * @return 包含新添加的路线列表的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    List<RouteVO> addRoute(AddRouteRequest addRouteRequest);

    /**
     * 删除路线
     *
     * @param deleteRouteRequest 包含要删除路线信息的请求对象
     * @return 包含删除操作结果的BaseResponse对象，其中data字段表示删除是否成功
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    Boolean deleteRoute(DeleteRouteRequest deleteRouteRequest);

    /**
     * 更新路线
     *
     * @param updateRouteRequest 包含更新后路线信息的请求对象
     * @return 包含更新后路线列表的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    List<RouteVO> updateRoute(UpdateRouteRequest updateRouteRequest);
}
