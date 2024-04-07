package com.chinasoft.backend.service;

import com.chinasoft.backend.model.vo.RouteVO;

import java.util.List;

/**
* @author 皎皎
* @description 针对搜索的数据库操作Service
* @createDate 2024-04-05 16:57:10
*/
public interface RecommService {

    RouteVO getRecommdation(Integer routeId);

}
