package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.RecommRouteMapper;
import com.chinasoft.backend.model.entity.route.RecommRoute;
import com.chinasoft.backend.service.route.RecommRouteService;
import org.springframework.stereotype.Service;

/**
 * 针对表【recomm_route(推荐路线表)】的数据库操作Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class RecommRouteServiceImpl extends ServiceImpl<RecommRouteMapper, RecommRoute>
        implements RecommRouteService {

}




