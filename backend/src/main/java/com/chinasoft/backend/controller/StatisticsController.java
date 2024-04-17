package com.chinasoft.backend.controller;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.route.Route;
import com.chinasoft.backend.model.vo.statistic.CrowingTimeCountVO;
import com.chinasoft.backend.model.vo.statistic.FacilityHeadCountVO;
import com.chinasoft.backend.model.vo.statistic.FacilityVisitCountVO;
import com.chinasoft.backend.model.vo.statistic.TotalHeadCountVO;
import com.chinasoft.backend.service.MqttService;
import com.chinasoft.backend.service.facility.CrowdingLevelService;
import com.chinasoft.backend.service.route.RouteService;
import com.chinasoft.backend.service.statistic.TotalHeadcountService;
import com.chinasoft.backend.service.visitsubscribe.VisitService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 数据统计接口
 *
 * @author 孟祥硕 姜堂蕴之
 */
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
     *
     * @author 孟祥硕
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
     *
     * @author 孟祥硕
     */
    @GetMapping("/visit/count")
    public BaseResponse<List<FacilityVisitCountVO>> visitCount() {
        List<FacilityVisitCountVO> list = visitService.visitCount();


        return ResultUtils.success(list);
    }

    /**
     * 以小时为单位统计当日的拥挤度（预期等待时间）
     *
     * @author 孟祥硕
     */
    @GetMapping("/crowingTimeCount")
    public BaseResponse<List<CrowingTimeCountVO>> crowingTimeCount() {

        List<CrowingTimeCountVO> list = crowdingLevelService.crowingTimeCount();

        return ResultUtils.success(list);
    }


    /**
     * 返回当前的今日游玩总人数
     *
     * @return BaseResponse<Integer> 包含今日游玩总人数的响应对象
     * @author 姜堂蕴之
     */
    @GetMapping("/headCount/total")
    public BaseResponse<Integer> getTotalCountFromRedis() {
        // 从Redis中查询数据库获取今日游玩总人数
        Integer data = mqttService.getTotalCountFromRedis();

        // 将查询结果封装为成功的响应对象并返回
        return ResultUtils.success(data);
    }

    /**
     * 按日期返回日期当天的游玩总人数
     *
     * @return BaseResponse<List<TotalHeadCountVO>> 包含按日期排列的游玩总人数列表的响应对象
     * @author 姜堂蕴之
     */
    @GetMapping("/headCount/totalArrangeByDate")
    public BaseResponse<List<TotalHeadCountVO>> getTotalCountArrangeByDate() {
        // 查询数据库获取按日期排列的游玩总人数列表
        List<TotalHeadCountVO> data = totalHeadcountService.getTotalCountArrangeByDate();
        
        // 将查询结果封装为成功的响应对象并返回
        return ResultUtils.success(data);
    }

    /**
     * 按设施id返回该设施的今日游玩总人数
     *
     * @return BaseResponse<List<FacilityHeadCountVO>> 包含按设施ID排列的游玩总人数列表的响应对象
     * @author 姜堂蕴之
     */
    @GetMapping("/headCount/facility")
    public BaseResponse<List<FacilityHeadCountVO>> getFacilityCount() {
        // 根据传入的设施ID查询数据库获取该设施的游玩总人数列表
        List<FacilityHeadCountVO> data = mqttService.getFacilityCount();

        // 将查询结果封装为成功的响应对象并返回
        return ResultUtils.success(data);
    }
}
