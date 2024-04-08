package com.chinasoft.backend.mapper;

import com.chinasoft.backend.model.entity.Route;
import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import org.apache.ibatis.annotations.Mapper;

/**
* @author 皎皎
* @description 针对表【route(路线表)】的数据库操作Mapper
* @createDate 2024-04-07 18:18:32
* @Entity com.chinasoft.backend.model.entity.Route
*/
@Mapper
public interface RouteMapper extends BaseMapper<Route> {

}




