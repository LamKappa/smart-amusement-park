package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.RouteMapper;
import com.chinasoft.backend.model.entity.route.Route;
import com.chinasoft.backend.service.route.RouteService;
import org.springframework.stereotype.Service;

/**
 * 针对表【route(路线表)】的数据库操作Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class RouteServiceImpl extends ServiceImpl<RouteMapper, Route>
        implements RouteService {

}




