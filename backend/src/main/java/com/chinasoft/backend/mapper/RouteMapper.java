package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.route.Route;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 姜堂蕴之
 * @description 针对表【route(路线表)】的数据库操作Mapper
 * @createDate 2024-04-07 18:18:32
 * @Entity com.chinasoft.backend.model.entity.route.Route
 */
@Mapper
public interface RouteMapper extends BaseMapper<Route> {

}




