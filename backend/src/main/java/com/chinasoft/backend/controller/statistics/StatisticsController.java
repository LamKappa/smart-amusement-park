package com.chinasoft.backend.controller.statistics;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.Route;
import com.chinasoft.backend.model.vo.FacilityVisitCountVO;
import com.chinasoft.backend.service.RouteService;
import com.chinasoft.backend.service.VisitService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/statistics")
public class StatisticsController {

    @Autowired
    RouteService routeService;

    @Autowired
    VisitService visitService;

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
}
