package com.chinasoft.backend.controller.statistics;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.model.entity.Route;
import com.chinasoft.backend.service.RouteService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/statistics/route")
public class RouteStatisticsController {

    @Autowired
    RouteService routeService;

    @GetMapping("/useCount")
    public BaseResponse<List<Route>> userCount() {
        List<Route> routes = routeService.list();

        for (Route route : routes) {
            route.setCreateTime(null);
            route.setUpdateTime(null);
            route.setImgUrl(null);
            route.setIsDeleted(null);
        }

        return ResultUtils.success(routes);
    }
}
