package com.chinasoft.backend.controller.statistics;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.Route;
import com.chinasoft.backend.model.vo.FacilityHeadCountVO;
import com.chinasoft.backend.model.vo.CrowingTimeCountVO;
import com.chinasoft.backend.model.vo.FacilityVisitCountVO;
import com.chinasoft.backend.model.vo.TotalHeadCountVO;
import com.chinasoft.backend.service.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/statistics")
public class StatisticsController {

    @Autowired
    RouteService routeService;

    @Autowired
    VisitService visitService;

    @Autowired
    MqttService mqttService;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Autowired
    TotalHeadcountService totalHeadcountService;

    /**
     * 统计推荐路线使用次数
     */
    @GetMapping("/route/useCount")
    public BaseResponse<List<Route>> useCount() {
        List<Route> routes = routeService.list();

        for (Route route : routes) {
            route.setCreateTime(null);
            route.setUpdateTime(null);
            route.setImgUrl(null);
            route.setIsDeleted(null);
        }

        return ResultUtils.success(routes);
    }

    /**
     * 统计每个设施的打卡次数
     */
    @GetMapping("/visit/count")
    public BaseResponse<List<FacilityVisitCountVO>> visitCount() {
        List<FacilityVisitCountVO> list = visitService.visitCount();


        return ResultUtils.success(list);
    }

    /**
     * 以小时为单位统计当日的拥挤度（预期等待时间）
     */
    @GetMapping("/crowingTimeCount")
    public BaseResponse<List<CrowingTimeCountVO>> crowingTimeCount() {

        List<CrowingTimeCountVO> list = crowdingLevelService.crowingTimeCount();

        return ResultUtils.success(list);
    }


    /**
     * 返回当前的今日游玩总人数
     *
     * @param
     * @return
     */
    @GetMapping("/headCount/total")
    public BaseResponse<Integer> getTotalCount() {

        // 查询数据库
        Integer data = mqttService.getTotalCount();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 按日期返回日期当天的游玩总人数
     *
     * @param
     * @return
     */
    @GetMapping("/headCount/totalArrangeByDate")
    public BaseResponse<List<TotalHeadCountVO>> getTotalCountArrangeByDate() {

        // 查询数据库
        List<TotalHeadCountVO> data = totalHeadcountService.getTotalCountArrangeByDate();

        // 返回响应
        return ResultUtils.success(data);
    }

    /**
     * 按设施id返回当前的今日游玩总人数
     *
     * @param
     * @return
     */
    @GetMapping("/headCount/facility")
    public BaseResponse<List<FacilityHeadCountVO>> getFacilityCount(@Param("facilityId") Integer facilityId) {

        // 查询数据库
        List<FacilityHeadCountVO> data = mqttService.getFacilityCount();

        // 返回响应
        return ResultUtils.success(data);
    }



}
