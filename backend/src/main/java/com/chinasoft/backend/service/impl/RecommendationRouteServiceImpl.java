package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.mapper.*;
import com.chinasoft.backend.model.entity.route.RecommRoute;
import com.chinasoft.backend.model.entity.route.Route;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.request.route.AddRouteRequest;
import com.chinasoft.backend.model.request.route.DeleteRouteRequest;
import com.chinasoft.backend.model.request.route.RouteRecommendationRequest;
import com.chinasoft.backend.model.request.route.UpdateRouteRequest;
import com.chinasoft.backend.model.vo.RouteVO;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;
import com.chinasoft.backend.service.MapService;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.route.RecommendationRouteService;
import com.chinasoft.backend.service.route.RouteService;
import org.apache.commons.lang3.ObjectUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.net.HttpURLConnection;
import java.net.URL;
import java.util.*;

/**
 * 针对轮播图的数据库操作Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class RecommendationRouteServiceImpl implements RecommendationRouteService {
    /**
     * 路线名称最大允许长度
     */
    private static final int MAX_ROUTE_NAME_LENGTH = 50;

    /**
     * 路线名称最小允许长度
     */
    private static final int MIN_ROUTE_NAME_LENGTH = 0;

    /**
     * URL正则表达式
     */
    public static final String VALID_URL_PATTERN = "^(http|https)://.*$";

    @Autowired
    private AmusementFacilityService amusementFacilityService;

    @Autowired
    private AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    FacilityImageMapper facilityImageMapper;

    @Autowired
    VisitMapper visitMapper;

    @Autowired
    SubscribeMapper subscribeMapper;

    @Autowired
    RouteMapper routeMapper;

    @Autowired
    RecommRouteMapper recommRouteMapper;

    @Autowired
    MapService mapService;

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
    public List<RouteVO> getRecommendation(RouteRecommendationRequest routeRecommendationRequest) {
        // 创建查询包装器，用于构建查询条件
        QueryWrapper<Route> queryWrapper = new QueryWrapper<>();

        // 如果请求中包含路线ID，则按照ID进行查询
        if (routeRecommendationRequest.getId() != null) {
            queryWrapper.eq("id", routeRecommendationRequest.getId());
        }

        // 如果请求中包含路线名称，则按照名称进行模糊查询
        if (routeRecommendationRequest.getName() != null) {
            queryWrapper.like("name", routeRecommendationRequest.getName());
        }

        // 用于存储转换后的推荐路线信息列表
        List<RouteVO> routeVOList = new ArrayList<>();

        // 查询数据库获取推荐路线信息列表
        List<Route> routes = routeMapper.selectList(queryWrapper);

        // 遍历查询到的路线信息
        for (Route route : routes) {
            // 查询路线详情，并按照优先级升序排序
            List<RecommRoute> recommRoutes = recommRouteMapper.selectList(Wrappers.<RecommRoute>query()
                    .eq("route_id", route.getId())
                    .orderByAsc("priority"));

            // 用于存储当前路线的相关设施信息列表
            List<AmusementFacilityVO> swiperList = new ArrayList<>();

            // 遍历路线详情
            for (RecommRoute recommRoute : recommRoutes) {
                // 根据设施ID构建查询请求
                AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
                amusementFilterRequest.setId(recommRoute.getFacilityId());

                // 查询设施信息
                AmusementFacilityVO facility = new AmusementFacilityVO();
                List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
                if (facilities != null) {
                    facility = facilities.get(0);
                }

                // 将设施信息添加到当前路线的设施列表
                swiperList.add(facility);
            }

            // 创建RouteVO对象，并设置相关信息
            RouteVO routeVO = new RouteVO();
            routeVO.setId(route.getId());
            routeVO.setName(route.getName());
            routeVO.setImgUrl(route.getImgUrl());
            routeVO.setSwiperList(swiperList);

            // 将转换后的RouteVO对象添加到结果列表
            routeVOList.add(routeVO);
        }

        // 返回最终的推荐路线列表
        return routeVOList;
    }

    /**
     * 根据设施打卡次数排序，返回由打卡最多的四个设施组成的游玩路线
     *
     * @return 包含排序后游玩路线的BaseResponse对象
     * @author 姜堂蕴之
     */
    @Override
    public RouteVO sortByVisit() {
        // 使用自定义的SQL查询前4个设施的打卡ID和打卡次数
        List<Map> topFourFacilities = visitMapper.getTopFourFacilitiesByVisitCount();

        List<AmusementFacilityVO> swiperList = new ArrayList<>();

        for (Map facility : topFourFacilities) {
            Set keys = facility.keySet(); // 获取当前Map中的所有键，此处键数为1
            Long facilityId = null;
            for (Object key : keys) {
                facilityId = (Long) facility.get(key);
            }

            // 获取设施信息
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);
            AmusementFacilityVO amusementFacilityVO = new AmusementFacilityVO();
            List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            if (facilities != null) {
                amusementFacilityVO = facilities.get(0);
            }

            swiperList.add(amusementFacilityVO);
        }

        // 选取第一个设施的第一个图片作为封面（后续可换成默认封面）
        String imgUrl = swiperList.get(0).getImageUrls().get(0);

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(null);
        routeVO.setName("热门打卡路线");
        routeVO.setImgUrl(imgUrl);
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

    /**
     * 根据设施订阅数量排序，返回由订阅最多的四个设施组成的游玩路线
     *
     * @return 包含排序后游玩路线的BaseResponse对象
     * @author 姜堂蕴之
     */
    @Override
    public RouteVO sortBySubscribe() {
        // 使用自定义的SQL查询前4个设施的订阅ID和订阅次数
        List<Map> topFourFacilities = subscribeMapper.getTopFourFacilitiesBySubscribeCount();

        List<AmusementFacilityVO> swiperList = new ArrayList<>();

        for (Map facility : topFourFacilities) {
            Set keys = facility.keySet(); // 获取当前Map中的所有键，此处键数为1
            Long facilityId = null;
            for (Object key : keys) {
                facilityId = (Long) facility.get(key);
            }

            // 获取设施信息
            AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
            amusementFilterRequest.setId(facilityId);
            AmusementFacilityVO amusementFacilityVO = new AmusementFacilityVO();
            List<AmusementFacilityVO> facilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);
            if (facilities != null) {
                amusementFacilityVO = facilities.get(0);
            }

            swiperList.add(amusementFacilityVO);
        }

        // 选取第一个设施的第一个图片作为封面（后续可换成默认封面）
        String imgUrl = swiperList.get(0).getImageUrls().get(0);

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(null);
        routeVO.setName("人气订阅路线");
        routeVO.setImgUrl(imgUrl);
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

    /**
     * 返回由拥挤度排行的四个设施组成的游玩路线
     *
     * @author 孟祥硕
     */
    @Override
    public RouteVO sortCrowingLevel() {

        List<AmusementFacilityVO> swiperList = new ArrayList<>();
        // 获取所有设施
        AmusementFilterRequest amusementFilterRequest = new AmusementFilterRequest();
        List<AmusementFacilityVO> allFacilities = amusementFacilityService.getAmusementFacility(amusementFilterRequest);

        // 从小到大进行排序
        Collections.sort(allFacilities, (a, b) -> {
            return a.getExpectWaitTime() - b.getExpectWaitTime();
        });

        // 取前四个
        for (int i = 0; i < 4; i++) {
            swiperList.add(allFacilities.get(i));
        }

        // 选取第一个设施的第一个图片作为封面
        String imgUrl = swiperList.get(0).getImageUrls().get(0);

        // 填入数据
        RouteVO routeVO = new RouteVO();
        routeVO.setId(null);
        routeVO.setName("畅通无阻路线");
        routeVO.setImgUrl(imgUrl);
        routeVO.setSwiperList(swiperList);

        return routeVO;
    }

    /**
     * 增加路线
     *
     * @param addRouteRequest 包含新路线信息的请求对象
     * @return 包含新添加的路线列表的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @Override
    public List<RouteVO> addRoute(AddRouteRequest addRouteRequest) {
        // 获取添加路线请求的参数
        String name = addRouteRequest.getName();
        String imgUrl = addRouteRequest.getImgUrl();
        List<Integer> facilityIdList = addRouteRequest.getFacilityIdList();

        // 检查参数是否为空
        if (ObjectUtils.anyNull(name, imgUrl, facilityIdList)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 校验路线名称有效性
        if (name.length() > MAX_ROUTE_NAME_LENGTH || name.length() < MIN_ROUTE_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "路线名称长度不在允许范围内");
        }

        // 判断URL格式是否合法
        if (!(imgUrl.matches(VALID_URL_PATTERN))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL格式不正确");
        }

        // 发起HTTP请求检查URL是否可访问
        try {
            URL obj = new URL(imgUrl);
            HttpURLConnection con = (HttpURLConnection) obj.openConnection();
            con.setRequestMethod("GET");
            int responseCode = con.getResponseCode();
            if (responseCode != HttpURLConnection.HTTP_OK) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
            }
        } catch (Exception e) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
        }

        // 校验途径设施列表中的设施是否存在
        for (Integer facilityId : facilityIdList) {
            if (amusementFacilityMapper.selectById(facilityId) == null) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "添加设施不存在");
            }
        }

        // 校验插入数据是否重复
        List<RouteVO> allRouteVOList = getRecommendation(new RouteRecommendationRequest());

        for (RouteVO existingRouteVO : allRouteVOList) {
            String existingName = existingRouteVO.getName();
            String existingImgUrl = existingRouteVO.getImgUrl();
            List<AmusementFacilityVO> existingSwiperList = existingRouteVO.getSwiperList();
            List<Integer> existingFacilityIdList = new ArrayList<>();

            for (AmusementFacilityVO amusementFacilityVO : existingSwiperList) {
                existingFacilityIdList.add(Math.toIntExact(amusementFacilityVO.getId()));
            }

            if (name.equals(existingName)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "名称重复");
            }

            if (imgUrl.equals(existingImgUrl)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "图片重复");
            }

            if (facilityIdList.equals(existingFacilityIdList)) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "途径设施重复");
            }
        }

        // 插入路线数据
        Route route = new Route();
        route.setName(name);
        route.setImgUrl(imgUrl);
        routeMapper.insert(route);

        Long routeId = route.getId();

        // 插入途径设施数据
        for (Integer facilityId : facilityIdList) {
            RecommRoute recommRoute = new RecommRoute();
            recommRoute.setRouteId(routeId);
            recommRoute.setFacilityId(Long.valueOf(facilityId));
            recommRoute.setPriority(facilityIdList.indexOf(facilityId) + 1);
            recommRouteMapper.insert(recommRoute);
        }

        // 获取插入的推荐路线
        RouteRecommendationRequest routeRecommendationRequest = new RouteRecommendationRequest();
        routeRecommendationRequest.setId(routeId);
        List<RouteVO> routeVOList = getRecommendation(routeRecommendationRequest);

        return routeVOList;
    }

    /**
     * 删除路线
     *
     * @param deleteRouteRequest 包含要删除路线信息的请求对象
     * @return 包含删除操作结果的BaseResponse对象，其中data字段表示删除是否成功
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @Override
    public Boolean deleteRoute(DeleteRouteRequest deleteRouteRequest) {
        // 获取待删除路线的ID
        Long routeId = deleteRouteRequest.getId();

        // 如果路线ID为空，则抛出参数错误的业务异常
        if (routeId == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 根据路线ID查询路线信息
        Route route = routeService.getById(routeId);

        // 如果待删除的路线不存在，则抛出未找到的错误业务异常
        if (route == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待删除的路线不存在");
        }

        // 创建查询条件包装器，用于指定删除操作的筛选条件
        QueryWrapper<RecommRoute> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("route_id", routeId); // 筛选条件：route_id等于传入的routeId

        // 首先删除路线详情表中对应的数据
        int deletedCount = recommRouteMapper.delete(queryWrapper);

        // 接着删除路线表中的数据
        Boolean res = routeService.removeById(routeId);

        // 如果上述两次删除均成功，则返回true表示删除成功
        if (deletedCount > 0 && res) {
            return true;
        } else {
            // 否则返回false表示删除失败
            return false;
        }
    }

    /**
     * 更新路线
     *
     * @param updateRouteRequest 包含更新后路线信息的请求对象
     * @return 包含更新后路线列表的BaseResponse对象
     * @throws BusinessException 如果请求体为空，则抛出参数错误异常
     * @author 姜堂蕴之
     */
    @Override
    public List<RouteVO> updateRoute(UpdateRouteRequest updateRouteRequest) {
        // 获取更新路线请求的参数
        Long routeId = updateRouteRequest.getId();
        String name = updateRouteRequest.getName();
        String imgUrl = updateRouteRequest.getImgUrl();
        List<Integer> facilityIdList = updateRouteRequest.getFacilityIdList();

        // 检查参数是否为空
        if (ObjectUtils.anyNull(routeId, name, imgUrl, facilityIdList)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }

        // 获取待更新的原有路线信息
        Route originalRoute = routeService.getById(routeId);

        // 校验待更新的路线是否存在
        if (originalRoute == null) {
            throw new BusinessException(ErrorCode.NOT_FOUND_ERROR, "待更新的路线不存在");
        }

        // 校验更新路线名称有效性
        if (name.length() > MAX_ROUTE_NAME_LENGTH || name.length() < MIN_ROUTE_NAME_LENGTH) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "路线名称长度不在允许范围内");
        }

        // 判断更新URL格式是否合法
        if (!(imgUrl.matches(VALID_URL_PATTERN))) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL格式不正确");
        }

        // 发起HTTP请求检查更新URL是否可访问
        try {
            URL obj = new URL(imgUrl);
            HttpURLConnection con = (HttpURLConnection) obj.openConnection();
            con.setRequestMethod("GET");
            int responseCode = con.getResponseCode();
            if (responseCode != HttpURLConnection.HTTP_OK) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
            }
        } catch (Exception e) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "URL不可访问");
        }

        // 校验更新途径设施列表中的设施是否存在
        for (Integer facilityId : facilityIdList) {
            if (amusementFacilityMapper.selectById(facilityId) == null) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "添加设施不存在");
            }
        }

        // 校验插入数据是否重复（允许与该路线原有数据重复）
        List<RouteVO> allRouteVOList = getRecommendation(new RouteRecommendationRequest());

        for (RouteVO existingRouteVO : allRouteVOList) {
            String existingName = existingRouteVO.getName();
            String existingImgUrl = existingRouteVO.getImgUrl();
            List<AmusementFacilityVO> existingSwiperList = existingRouteVO.getSwiperList();
            List<Integer> existingFacilityIdList = new ArrayList<>();

            for (AmusementFacilityVO amusementFacilityVO : existingSwiperList) {
                existingFacilityIdList.add(Math.toIntExact(amusementFacilityVO.getId()));
            }

            if (name.equals(existingName) && !(name.equals(originalRoute.getName()))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "名称重复");
            }

            if (imgUrl.equals(existingImgUrl) && !(imgUrl.equals(originalRoute.getImgUrl()))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "图片重复");
            }

            if (facilityIdList.equals(existingFacilityIdList) && !(facilityIdList.equals(updateRouteRequest.getFacilityIdList()))) {
                throw new BusinessException(ErrorCode.PARAMS_ERROR, "途径设施重复");
            }
        }

        // 在路线表中更新路线信息
        Route updateRoute = new Route();
        updateRoute.setId(routeId);
        updateRoute.setName(name);
        updateRoute.setImgUrl(imgUrl);
        routeMapper.updateById(updateRoute);

        // 在路线详情表中删除原有的路线详情记录
        QueryWrapper<RecommRoute> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("route_id", routeId);
        recommRouteMapper.delete(queryWrapper);

        // 增加现有的路线详情记录
        for (Integer facilityId : facilityIdList) {
            RecommRoute recommRoute = new RecommRoute();
            recommRoute.setRouteId(routeId);
            recommRoute.setFacilityId(Long.valueOf(facilityId));
            recommRoute.setPriority(facilityIdList.indexOf(facilityId) + 1);
            recommRouteMapper.insert(recommRoute);
        }

        // 获取更新的推荐路线列表
        RouteRecommendationRequest routeRecommendationRequest = new RouteRecommendationRequest();
        routeRecommendationRequest.setId(routeId);
        List<RouteVO> routeVOList = getRecommendation(routeRecommendationRequest);

        return routeVOList;
    }

}




