package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.route.RecommRoute;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 姜堂蕴之
 * @description 针对表【recomm_route(推荐路线表)】的数据库操作Mapper
 * @createDate 2024-04-07 18:18:52
 * @Entity com.chinasoft.backend.model.entity.route.RecommRoute
 */
@Mapper
public interface RecommRouteMapper extends BaseMapper<RecommRoute> {

}




