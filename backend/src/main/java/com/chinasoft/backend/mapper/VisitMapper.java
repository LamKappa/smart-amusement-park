package com.chinasoft.backend.mapper;

import com.chinasoft.backend.model.entity.Visit;
import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import org.apache.ibatis.annotations.Mapper;

/**
* @author 皎皎
* @description 针对表【visit(用户打卡记录表)】的数据库操作Mapper
* @createDate 2024-04-09 11:20:30
* @Entity com.chinasoft.backend.model.entity.Visit
*/
@Mapper
public interface VisitMapper extends BaseMapper<Visit> {

}




